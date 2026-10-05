#pragma once

#include "Math.h"
#include <unordered_map>
#include <string>
#include <cstdint>
#include <array>

namespace nexvora {

enum class TouchAction {
    Down = 0,
    Move = 1,
    Up = 2,
    PointerDown = 3,
    PointerUp = 4
};

struct TouchPoint {
    int id = -1;
    float x = 0.0f;
    float y = 0.0f;
    float nx = 0.0f; // normalized 0..1
    float ny = 0.0f;
    bool active = false;
};

// Virtual button
struct TouchButton {
    std::string name;
    float x = 0.0f, y = 0.0f;       // center in normalized coords
    float halfW = 0.1f, halfH = 0.1f;
    bool pressed = false;
    bool justPressed = false;
    bool justReleased = false;
    int boundPointer = -1;

    bool contains(float nx, float ny) const {
        return nx >= (x - halfW) && nx <= (x + halfW) &&
               ny >= (y - halfH) && ny <= (y + halfH);
    }
};

// Virtual joystick
struct TouchJoystick {
    std::string name;
    float cx = 0.2f, cy = 0.75f; // center normalized
    float radius = 0.12f;
    float axisX = 0.0f;
    float axisY = 0.0f;
    bool active = false;
    int boundPointer = -1;

    void updateFromTouch(float nx, float ny) {
        float dx = nx - cx;
        float dy = ny - cy;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len > radius && len > 1e-5f) {
            dx = dx / len * radius;
            dy = dy / len * radius;
            len = radius;
        }
        axisX = (radius > 1e-5f) ? (dx / radius) : 0.0f;
        axisY = (radius > 1e-5f) ? (dy / radius) : 0.0f;
    }

    void reset() {
        axisX = 0.0f;
        axisY = 0.0f;
        active = false;
        boundPointer = -1;
    }
};

struct InputAction {
    std::string name;
    bool pressed = false;
    bool justPressed = false;
    bool justReleased = false;
};

struct InputAxis {
    std::string name;
    float value = 0.0f;
};

class InputManager {
public:
    static InputManager& instance();

    void setScreenSize(int width, int height);
    void onTouch(int action, int pointerId, float x, float y);
    void update(float deltaTime);
    void reset();

    // Raw touch
    bool isTouchDown(int pointerId = 0) const;
    math::Vec2 touchPosition(int pointerId = 0) const;
    math::Vec2 touchNormalized(int pointerId = 0) const;
    int activeTouchCount() const;

    // Virtual controls
    TouchButton& createButton(const std::string& name, float nx, float ny, float halfW, float halfH);
    TouchJoystick& createJoystick(const std::string& name, float cx, float cy, float radius);
    TouchButton* getButton(const std::string& name);
    TouchJoystick* getJoystick(const std::string& name);

    // Actions / Axes
    void registerAction(const std::string& name);
    void registerAxis(const std::string& name);
    bool getAction(const std::string& name) const;
    bool getActionDown(const std::string& name) const;
    float getAxis(const std::string& name) const;
    void setAction(const std::string& name, bool pressed);
    void setAxis(const std::string& name, float value);

private:
    InputManager() = default;

    int m_screenW = 1;
    int m_screenH = 1;
    std::unordered_map<int, TouchPoint> m_touches;
    std::unordered_map<std::string, TouchButton> m_buttons;
    std::unordered_map<std::string, TouchJoystick> m_joysticks;
    std::unordered_map<std::string, InputAction> m_actions;
    std::unordered_map<std::string, InputAxis> m_axes;
};

} // namespace nexvora
