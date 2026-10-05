#include "InputManager.h"
#include "Logger.h"
#include <cmath>

namespace nexvora {

InputManager& InputManager::instance() {
    static InputManager inst;
    return inst;
}

void InputManager::setScreenSize(int width, int height) {
    m_screenW = width > 0 ? width : 1;
    m_screenH = height > 0 ? height : 1;
}

void InputManager::onTouch(int action, int pointerId, float x, float y) {
    TouchAction act = static_cast<TouchAction>(action);
    float nx = x / static_cast<float>(m_screenW);
    float ny = y / static_cast<float>(m_screenH);

    switch (act) {
        case TouchAction::Down:
        case TouchAction::PointerDown:
        case TouchAction::Move: {
            auto& tp = m_touches[pointerId];
            tp.id = pointerId;
            tp.x = x; tp.y = y;
            tp.nx = nx; tp.ny = ny;
            tp.active = true;

            // Update joysticks
            for (auto& pair : m_joysticks) {
                auto& joy = pair.second;
                if (joy.boundPointer == pointerId || (!joy.active && joy.boundPointer < 0)) {
                    float dx = nx - joy.cx;
                    float dy = ny - joy.cy;
                    float dist = std::sqrt(dx*dx + dy*dy);
                    if (dist <= joy.radius * 1.5f || joy.boundPointer == pointerId) {
                        joy.active = true;
                        joy.boundPointer = pointerId;
                        joy.updateFromTouch(nx, ny);
                    }
                }
            }

            // Update buttons
            for (auto& pair : m_buttons) {
                auto& btn = pair.second;
                if (btn.contains(nx, ny)) {
                    if (!btn.pressed) {
                        btn.justPressed = true;
                        btn.pressed = true;
                        btn.boundPointer = pointerId;
                    }
                }
            }
            break;
        }
        case TouchAction::Up:
        case TouchAction::PointerUp: {
            auto it = m_touches.find(pointerId);
            if (it != m_touches.end()) {
                it->second.active = false;
            }

            for (auto& pair : m_joysticks) {
                if (pair.second.boundPointer == pointerId) {
                    pair.second.reset();
                }
            }
            for (auto& pair : m_buttons) {
                if (pair.second.boundPointer == pointerId) {
                    if (pair.second.pressed) {
                        pair.second.justReleased = true;
                    }
                    pair.second.pressed = false;
                    pair.second.boundPointer = -1;
                }
            }
            break;
        }
        default:
            break;
    }
}

void InputManager::update(float /*deltaTime*/) {
    // Clear one-frame flags
    for (auto& pair : m_buttons) {
        pair.second.justPressed = false;
        pair.second.justReleased = false;
    }
    for (auto& pair : m_actions) {
        pair.second.justPressed = false;
        pair.second.justReleased = false;
    }

    // Sync joystick axes to named axes if registered
    for (auto& pair : m_joysticks) {
        auto& joy = pair.second;
        std::string axName = joy.name + "_X";
        std::string ayName = joy.name + "_Y";
        if (m_axes.count(axName)) m_axes[axName].value = joy.axisX;
        if (m_axes.count(ayName)) m_axes[ayName].value = joy.axisY;
    }
}

void InputManager::reset() {
    m_touches.clear();
    for (auto& p : m_joysticks) p.second.reset();
    for (auto& p : m_buttons) {
        p.second.pressed = false;
        p.second.boundPointer = -1;
    }
}

bool InputManager::isTouchDown(int pointerId) const {
    auto it = m_touches.find(pointerId);
    return it != m_touches.end() && it->second.active;
}

math::Vec2 InputManager::touchPosition(int pointerId) const {
    auto it = m_touches.find(pointerId);
    if (it != m_touches.end() && it->second.active)
        return {it->second.x, it->second.y};
    return {0, 0};
}

math::Vec2 InputManager::touchNormalized(int pointerId) const {
    auto it = m_touches.find(pointerId);
    if (it != m_touches.end() && it->second.active)
        return {it->second.nx, it->second.ny};
    return {0, 0};
}

int InputManager::activeTouchCount() const {
    int c = 0;
    for (const auto& p : m_touches) if (p.second.active) ++c;
    return c;
}

TouchButton& InputManager::createButton(const std::string& name, float nx, float ny, float halfW, float halfH) {
    TouchButton btn;
    btn.name = name;
    btn.x = nx; btn.y = ny;
    btn.halfW = halfW; btn.halfH = halfH;
    m_buttons[name] = btn;
    return m_buttons[name];
}

TouchJoystick& InputManager::createJoystick(const std::string& name, float cx, float cy, float radius) {
    TouchJoystick joy;
    joy.name = name;
    joy.cx = cx; joy.cy = cy;
    joy.radius = radius;
    m_joysticks[name] = joy;
    registerAxis(name + "_X");
    registerAxis(name + "_Y");
    return m_joysticks[name];
}

TouchButton* InputManager::getButton(const std::string& name) {
    auto it = m_buttons.find(name);
    return it != m_buttons.end() ? &it->second : nullptr;
}

TouchJoystick* InputManager::getJoystick(const std::string& name) {
    auto it = m_joysticks.find(name);
    return it != m_joysticks.end() ? &it->second : nullptr;
}

void InputManager::registerAction(const std::string& name) {
    if (!m_actions.count(name)) {
        InputAction a;
        a.name = name;
        m_actions[name] = a;
    }
}

void InputManager::registerAxis(const std::string& name) {
    if (!m_axes.count(name)) {
        InputAxis a;
        a.name = name;
        m_axes[name] = a;
    }
}

bool InputManager::getAction(const std::string& name) const {
    auto it = m_actions.find(name);
    return it != m_actions.end() && it->second.pressed;
}

bool InputManager::getActionDown(const std::string& name) const {
    auto it = m_actions.find(name);
    return it != m_actions.end() && it->second.justPressed;
}

float InputManager::getAxis(const std::string& name) const {
    auto it = m_axes.find(name);
    return it != m_axes.end() ? it->second.value : 0.0f;
}

void InputManager::setAction(const std::string& name, bool pressed) {
    auto& a = m_actions[name];
    if (pressed && !a.pressed) a.justPressed = true;
    if (!pressed && a.pressed) a.justReleased = true;
    a.pressed = pressed;
    a.name = name;
}

void InputManager::setAxis(const std::string& name, float value) {
    m_axes[name].name = name;
    m_axes[name].value = value;
}

} // namespace nexvora
