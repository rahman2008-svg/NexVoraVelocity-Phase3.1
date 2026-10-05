#pragma once

#include "Math.h"
#include "Transform.h"

namespace nexvora {

class Camera {
public:
    Camera() {
        m_transform.setPosition(0.0f, 8.0f, 18.0f);
    }

    void setPerspective(float fovyDegrees, float aspect, float zNear, float zFar) {
        m_fovy = fovyDegrees * math::DEG2RAD;
        m_aspect = aspect > 0.01f ? aspect : 1.0f;
        m_zNear = zNear;
        m_zFar = zFar;
        m_projectionDirty = true;
    }

    void setAspect(float aspect) {
        m_aspect = aspect > 0.01f ? aspect : 1.0f;
        m_projectionDirty = true;
    }

    void setFOV(float degrees) {
        m_fovy = degrees * math::DEG2RAD;
        m_projectionDirty = true;
    }

    float fovDegrees() const { return m_fovy * math::RAD2DEG; }
    float nearPlane() const { return m_zNear; }
    float farPlane() const { return m_zFar; }
    float aspect() const { return m_aspect; }

    Transform& transform() { return m_transform; }
    const Transform& transform() const { return m_transform; }

    void lookAt(const math::Vec3& target) {
        m_lookTarget = target;
        m_useLookAt = true;
    }

    void clearLookAt() { m_useLookAt = false; }

    // Smooth follow helpers (for Phase 3 chase camera prep)
    void setFollowTarget(const math::Vec3& target, float smooth = 5.0f) {
        m_followTarget = target;
        m_followSmooth = smooth;
        m_hasFollow = true;
    }

    void updateFollow(float dt) {
        if (!m_hasFollow) return;
        math::Vec3 pos = m_transform.position();
        math::Vec3 desired = m_followTarget + m_followOffset;
        pos = math::Vec3::lerp(pos, desired, math::clamp(m_followSmooth * dt, 0.0f, 1.0f));
        m_transform.setPosition(pos);
    }

    void setFollowOffset(const math::Vec3& offset) { m_followOffset = offset; }

    const math::Mat4& viewMatrix() {
        math::Vec3 eye = m_transform.position();
        math::Vec3 target;
        if (m_useLookAt) {
            target = m_lookTarget;
        } else {
            target = eye + m_transform.forward();
        }
        math::Vec3 up{0.0f, 1.0f, 0.0f};
        m_view = math::Mat4::lookAt(eye, target, up);
        return m_view;
    }

    const math::Mat4& projectionMatrix() {
        if (m_projectionDirty) {
            m_projection = math::Mat4::perspective(m_fovy, m_aspect, m_zNear, m_zFar);
            m_projectionDirty = false;
        }
        return m_projection;
    }

    math::Mat4 viewProjectionMatrix() {
        return projectionMatrix() * viewMatrix();
    }

private:
    Transform m_transform;
    float m_fovy = 55.0f * math::DEG2RAD;
    float m_aspect = 16.0f / 9.0f;
    float m_zNear = 0.1f;
    float m_zFar = 300.0f;
    math::Mat4 m_view;
    math::Mat4 m_projection;
    bool m_projectionDirty = true;

    bool m_useLookAt = true;
    math::Vec3 m_lookTarget{0.0f, 0.0f, 0.0f};

    bool m_hasFollow = false;
    math::Vec3 m_followTarget{0,0,0};
    math::Vec3 m_followOffset{0.0f, 6.0f, 12.0f};
    float m_followSmooth = 5.0f;
};

} // namespace nexvora
