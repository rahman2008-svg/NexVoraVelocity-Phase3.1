#include "VehicleController.h"
#include "Scene.h"
#include "Mesh.h"
#include "Logger.h"
#include <cmath>
#include <algorithm>

namespace nexvora {

void VehicleController::initialize(const VehicleConfig& config, const math::Vec3& startPos, float startYaw) {
    m_config = config;
    m_nitroAmount = config.nitroCapacity;
    reset(startPos, startYaw);
}

void VehicleController::reset(const math::Vec3& pos, float yaw) {
    m_transform.setPosition(pos);
    m_yaw = yaw;
    m_pitch = 0.0f;
    m_velocity = {0,0,0};
    m_speed = 0.0f;
    m_nitroActive = false;
    m_nitroCooldown = 0.0f;
    m_grounded = true;
    m_checkpointIndex = 0;
    m_lap = 0;
    m_waypointIndex = 0;
    m_trackProgress = 0.0f;
    m_finished = false;
    m_raceTime = 0.0f;
    m_currentLapTime = 0.0f;
    m_wheelSpin = 0.0f;
    applyOrientation();
}

void VehicleController::applyOrientation() {
    m_transform.setRotationEuler(m_pitch, m_yaw, 0.0f);
}

void VehicleController::applyImpulse(const math::Vec3& impulse) {
    m_velocity += impulse;
    m_speed = m_velocity.length();
}

void VehicleController::update(float dt, float steerInput, float throttleInput, float brakeInput, bool nitroPressed) {
    if (!m_enabled || m_finished) {
        // Still allow gravity settle
        if (!m_grounded) {
            m_velocity.y -= m_config.gravity * dt;
            auto p = m_transform.position();
            p += m_velocity * dt;
            if (p.y < 0.0f) { p.y = 0.0f; m_velocity.y = 0.0f; m_grounded = true; }
            m_transform.setPosition(p);
        }
        return;
    }
    integrate(dt, steerInput, throttleInput, brakeInput, nitroPressed);
    syncVisuals();
}

void VehicleController::integrate(float dt, float steer, float throttle, float brake, bool nitroPressed) {
    dt = std::min(dt, 0.05f);
    steer = math::clamp(steer, -1.0f, 1.0f);
    throttle = math::clamp(throttle, 0.0f, 1.0f);
    brake = math::clamp(brake, 0.0f, 1.0f);

    // Nitro
    m_nitroCooldown = std::max(0.0f, m_nitroCooldown - dt);
    m_nitroActive = false;
    if (nitroPressed && m_nitroAmount > 0.05f && m_nitroCooldown <= 0.0f && throttle > 0.1f) {
        m_nitroActive = true;
        m_nitroAmount = std::max(0.0f, m_nitroAmount - dt);
        if (m_nitroAmount <= 0.0f) m_nitroCooldown = 0.4f;
    } else if (!nitroPressed) {
        m_nitroAmount = std::min(m_config.nitroCapacity, m_nitroAmount + m_config.nitroRechargeRate * dt);
    }

    float speedRatio = m_config.maxSpeed > 0.0f ? math::clamp(m_speed / m_config.maxSpeed, 0.0f, 1.0f) : 0.0f;
    float steerMul = math::lerp(1.0f, m_config.steeringAtHighSpeed, speedRatio);
    float turnRate = m_config.steeringStrength * steerMul * m_config.turnResponsiveness;

    // Forward direction
    math::Vec3 forward{std::sin(m_yaw), 0.0f, std::cos(m_yaw)};
    math::Vec3 right{std::cos(m_yaw), 0.0f, -std::sin(m_yaw)};

    // Steering only when moving
    float steerEffect = steer * turnRate * dt * math::clamp(m_speed / 5.0f, 0.0f, 1.0f);
    if (m_speed > 0.5f) {
        // Reverse steering when going backwards
        float forwardDot = math::Vec3::dot(m_velocity.normalized(), forward);
        if (forwardDot < -0.1f) steerEffect = -steerEffect;
        m_yaw += steerEffect;
    }

    forward = {std::sin(m_yaw), 0.0f, std::cos(m_yaw)};
    right = {std::cos(m_yaw), 0.0f, -std::sin(m_yaw)};

    // Acceleration / brake / reverse
    float accel = 0.0f;
    if (throttle > 0.0f) {
        accel = m_config.acceleration * throttle;
        if (m_nitroActive) accel += m_config.nitroAcceleration;
    }
    if (brake > 0.0f) {
        if (m_speed > 1.5f) {
            // Braking
            math::Vec3 brakeDir = m_velocity.length() > 0.01f ? m_velocity.normalized() : forward;
            m_velocity -= brakeDir * (m_config.brakeForce * brake * dt);
        } else {
            // Reverse
            accel = -m_config.acceleration * 0.6f * brake;
        }
    }

    m_velocity += forward * (accel * dt);

    // Lateral grip / drift
    float lateral = math::Vec3::dot(m_velocity, right);
    float grip = m_config.grip;
    if (std::abs(steer) > 0.5f && m_speed > 15.0f) {
        grip *= (1.0f - m_config.driftFactor * std::abs(steer));
    }
    m_velocity -= right * (lateral * grip * dt * 8.0f);

    // Drag & friction
    m_velocity *= (1.0f - m_config.drag * dt);
    if (m_grounded) {
        float fric = m_config.friction * dt;
        float sp = m_velocity.length();
        if (sp > 0.01f) {
            m_velocity -= m_velocity.normalized() * std::min(sp, fric * 2.0f);
        }
    }

    // Gravity
    if (!m_grounded) {
        m_velocity.y -= m_config.gravity * dt;
    } else {
        m_velocity.y = 0.0f;
    }

    // Clamp max speed
    float maxSp = m_config.maxSpeed;
    if (m_nitroActive) maxSp += m_config.nitroMaxSpeedBonus;
    math::Vec3 horiz{m_velocity.x, 0.0f, m_velocity.z};
    float hsp = horiz.length();
    if (hsp > maxSp) {
        horiz = horiz.normalized() * maxSp;
        m_velocity.x = horiz.x;
        m_velocity.z = horiz.z;
    }

    // Integrate position
    auto pos = m_transform.position();
    pos += m_velocity * dt;

    // Ground
    if (pos.y <= 0.0f) {
        pos.y = 0.0f;
        if (m_velocity.y < 0.0f) m_velocity.y = 0.0f;
        m_grounded = true;
    } else {
        m_grounded = false;
    }

    m_transform.setPosition(pos);
    m_speed = math::Vec3{m_velocity.x, 0.0f, m_velocity.z}.length();
    m_wheelSpin += m_speed * dt * 2.5f;
    applyOrientation();
}

void VehicleController::buildVisuals(Scene& scene, const std::string& namePrefix) {
    m_visualRootName = namePrefix;
    m_wheelNames.clear();

    const auto& col = m_config.bodyColor;
    const auto& acc = m_config.accentColor;

    // Body
    {
        auto& body = scene.createObject(namePrefix + "_body");
        body.setMesh(Mesh::createBox(1.8f, 0.45f, 3.6f));
        body.setColor(col);
        body.transform().setPosition(0, 0.45f, 0);
    }
    // Cabin
    {
        auto& cabin = scene.createObject(namePrefix + "_cabin");
        cabin.setMesh(Mesh::createBox(1.4f, 0.4f, 1.4f));
        cabin.setColor(col * 0.85f);
        cabin.transform().setPosition(0, 0.85f, -0.2f);
    }
    // Windshield (dark)
    {
        auto& ws = scene.createObject(namePrefix + "_ws");
        ws.setMesh(Mesh::createBox(1.35f, 0.08f, 0.9f));
        ws.setColor({0.15f, 0.2f, 0.28f});
        ws.transform().setPosition(0, 0.95f, 0.35f);
    }
    // Spoiler
    {
        auto& sp = scene.createObject(namePrefix + "_spoiler");
        sp.setMesh(Mesh::createBox(1.6f, 0.08f, 0.35f));
        sp.setColor(acc);
        sp.transform().setPosition(0, 0.95f, -1.7f);
    }
    // Headlights
    {
        auto& hl = scene.createObject(namePrefix + "_hl");
        hl.setMesh(Mesh::createBox(1.5f, 0.12f, 0.15f));
        hl.setColor({1.0f, 0.95f, 0.7f});
        hl.transform().setPosition(0, 0.4f, 1.75f);
    }
    // Rear lights
    {
        auto& rl = scene.createObject(namePrefix + "_rl");
        rl.setMesh(Mesh::createBox(1.5f, 0.1f, 0.12f));
        rl.setColor({0.95f, 0.1f, 0.1f});
        rl.transform().setPosition(0, 0.4f, -1.75f);
    }
    // Exhaust
    {
        auto& ex = scene.createObject(namePrefix + "_ex");
        ex.setMesh(Mesh::createCylinder(0.08f, 0.3f, 6));
        ex.setColor({0.2f, 0.2f, 0.22f});
        ex.transform().setPosition(0.5f, 0.2f, -1.9f);
    }

    // Wheels
    const float wheelX[4] = {-0.85f, 0.85f, -0.85f, 0.85f};
    const float wheelZ[4] = {1.15f, 1.15f, -1.15f, -1.15f};
    for (int i = 0; i < 4; ++i) {
        std::string wn = namePrefix + "_wheel" + std::to_string(i);
        m_wheelNames.push_back(wn);
        auto& w = scene.createObject(wn);
        w.setMesh(Mesh::createCylinder(0.32f, 0.28f, 10));
        w.setColor({0.12f, 0.12f, 0.12f});
        w.transform().setPosition(wheelX[i], 0.32f, wheelZ[i]);
        // Rotate cylinder to align as wheel (X axis)
        w.transform().setRotationEuler(0.0f, 0.0f, math::HALF_PI);
    }
}

void VehicleController::syncVisuals() {
    // Visual root parts are updated by RaceManager which holds scene refs
    // Wheel spin is applied there for simplicity
    (void)m_wheelSpin;
}

} // namespace nexvora
