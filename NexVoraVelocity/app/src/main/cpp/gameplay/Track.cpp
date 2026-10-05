#include "Track.h"
#include "Logger.h"
#include <algorithm>
#include <cmath>

namespace nexvora {

void Track::clear() {
    m_waypoints.clear();
    m_checkpoints.clear();
    m_respawnPoints.clear();
    m_segments.clear();
    m_barriers.clear();
    m_objCounter = 0;
}

void Track::addWaypoint(const math::Vec3& p, float yaw, float width) {
    Waypoint wp;
    wp.position = p;
    wp.yaw = yaw;
    wp.width = width;
    m_waypoints.push_back(wp);
}

void Track::addCheckpoint(const math::Vec3& p, float radius, bool startFinish) {
    Checkpoint cp;
    cp.position = p;
    cp.radius = radius;
    cp.index = static_cast<int>(m_checkpoints.size());
    cp.isStartFinish = startFinish;
    m_checkpoints.push_back(cp);

    RespawnPoint rp;
    rp.position = p;
    rp.position.y = 0.0f;
    // yaw toward next waypoint roughly
    rp.yaw = m_waypoints.empty() ? 0.0f : m_waypoints.back().yaw;
    rp.checkpointIndex = cp.index;
    m_respawnPoints.push_back(rp);
}

void Track::addBarrierBox(Scene& scene, const math::Vec3& pos, const math::Vec3& size, float yaw, const math::Vec3& color) {
    std::string name = "Barrier_" + std::to_string(m_objCounter++);
    auto& obj = scene.createObject(name);
    obj.setMesh(Mesh::createBox(size.x, size.y, size.z));
    obj.setColor(color);
    obj.transform().setPosition(pos);
    obj.transform().setRotationEuler(0, yaw, 0);

    // Approximate AABB (ignore rotation for simplicity - barriers mostly axis aligned)
    BarrierAABB b;
    float hx = size.x * 0.5f + 0.3f;
    float hy = size.y * 0.5f;
    float hz = size.z * 0.5f + 0.3f;
    b.min = {pos.x - hx, pos.y - hy, pos.z - hz};
    b.max = {pos.x + hx, pos.y + hy, pos.z + hz};
    m_barriers.push_back(b);
}

void Track::addBuilding(Scene& scene, const math::Vec3& pos, const math::Vec3& size, const math::Vec3& color) {
    auto& obj = scene.createObject("Bld_" + std::to_string(m_objCounter++));
    obj.setMesh(Mesh::createBox(size.x, size.y, size.z));
    obj.setColor(color);
    obj.transform().setPosition({pos.x, size.y * 0.5f, pos.z});
}

void Track::addTree(Scene& scene, const math::Vec3& pos) {
    auto& trunk = scene.createObject("TreeT_" + std::to_string(m_objCounter++));
    trunk.setMesh(Mesh::createCylinder(0.22f, 1.5f, 6));
    trunk.setColor({0.38f, 0.26f, 0.14f});
    trunk.transform().setPosition({pos.x, 0.75f, pos.z});

    auto& leaf = scene.createObject("TreeL_" + std::to_string(m_objCounter++));
    leaf.setMesh(Mesh::createSphere(1.0f, 8));
    leaf.setColor({0.18f, 0.52f, 0.22f});
    leaf.transform().setPosition({pos.x, 2.0f, pos.z});
}

void Track::addStreetLight(Scene& scene, const math::Vec3& pos) {
    auto& pole = scene.createObject("LightP_" + std::to_string(m_objCounter++));
    pole.setMesh(Mesh::createCylinder(0.08f, 4.0f, 6));
    pole.setColor({0.3f, 0.3f, 0.35f});
    pole.transform().setPosition({pos.x, 2.0f, pos.z});

    auto& lamp = scene.createObject("LightL_" + std::to_string(m_objCounter++));
    lamp.setMesh(Mesh::createSphere(0.35f, 6));
    lamp.setColor({1.0f, 0.9f, 0.6f});
    lamp.transform().setPosition({pos.x, 4.1f, pos.z});
}

void Track::buildFuturisticCircuit(Scene& scene) {
    clear();
    NV_LOGI("Building futuristic racing circuit...");

    // Ground
    {
        auto& ground = scene.createObject("TrackGround");
        ground.setMesh(Mesh::createPlane(200.0f, 6));
        ground.setColor({0.22f, 0.28f, 0.20f});
    }

    // Define an oval-ish circuit with waypoints
    // Track roughly centered at origin, elongated on Z
    const float trackHalfW = 5.0f;
    const int numPoints = 48;
    const float radiusX = 45.0f;
    const float radiusZ = 70.0f;

    for (int i = 0; i < numPoints; ++i) {
        float t = static_cast<float>(i) / numPoints * math::TWO_PI;
        // Rounded rectangle / oval
        float x = std::sin(t) * radiusX;
        float z = -std::cos(t) * radiusZ;
        float yaw = t + math::HALF_PI;
        addWaypoint({x, 0.0f, z}, yaw, trackHalfW * 2.0f);

        TrackSegment seg;
        seg.center = {x, 0.0f, z};
        seg.yaw = yaw;
        seg.length = 10.0f;
        seg.width = trackHalfW * 2.0f;
        m_segments.push_back(seg);
    }

    // Road surfaces along waypoints
    for (size_t i = 0; i < m_waypoints.size(); ++i) {
        const auto& wp = m_waypoints[i];
        auto& road = scene.createObject("Road_" + std::to_string(static_cast<int>(i)));
        road.setMesh(Mesh::createRoadSegment(14.0f, trackHalfW * 2.0f));
        road.setColor({0.16f, 0.16f, 0.18f});
        road.transform().setPosition(wp.position);
        road.transform().setRotationEuler(0, wp.yaw, 0);
    }

    // Barriers along both sides
    for (size_t i = 0; i < m_waypoints.size(); ++i) {
        const auto& wp = m_waypoints[i];
        math::Vec3 right{std::cos(wp.yaw), 0, -std::sin(wp.yaw)};
        math::Vec3 left = right * -1.0f;

        math::Vec3 bPosR = wp.position + right * (trackHalfW + 0.6f);
        math::Vec3 bPosL = wp.position + left * (trackHalfW + 0.6f);
        bPosR.y = 0.4f;
        bPosL.y = 0.4f;

        math::Vec3 barrierColor = (i % 2 == 0) ? math::Vec3{0.9f, 0.75f, 0.1f} : math::Vec3{0.9f, 0.2f, 0.15f};
        addBarrierBox(scene, bPosR, {0.5f, 0.8f, 12.0f}, wp.yaw, barrierColor);
        addBarrierBox(scene, bPosL, {0.5f, 0.8f, 12.0f}, wp.yaw, barrierColor);

        // Street lights every 4th
        if (i % 4 == 0) {
            addStreetLight(scene, wp.position + right * (trackHalfW + 3.5f));
            addStreetLight(scene, wp.position + left * (trackHalfW + 3.5f));
        }
    }

    // Checkpoints every ~1/6 of track + start/finish
    int cpStep = numPoints / 6;
    for (int i = 0; i < numPoints; i += cpStep) {
        bool isSF = (i == 0);
        addCheckpoint(m_waypoints[i].position, 14.0f, isSF);
    }
    // Ensure last connects - start/finish is index 0
    if (m_checkpoints.empty()) {
        addCheckpoint(m_waypoints[0].position, 14.0f, true);
    }

    // Start position slightly before start/finish facing along track
    m_startPos = m_waypoints[0].position;
    m_startPos.y = 0.0f;
    // Offset to grid positions later
    m_startYaw = m_waypoints[0].yaw;

    // Scenery buildings outside track
    struct BDef { float x,z,w,h,d; math::Vec3 c; };
    BDef blds[] = {
        {-70,  40, 12, 20, 10, {0.45f,0.42f,0.50f}},
        {-75, -30, 10, 28, 10, {0.40f,0.48f,0.55f}},
        { 70,  35, 14, 18, 12, {0.50f,0.40f,0.42f}},
        { 72, -40, 11, 24,  9, {0.38f,0.42f,0.50f}},
        {-60,  80,  8, 15,  8, {0.48f,0.45f,0.40f}},
        { 60,  85,  9, 22,  9, {0.42f,0.40f,0.48f}},
        {-65, -80, 10, 16, 10, {0.45f,0.50f,0.48f}},
        { 68, -75, 12, 19,  8, {0.40f,0.38f,0.45f}},
        {-85,   0,  8, 30,  8, {0.55f,0.35f,0.40f}},
        { 85,   5,  7, 26,  7, {0.35f,0.50f,0.55f}},
    };
    for (const auto& b : blds) {
        addBuilding(scene, {b.x, 0, b.z}, {b.w, b.h, b.d}, b.c);
    }

    // Trees
    float treePos[][2] = {
        {-55,20},{-58,-15},{-50,50},{-52,-55},
        {55,25},{58,-20},{50,55},{53,-60},
        {-40,90},{40,92},{-42,-90},{42,-88},
        {-30,10},{30,-12},{-25,-40},{28,45}
    };
    for (auto& tp : treePos) {
        addTree(scene, {tp[0], 0, tp[1]});
    }

    // Start/finish markers
    {
        auto& sf = scene.createObject("StartFinish");
        sf.setMesh(Mesh::createBox(12.0f, 0.15f, 1.5f));
        sf.setColor({0.95f, 0.95f, 0.95f});
        sf.transform().setPosition(m_waypoints[0].position + math::Vec3{0, 0.08f, 0});
        sf.transform().setRotationEuler(0, m_waypoints[0].yaw, 0);
    }
    // Gate pillars
    {
        math::Vec3 right{std::cos(m_startYaw), 0, -std::sin(m_startYaw)};
        auto& p1 = scene.createObject("GateL");
        p1.setMesh(Mesh::createBox(0.6f, 5.0f, 0.6f));
        p1.setColor({0.2f, 0.8f, 1.0f});
        p1.transform().setPosition(m_startPos + right * -6.0f + math::Vec3{0, 2.5f, 0});
        auto& p2 = scene.createObject("GateR");
        p2.setMesh(Mesh::createBox(0.6f, 5.0f, 0.6f));
        p2.setColor({0.2f, 0.8f, 1.0f});
        p2.transform().setPosition(m_startPos + right * 6.0f + math::Vec3{0, 2.5f, 0});
        auto& beam = scene.createObject("GateBeam");
        beam.setMesh(Mesh::createBox(12.5f, 0.4f, 0.4f));
        beam.setColor({0.3f, 0.9f, 1.0f});
        beam.transform().setPosition(m_startPos + math::Vec3{0, 5.0f, 0});
        beam.transform().setRotationEuler(0, m_startYaw, 0);
    }

    NV_LOGI("Track built: %d waypoints, %d checkpoints, %d barriers",
            waypointCount(), checkpointCount(), static_cast<int>(m_barriers.size()));
}

int Track::nearestWaypoint(const math::Vec3& pos) const {
    int best = 0;
    float bestD = 1e30f;
    for (size_t i = 0; i < m_waypoints.size(); ++i) {
        float d = math::Vec3::distance(pos, m_waypoints[i].position);
        if (d < bestD) { bestD = d; best = static_cast<int>(i); }
    }
    return best;
}

float Track::progressAlongTrack(const math::Vec3& pos, int hintWp) const {
    if (m_waypoints.empty()) return 0.0f;
    int n = static_cast<int>(m_waypoints.size());
    int idx = hintWp;
    if (idx < 0 || idx >= n) idx = nearestWaypoint(pos);
    // Refine: check neighbors
    float bestD = math::Vec3::distance(pos, m_waypoints[idx].position);
    for (int o = -2; o <= 2; ++o) {
        int i = (idx + o + n) % n;
        float d = math::Vec3::distance(pos, m_waypoints[i].position);
        if (d < bestD) { bestD = d; idx = i; }
    }
    return static_cast<float>(idx) / static_cast<float>(n);
}

math::Vec3 Track::waypointDirection(int index) const {
    if (m_waypoints.empty()) return {0,0,1};
    int n = static_cast<int>(m_waypoints.size());
    int i0 = ((index % n) + n) % n;
    int i1 = (i0 + 1) % n;
    return (m_waypoints[i1].position - m_waypoints[i0].position).normalized();
}

bool Track::collideBarriers(const math::Vec3& pos, float radius, math::Vec3& outPush) const {
    bool hit = false;
    outPush = {0,0,0};
    math::Vec3 center = pos;
    center.y += 0.5f;
    for (const auto& b : m_barriers) {
        math::Vec3 closest{
            math::clamp(center.x, b.min.x, b.max.x),
            math::clamp(center.y, b.min.y, b.max.y),
            math::clamp(center.z, b.min.z, b.max.z)
        };
        math::Vec3 delta = center - closest;
        float dist = delta.length();
        if (dist < radius && dist > 1e-5f) {
            math::Vec3 n = delta / dist;
            float penetration = radius - dist;
            outPush += n * penetration;
            hit = true;
        } else if (dist <= 1e-5f) {
            // Inside
            outPush += math::Vec3{0, 0, 1} * radius;
            hit = true;
        }
    }
    return hit;
}

bool Track::isOffTrack(const math::Vec3& pos, float maxDist) const {
    if (m_waypoints.empty()) return false;
    int idx = nearestWaypoint(pos);
    float d = math::Vec3::distance(pos, m_waypoints[idx].position);
    return d > maxDist;
}

RespawnPoint Track::getRespawnForCheckpoint(int cpIndex) const {
    if (m_respawnPoints.empty()) {
        RespawnPoint rp;
        rp.position = m_startPos;
        rp.yaw = m_startYaw;
        return rp;
    }
    cpIndex = math::clamp(cpIndex, 0, static_cast<int>(m_respawnPoints.size()) - 1);
    // Find matching
    for (const auto& rp : m_respawnPoints) {
        if (rp.checkpointIndex == cpIndex) return rp;
    }
    return m_respawnPoints[static_cast<size_t>(cpIndex) % m_respawnPoints.size()];
}

} // namespace nexvora
