#pragma once

#include "Math.h"
#include <vector>
#include <cmath>

namespace nexvora {
namespace physics {

struct AABB {
    math::Vec3 min;
    math::Vec3 max;

    bool intersects(const AABB& o) const {
        return (min.x <= o.max.x && max.x >= o.min.x) &&
               (min.y <= o.max.y && max.y >= o.min.y) &&
               (min.z <= o.max.z && max.z >= o.min.z);
    }

    math::Vec3 center() const { return (min + max) * 0.5f; }
    math::Vec3 extents() const { return (max - min) * 0.5f; }
};

struct Sphere {
    math::Vec3 center;
    float radius = 1.0f;

    bool intersects(const Sphere& o) const {
        float d = math::Vec3::distance(center, o.center);
        return d <= (radius + o.radius);
    }

    bool intersects(const AABB& box) const {
        math::Vec3 closest{
            math::clamp(center.x, box.min.x, box.max.x),
            math::clamp(center.y, box.min.y, box.max.y),
            math::clamp(center.z, box.min.z, box.max.z)
        };
        return math::Vec3::distance(center, closest) <= radius;
    }
};

struct Ray {
    math::Vec3 origin;
    math::Vec3 direction;

    bool intersectSphere(const Sphere& s, float& t) const {
        math::Vec3 oc = origin - s.center;
        float a = math::Vec3::dot(direction, direction);
        float b = 2.0f * math::Vec3::dot(oc, direction);
        float c = math::Vec3::dot(oc, oc) - s.radius * s.radius;
        float disc = b * b - 4.0f * a * c;
        if (disc < 0.0f) return false;
        t = (-b - std::sqrt(disc)) / (2.0f * a);
        return t >= 0.0f;
    }

    bool intersectAABB(const AABB& box, float& t) const {
        float tmin = 0.0f, tmax = 1e30f;
        for (int i = 0; i < 3; ++i) {
            float o = (&origin.x)[i];
            float d = (&direction.x)[i];
            float mn = (&box.min.x)[i];
            float mx = (&box.max.x)[i];
            if (std::abs(d) < 1e-8f) {
                if (o < mn || o > mx) return false;
            } else {
                float t1 = (mn - o) / d;
                float t2 = (mx - o) / d;
                if (t1 > t2) std::swap(t1, t2);
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
                if (tmin > tmax) return false;
            }
        }
        t = tmin;
        return true;
    }
};

struct RigidBody {
    math::Vec3 position{0,0,0};
    math::Vec3 velocity{0,0,0};
    math::Vec3 acceleration{0,0,0};
    float mass = 1.0f;
    float friction = 0.8f;
    float drag = 0.1f;
    bool useGravity = true;
    bool isGrounded = false;

    void integrate(float dt, float gravity = -9.81f) {
        if (useGravity && !isGrounded) {
            acceleration.y += gravity;
        }
        velocity += acceleration * dt;
        // Drag
        velocity *= (1.0f - drag * dt);
        // Friction when grounded
        if (isGrounded) {
            velocity.x *= (1.0f - friction * dt);
            velocity.z *= (1.0f - friction * dt);
            if (velocity.y < 0.0f) velocity.y = 0.0f;
        }
        position += velocity * dt;
        acceleration = {0,0,0};
    }

    void addForce(const math::Vec3& force) {
        acceleration += force / mass;
    }
};

// Simple collision response
inline void resolveSphereSphere(Sphere& a, Sphere& b, float restitution = 0.3f) {
    math::Vec3 delta = b.center - a.center;
    float dist = delta.length();
    float overlap = a.radius + b.radius - dist;
    if (overlap <= 0.0f || dist < 1e-6f) return;
    math::Vec3 n = delta / dist;
    a.center -= n * (overlap * 0.5f);
    b.center += n * (overlap * 0.5f);
}

inline void resolveSphereAABB(Sphere& s, const AABB& box) {
    math::Vec3 closest{
        math::clamp(s.center.x, box.min.x, box.max.x),
        math::clamp(s.center.y, box.min.y, box.max.y),
        math::clamp(s.center.z, box.min.z, box.max.z)
    };
    math::Vec3 delta = s.center - closest;
    float dist = delta.length();
    if (dist < s.radius && dist > 1e-6f) {
        math::Vec3 n = delta / dist;
        s.center = closest + n * s.radius;
    }
}

} // namespace physics
} // namespace nexvora
