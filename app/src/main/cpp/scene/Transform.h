#pragma once
#include <algorithm>

#include "Math.h"
#include <vector>
#include <memory>
#include <cstdint>

namespace nexvora {

class Transform {
public:
    Transform() = default;

    void setPosition(const math::Vec3& pos) { m_position = pos; markDirty(); }
    void setPosition(float x, float y, float z) { m_position = {x, y, z}; markDirty(); }
    const math::Vec3& position() const { return m_position; }

    void setRotation(const math::Quat& q) { m_rotation = q.normalized(); markDirty(); }
    void setRotationEuler(float pitch, float yaw, float roll) {
        m_rotation = math::Quat::fromEuler(pitch, yaw, roll);
        markDirty();
    }
    void setRotationEuler(const math::Vec3& euler) {
        setRotationEuler(euler.x, euler.y, euler.z);
    }
    const math::Quat& rotation() const { return m_rotation; }
    math::Vec3 eulerAngles() const {
        // Approximate extraction
        float sinr_cosp = 2.0f * (m_rotation.w * m_rotation.x + m_rotation.y * m_rotation.z);
        float cosr_cosp = 1.0f - 2.0f * (m_rotation.x * m_rotation.x + m_rotation.y * m_rotation.y);
        float roll = std::atan2(sinr_cosp, cosr_cosp);

        float sinp = 2.0f * (m_rotation.w * m_rotation.y - m_rotation.z * m_rotation.x);
        float pitch = std::abs(sinp) >= 1.0f ? std::copysign(math::HALF_PI, sinp) : std::asin(sinp);

        float siny_cosp = 2.0f * (m_rotation.w * m_rotation.z + m_rotation.x * m_rotation.y);
        float cosy_cosp = 1.0f - 2.0f * (m_rotation.y * m_rotation.y + m_rotation.z * m_rotation.z);
        float yaw = std::atan2(siny_cosp, cosy_cosp);

        return {pitch, yaw, roll};
    }

    void setScale(const math::Vec3& s) { m_scale = s; markDirty(); }
    void setScale(float x, float y, float z) { m_scale = {x, y, z}; markDirty(); }
    void setScale(float uniform) { m_scale = {uniform, uniform, uniform}; markDirty(); }
    const math::Vec3& scale() const { return m_scale; }

    void translate(const math::Vec3& delta) { m_position += delta; markDirty(); }
    void rotate(const math::Quat& q) { m_rotation = (m_rotation * q).normalized(); markDirty(); }
    void rotateY(float radians) {
        m_rotation = (m_rotation * math::Quat::fromAxisAngle({0,1,0}, radians)).normalized();
        markDirty();
    }

    // Hierarchy
    void setParent(Transform* parent) {
        if (m_parent == parent) return;
        if (m_parent) {
            auto& children = m_parent->m_children;
            children.erase(std::remove(children.begin(), children.end(), this), children.end());
        }
        m_parent = parent;
        if (m_parent) {
            m_parent->m_children.push_back(this);
        }
        markDirty();
    }

    Transform* parent() const { return m_parent; }
    const std::vector<Transform*>& children() const { return m_children; }

    const math::Mat4& localMatrix() {
        if (m_localDirty) rebuildLocal();
        return m_localMatrix;
    }

    const math::Mat4& worldMatrix() {
        updateWorld();
        return m_worldMatrix;
    }

    void markDirty() {
        m_localDirty = true;
        m_worldDirty = true;
        for (Transform* child : m_children) {
            if (child) child->markDirty();
        }
    }

    math::Vec3 forward() const {
        return m_rotation.rotate({0.0f, 0.0f, -1.0f});
    }

    math::Vec3 right() const {
        return m_rotation.rotate({1.0f, 0.0f, 0.0f});
    }

    math::Vec3 up() const {
        return m_rotation.rotate({0.0f, 1.0f, 0.0f});
    }

private:
    void rebuildLocal() {
        m_localMatrix = math::Mat4::TRS(m_position, m_rotation, m_scale);
        m_localDirty = false;
    }

    void updateWorld() {
        if (!m_worldDirty && !m_localDirty) return;
        if (m_localDirty) rebuildLocal();
        if (m_parent) {
            m_worldMatrix = m_parent->worldMatrix() * m_localMatrix;
        } else {
            m_worldMatrix = m_localMatrix;
        }
        m_worldDirty = false;
    }

    math::Vec3 m_position{0.0f, 0.0f, 0.0f};
    math::Quat m_rotation = math::Quat::identity();
    math::Vec3 m_scale{1.0f, 1.0f, 1.0f};

    math::Mat4 m_localMatrix;
    math::Mat4 m_worldMatrix;
    bool m_localDirty = true;
    bool m_worldDirty = true;

    Transform* m_parent = nullptr;
    std::vector<Transform*> m_children;
};

} // namespace nexvora
