#include "AIController.h"
#include "Math.h"
#include <cmath>
#include <algorithm>

namespace nexvora {

void AIController::initialize(VehicleController* vehicle, const Track* track, Difficulty diff, int aiIndex) {
    m_vehicle = vehicle;
    m_track = track;
    m_diff = diff;
    m_aiIndex = aiIndex;
    m_targetWp = 1;
    m_reactionTimer = 0.0f;
    m_steerSmooth = 0.0f;

    switch (diff) {
        case Difficulty::Easy:   m_speedMul = 0.72f; break;
        case Difficulty::Hard:   m_speedMul = 0.98f; break;
        default:                 m_speedMul = 0.88f; break;
    }
    // Slight per-AI variation
    m_speedMul += (aiIndex - 1) * 0.03f;
}

void AIController::update(float dt) {
    if (!m_vehicle || !m_track || m_vehicle->finished() || !m_vehicle->isEnabled()) return;

    const auto& wps = m_track->waypoints();
    if (wps.empty()) return;
    int n = static_cast<int>(wps.size());

    math::Vec3 pos = m_vehicle->position();
    int nearest = m_track->nearestWaypoint(pos);
    m_vehicle->setWaypointIndex(nearest);

    // Target a few waypoints ahead
    int lookAhead = (m_diff == Difficulty::Hard) ? 3 : (m_diff == Difficulty::Easy) ? 2 : 3;
    m_targetWp = (nearest + lookAhead) % n;

    math::Vec3 target = wps[m_targetWp].position;
    math::Vec3 toTarget = target - pos;
    toTarget.y = 0.0f;
    float dist = toTarget.length();
    math::Vec3 dir = dist > 0.1f ? toTarget.normalized() : m_track->waypointDirection(nearest);

    // Desired yaw
    float desiredYaw = std::atan2(dir.x, dir.z);
    float currentYaw = m_vehicle->yaw();
    float yawDiff = desiredYaw - currentYaw;
    // Normalize to [-pi, pi]
    while (yawDiff > math::PI) yawDiff -= math::TWO_PI;
    while (yawDiff < -math::PI) yawDiff += math::TWO_PI;

    float steer = math::clamp(yawDiff * 1.8f, -1.0f, 1.0f);
    // Smooth steering
    float smoothRate = (m_diff == Difficulty::Hard) ? 10.0f : 6.0f;
    m_steerSmooth = math::lerp(m_steerSmooth, steer, math::clamp(smoothRate * dt, 0.0f, 1.0f));

    // Speed control - slow for corners
    float cornerFactor = 1.0f - math::clamp(std::abs(yawDiff) / 1.2f, 0.0f, 0.55f);
    float throttle = 0.85f * m_speedMul * cornerFactor;
    float brake = 0.0f;
    if (std::abs(yawDiff) > 0.9f && m_vehicle->speed() > 20.0f) {
        brake = 0.4f;
        throttle = 0.2f;
    }

    // Nitro occasionally on straights
    bool nitro = false;
    if (m_diff != Difficulty::Easy && std::abs(yawDiff) < 0.25f && m_vehicle->speed() > 25.0f) {
        if (m_vehicle->nitroNormalized() > 0.3f && (m_aiIndex + static_cast<int>(m_vehicle->raceTime() * 2)) % 7 == 0) {
            nitro = true;
        }
    }

    // Recovery if stuck
    if (m_vehicle->speed() < 1.0f) {
        throttle = 1.0f;
        brake = 0.0f;
        m_steerSmooth = (m_aiIndex % 2 == 0) ? 0.5f : -0.5f;
    }

    m_vehicle->update(dt, m_steerSmooth, throttle, brake, nitro);

    // Progress
    float prog = m_track->progressAlongTrack(pos, nearest);
    m_vehicle->setTrackProgress(prog + m_vehicle->lap());
}

} // namespace nexvora
