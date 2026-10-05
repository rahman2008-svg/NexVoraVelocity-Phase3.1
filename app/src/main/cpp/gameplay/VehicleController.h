#pragma once

#include "VehicleConfig.h"
#include "Transform.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Math.h"
#include <memory>
#include <vector>
#include <string>

namespace nexvora {

class VehicleController {
public:
    VehicleController() = default;

    void initialize(const VehicleConfig& config, const math::Vec3& startPos, float startYaw);
    void reset(const math::Vec3& pos, float yaw);
    void update(float dt, float steerInput, float throttleInput, float brakeInput, bool nitroPressed);

    // Build visual mesh hierarchy into scene objects
    void buildVisuals(class Scene& scene, const std::string& namePrefix);
    void syncVisuals();

    Transform& transform() { return m_transform; }
    const Transform& transform() const { return m_transform; }

    float speed() const { return m_speed; }
    float speedKmh() const { return m_speed * 3.6f; }
    float nitroAmount() const { return m_nitroAmount; }
    float nitroNormalized() const { return m_config.nitroCapacity > 0.0f ? m_nitroAmount / m_config.nitroCapacity : 0.0f; }
    bool isNitroActive() const { return m_nitroActive; }
    bool isGrounded() const { return m_grounded; }
    float yaw() const { return m_yaw; }
    const VehicleConfig& config() const { return m_config; }
    math::Vec3 velocity() const { return m_velocity; }
    math::Vec3 position() const { return m_transform.position(); }

    void setPosition(const math::Vec3& p) { m_transform.setPosition(p); }
    void setYaw(float yaw) { m_yaw = yaw; applyOrientation(); }
    void setVelocity(const math::Vec3& v) { m_velocity = v; m_speed = v.length(); }
    void applyImpulse(const math::Vec3& impulse);
    void setEnabled(bool e) { m_enabled = e; }
    bool isEnabled() const { return m_enabled; }

    // Collision radius for sphere checks
    float collisionRadius() const { return 1.6f; }
    math::Vec3 collisionCenter() const {
        auto p = m_transform.position();
        p.y += 0.5f;
        return p;
    }

    int checkpointIndex() const { return m_checkpointIndex; }
    void setCheckpointIndex(int i) { m_checkpointIndex = i; }
    int lap() const { return m_lap; }
    void setLap(int l) { m_lap = l; }
    int waypointIndex() const { return m_waypointIndex; }
    void setWaypointIndex(int i) { m_waypointIndex = i; }
    float trackProgress() const { return m_trackProgress; }
    void setTrackProgress(float p) { m_trackProgress = p; }
    bool finished() const { return m_finished; }
    void setFinished(bool f) { m_finished = f; }
    float raceTime() const { return m_raceTime; }
    void setRaceTime(float t) { m_raceTime = t; }
    float bestLapTime() const { return m_bestLapTime; }
    void setBestLapTime(float t) { m_bestLapTime = t; }
    float currentLapTime() const { return m_currentLapTime; }
    void setCurrentLapTime(float t) { m_currentLapTime = t; }

    bool isPlayer() const { return m_isPlayer; }
    void setIsPlayer(bool p) { m_isPlayer = p; }

    const std::string& visualRootName() const { return m_visualRootName; }

private:
    void applyOrientation();
    void integrate(float dt, float steer, float throttle, float brake, bool nitro);

    VehicleConfig m_config;
    Transform m_transform;
    math::Vec3 m_velocity{0,0,0};
    float m_speed = 0.0f;
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_nitroAmount = 0.0f;
    bool m_nitroActive = false;
    float m_nitroCooldown = 0.0f;
    bool m_grounded = true;
    bool m_enabled = true;
    bool m_isPlayer = false;

    int m_checkpointIndex = 0;
    int m_lap = 0;
    int m_waypointIndex = 0;
    float m_trackProgress = 0.0f;
    bool m_finished = false;
    float m_raceTime = 0.0f;
    float m_bestLapTime = 0.0f;
    float m_currentLapTime = 0.0f;

    // Visual parts (names in scene)
    std::string m_visualRootName;
    std::vector<std::string> m_wheelNames;
    float m_wheelSpin = 0.0f;
};

} // namespace nexvora
