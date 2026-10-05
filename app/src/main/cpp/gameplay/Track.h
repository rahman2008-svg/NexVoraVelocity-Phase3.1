#pragma once

#include "Math.h"
#include "Scene.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include <cmath>

namespace nexvora {

struct Waypoint {
    math::Vec3 position;
    float width = 8.0f;
    float yaw = 0.0f;
};

struct Checkpoint {
    math::Vec3 position;
    float radius = 12.0f;
    int index = 0;
    bool isStartFinish = false;
};

struct RespawnPoint {
    math::Vec3 position;
    float yaw = 0.0f;
    int checkpointIndex = 0;
};

struct TrackSegment {
    math::Vec3 center;
    float yaw = 0.0f;
    float length = 12.0f;
    float width = 8.0f;
};

class Track {
public:
    void buildFuturisticCircuit(Scene& scene);
    void clear();

    const std::vector<Waypoint>& waypoints() const { return m_waypoints; }
    const std::vector<Checkpoint>& checkpoints() const { return m_checkpoints; }
    const std::vector<RespawnPoint>& respawnPoints() const { return m_respawnPoints; }
    const std::vector<TrackSegment>& segments() const { return m_segments; }

    int waypointCount() const { return static_cast<int>(m_waypoints.size()); }
    int checkpointCount() const { return static_cast<int>(m_checkpoints.size()); }

    // Closest waypoint index and distance along track progress 0..1
    int nearestWaypoint(const math::Vec3& pos) const;
    float progressAlongTrack(const math::Vec3& pos, int hintWp = 0) const;
    math::Vec3 waypointDirection(int index) const;

    // Barrier collision: returns true if hit and outputs push-out
    bool collideBarriers(const math::Vec3& pos, float radius, math::Vec3& outPush) const;

    // Off-track detection
    bool isOffTrack(const math::Vec3& pos, float maxDist = 18.0f) const;

    RespawnPoint getRespawnForCheckpoint(int cpIndex) const;

    const math::Vec3& startPosition() const { return m_startPos; }
    float startYaw() const { return m_startYaw; }

private:
    void addWaypoint(const math::Vec3& p, float yaw, float width = 8.0f);
    void addCheckpoint(const math::Vec3& p, float radius, bool startFinish = false);
    void addBarrierBox(Scene& scene, const math::Vec3& pos, const math::Vec3& size, float yaw, const math::Vec3& color);
    void addBuilding(Scene& scene, const math::Vec3& pos, const math::Vec3& size, const math::Vec3& color);
    void addTree(Scene& scene, const math::Vec3& pos);
    void addStreetLight(Scene& scene, const math::Vec3& pos);

    std::vector<Waypoint> m_waypoints;
    std::vector<Checkpoint> m_checkpoints;
    std::vector<RespawnPoint> m_respawnPoints;
    std::vector<TrackSegment> m_segments;

    // Barrier AABBs for collision (axis-aligned approx)
    struct BarrierAABB {
        math::Vec3 min;
        math::Vec3 max;
    };
    std::vector<BarrierAABB> m_barriers;

    math::Vec3 m_startPos{0, 0, 0};
    float m_startYaw = 0.0f;
    int m_objCounter = 0;
};

} // namespace nexvora
