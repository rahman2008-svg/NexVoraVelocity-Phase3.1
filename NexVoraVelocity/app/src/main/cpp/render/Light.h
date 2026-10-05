#pragma once

#include "Math.h"

namespace nexvora {

enum class LightType {
    Directional = 0,
    Point = 1
};

struct Light {
    LightType type = LightType::Directional;
    math::Vec3 position{0.0f, 10.0f, 0.0f};
    math::Vec3 direction{-0.3f, -1.0f, -0.2f};
    math::Vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;
    float range = 50.0f;
    bool enabled = true;

    static Light directional(const math::Vec3& dir, const math::Vec3& color, float intensity = 1.0f) {
        Light l;
        l.type = LightType::Directional;
        l.direction = dir.normalized();
        l.color = color;
        l.intensity = intensity;
        return l;
    }

    static Light point(const math::Vec3& pos, const math::Vec3& color, float intensity = 1.0f, float range = 20.0f) {
        Light l;
        l.type = LightType::Point;
        l.position = pos;
        l.color = color;
        l.intensity = intensity;
        l.range = range;
        return l;
    }
};

struct AmbientLight {
    math::Vec3 color{0.2f, 0.22f, 0.25f};
    float intensity = 1.0f;
};

} // namespace nexvora
