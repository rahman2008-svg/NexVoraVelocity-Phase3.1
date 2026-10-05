#include "RaceManager.h"
#include <cstdio>
#include "Logger.h"
#include "LocalSave.h"
#include "Light.h"
#include "Mesh.h"
#include "InputManager.h"
#include <cmath>
#include <algorithm>
#include <cstring>

namespace nexvora {

RaceManager& RaceManager::instance() {
    static RaceManager inst;
    return inst;
}

void RaceManager::initialize(Scene& scene) {
    m_scene = &scene;
    m_particles.initialize();
    LocalSave::instance().load();
    LocalSave::instance().applyToSettings(m_settings);
    m_state = RaceState::MainMenu;
    m_initialized = true;
    clearTouchState();
    updateHud();
    NV_LOGI("RaceManager initialized");
}

void RaceManager::shutdown() {
    m_player.reset();
    for (auto& a : m_aiVehicles) a.reset();
    m_particles.clear();
    clearTouchState();
    m_initialized = false;
}

void RaceManager::clearTouchState() {
    m_pointerControls.clear();
    m_pointerNormX.clear();
    m_steerInput = 0.0f;
    m_throttleInput = 0.0f;
    m_brakeInput = 0.0f;
    m_nitroPressed = false;
}

void RaceManager::setState(RaceState state) {
    NV_LOGI("RaceState: %s -> %s", raceStateName(m_state), raceStateName(state));
    m_state = state;
    updateHud();
}

void RaceManager::applySettings(const RaceSettings& s) {
    m_settings = s;
    LocalSave::instance().captureFromSettings(s);
    LocalSave::instance().save();
}

void RaceManager::selectCar(const std::string& carId) {
    m_settings.selectedCarId = carId;
    LocalSave::instance().data().selectedCarId = carId;
    LocalSave::instance().save();
}

void RaceManager::setDifficulty(Difficulty d) {
    m_settings.difficulty = d;
    LocalSave::instance().data().difficulty = static_cast<int>(d);
    LocalSave::instance().save();
}

void RaceManager::returnToMenu() {
    if (m_scene) m_scene->clear();
    m_player.reset();
    for (auto& a : m_aiVehicles) a.reset();
    m_particles.clear();
    clearTouchState();
    m_results = RaceResults{};
    setState(RaceState::MainMenu);
}

void RaceManager::startRace() {
    if (!m_scene) return;
    NV_LOGI("Starting race car=%s diff=%d laps=%d",
            m_settings.selectedCarId.c_str(),
            static_cast<int>(m_settings.difficulty),
            m_settings.totalLaps);

    m_scene->clear();
    m_particles.clear();
    clearTouchState();
    m_results = RaceResults{};

    m_scene->ambient() = {{0.25f, 0.27f, 0.32f}, 1.0f};
    m_scene->addLight(Light::directional({-0.35f, -1.0f, -0.25f}, {1.0f, 0.97f, 0.92f}, 1.15f));
    m_scene->addLight(Light::point({0.0f, 8.0f, 0.0f}, {0.4f, 0.7f, 1.0f}, 1.5f, 40.0f));

    m_track.buildFuturisticCircuit(*m_scene);
    setupRaceGrid();

    m_raceTimer = 0.0f;
    m_countdownTimer = 1.0f;
    m_countdownValue = 3;
    m_cameraShake = 0.0f;
    m_offTrackTimers = {{0.0f, 0.0f, 0.0f, 0.0f}};

    setState(RaceState::Countdown);
}

void RaceManager::setupRaceGrid() {
    VehicleConfig pcfg = VehicleConfig::byId(m_settings.selectedCarId);
    m_player = std::make_unique<VehicleController>();
    math::Vec3 start = m_track.startPosition();
    float yaw = m_track.startYaw();
    math::Vec3 right{std::cos(yaw), 0, -std::sin(yaw)};
    math::Vec3 forward{std::sin(yaw), 0, std::cos(yaw)};

    math::Vec3 ppos = start - forward * 4.0f + right * (-2.0f);
    m_player->initialize(pcfg, ppos, yaw);
    m_player->setIsPlayer(true);
    m_player->buildVisuals(*m_scene, "Player");
    m_playerPartNames = {
        "Player_body","Player_cabin","Player_ws","Player_spoiler",
        "Player_hl","Player_rl","Player_ex",
        "Player_wheel0","Player_wheel1","Player_wheel2","Player_wheel3"
    };

    const char* aiIds[] = {"nova_gt", "cyber_r", "velocity_x"};
    math::Vec3 aiColors[] = {{0.2f,0.4f,0.9f},{0.15f,0.85f,0.4f},{0.9f,0.5f,0.1f}};
    for (int i = 0; i < 3; ++i) {
        VehicleConfig cfg = VehicleConfig::byId(aiIds[i]);
        cfg.bodyColor = aiColors[i];
        m_aiVehicles[i] = std::make_unique<VehicleController>();
        float lane = (i - 1) * 2.2f;
        math::Vec3 apos = start - forward * (8.0f + i * 3.5f) + right * lane;
        m_aiVehicles[i]->initialize(cfg, apos, yaw);
        m_aiVehicles[i]->setIsPlayer(false);
        m_aiVehicles[i]->buildVisuals(*m_scene, "AI" + std::to_string(i));
        m_aiControllers[i].initialize(m_aiVehicles[i].get(), &m_track, m_settings.difficulty, i);
    }
}

void RaceManager::pauseRace() {
    if (m_state == RaceState::Racing || m_state == RaceState::Countdown) {
        clearTouchState();
        setState(RaceState::Paused);
    }
}

void RaceManager::resumeRace() {
    if (m_state == RaceState::Paused) {
        clearTouchState();
        setState(RaceState::Racing);
    }
}

void RaceManager::restartRace() {
    startRace();
}

TouchControl RaceManager::zoneForNormalized(float nx, float ny) const {
    if (nx < 0.40f) return TouchControl::Steer;
    if (nx > 0.65f) {
        if (ny > 0.72f) return TouchControl::Brake;
        if (ny > 0.38f) return TouchControl::Accel;
        return TouchControl::Nitro;
    }
    return TouchControl::None;
}

void RaceManager::recomputeInputsFromPointers() {
    m_steerInput = 0.0f;
    m_throttleInput = 0.0f;
    m_brakeInput = 0.0f;
    m_nitroPressed = false;

    for (const auto& pair : m_pointerControls) {
        int pid = pair.first;
        TouchControl ctrl = pair.second;
        switch (ctrl) {
            case TouchControl::Steer: {
                auto it = m_pointerNormX.find(pid);
                float nx = (it != m_pointerNormX.end()) ? it->second : 0.20f;
                m_steerInput = math::clamp((nx - 0.20f) / 0.20f, -1.0f, 1.0f);
                break;
            }
            case TouchControl::Accel:
                m_throttleInput = 1.0f;
                break;
            case TouchControl::Brake:
                m_brakeInput = 1.0f;
                break;
            case TouchControl::Nitro:
                m_nitroPressed = true;
                break;
            default:
                break;
        }
    }
}

void RaceManager::onTouch(int action, int pointerId, float x, float y, int screenW, int screenH) {
    float nx = x / static_cast<float>(std::max(screenW, 1));
    float ny = y / static_cast<float>(std::max(screenH, 1));

    // action: 0=DOWN, 1=MOVE, 2=UP, 3=POINTER_DOWN, 4=POINTER_UP, -1=other
    // Treat CANCEL (sometimes sent as UP) by clearing all if needed - handled via UP

    if (m_state != RaceState::Racing && m_state != RaceState::Countdown) {
        InputManager::instance().onTouch(action, pointerId, x, y);
        return;
    }

    if (action == 0 || action == 3) {
        // DOWN / POINTER_DOWN — assign control by zone
        TouchControl zone = zoneForNormalized(nx, ny);
        if (zone != TouchControl::None) {
            m_pointerControls[pointerId] = zone;
            if (zone == TouchControl::Steer) {
                m_pointerNormX[pointerId] = nx;
            }
        }
        recomputeInputsFromPointers();
    } else if (action == 1) {
        // MOVE — update steer x if this pointer owns steer
        auto it = m_pointerControls.find(pointerId);
        if (it != m_pointerControls.end()) {
            if (it->second == TouchControl::Steer) {
                m_pointerNormX[pointerId] = nx;
                recomputeInputsFromPointers();
            }
            // Optionally re-zone if finger slides far — keep original assignment for stability
        }
    } else if (action == 2 || action == 4) {
        // UP / POINTER_UP — release only this pointer's control
        m_pointerControls.erase(pointerId);
        m_pointerNormX.erase(pointerId);
        recomputeInputsFromPointers();
    }

    InputManager::instance().onTouch(action, pointerId, x, y);
}

void RaceManager::updatePlayerInput(float dt) {
    if (!m_player) return;
    float sens = m_settings.controlSensitivity;
    float steer = m_steerInput * sens;
    float throttle = m_throttleInput;
    float brake = m_brakeInput;
    bool nitro = m_nitroPressed;

    auto* joy = InputManager::instance().getJoystick("Steer");
    if (joy && joy->active) {
        steer = joy->axisX * sens;
    }
    auto* accelBtn = InputManager::instance().getButton("Accelerate");
    if (accelBtn && accelBtn->pressed) throttle = 1.0f;
    auto* brakeBtn = InputManager::instance().getButton("Brake");
    if (brakeBtn && brakeBtn->pressed) brake = 1.0f;

    m_player->update(dt, steer, throttle, brake, nitro);

    if (m_player->speed() > 2.0f || m_player->isNitroActive()) {
        math::Vec3 back{-std::sin(m_player->yaw()), 0.2f, -std::cos(m_player->yaw())};
        math::Vec3 epos = m_player->position() + back * 1.8f;
        epos.y += 0.3f;
        m_particles.emitExhaust(epos, back, m_player->isNitroActive());
    }
    if (std::abs(m_steerInput) > 0.7f && m_player->speed() > 20.0f) {
        m_particles.emitSmoke(m_player->position(), 2);
    }
}

void RaceManager::updateCheckpoints(VehicleController& v, bool isPlayer) {
    if (v.finished()) return;
    const auto& cps = m_track.checkpoints();
    if (cps.empty()) return;

    int expected = v.checkpointIndex();
    if (expected < 0) expected = 0;
    if (expected >= static_cast<int>(cps.size())) expected = 0;

    math::Vec3 pos = v.position();
    const Checkpoint& cp = cps[static_cast<size_t>(expected)];
    float dist = math::Vec3::distance(pos, cp.position);

    if (dist >= cp.radius) {
        // Still update track progress for position
        int wp = m_track.nearestWaypoint(pos);
        v.setWaypointIndex(wp);
        float prog = static_cast<float>(v.lap()) + m_track.progressAlongTrack(pos, wp);
        v.setTrackProgress(prog);
        return;
    }

    // Hit expected checkpoint
    if (cp.isStartFinish) {
        if (v.lap() == 0) {
            // First crossing starts Lap 1
            v.setLap(1);
            v.setCurrentLapTime(0.0f);
            if (isPlayer) NV_LOGI("Race started — Lap 1 begins");
        } else {
            // Completed a lap
            float lapTime = v.currentLapTime();
            if (lapTime > 0.5f) { // ignore tiny accidental retriggers
                if (v.bestLapTime() <= 0.0f || lapTime < v.bestLapTime()) {
                    v.setBestLapTime(lapTime);
                }
                if (isPlayer) {
                    NV_LOGI("Lap %d complete in %.2fs (best %.2f)", v.lap(), lapTime, v.bestLapTime());
                }
                v.setCurrentLapTime(0.0f);
                v.setLap(v.lap() + 1);
                if (v.lap() > m_settings.totalLaps) {
                    // Finished final lap
                    v.setLap(m_settings.totalLaps);
                    v.setFinished(true);
                    v.setRaceTime(m_raceTimer);
                    if (isPlayer) {
                        // Progress/position updated just before finishRace in update()
                        finishRace();
                    }
                    return;
                }
            }
        }
    }

    int next = expected + 1;
    if (next >= static_cast<int>(cps.size())) {
        next = 0;
    }
    v.setCheckpointIndex(next);

    int wp = m_track.nearestWaypoint(pos);
    v.setWaypointIndex(wp);
    float prog = static_cast<float>(v.lap()) + m_track.progressAlongTrack(pos, wp);
    v.setTrackProgress(prog);
}

void RaceManager::updatePositions() {
    struct Entry { float progress; bool player; };
    std::vector<Entry> entries;
    if (m_player) entries.push_back({m_player->trackProgress(), true});
    for (int i = 0; i < 3; ++i) {
        if (m_aiVehicles[i]) entries.push_back({m_aiVehicles[i]->trackProgress(), false});
    }
    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b) {
        return a.progress > b.progress;
    });
    for (size_t i = 0; i < entries.size(); ++i) {
        if (entries[i].player) {
            m_hud.position = static_cast<int>(i) + 1;
            break;
        }
    }
}

void RaceManager::updateCollisions(float dt) {
    (void)dt;
    auto resolveVehicleBarrier = [&](VehicleController& v) {
        math::Vec3 push;
        if (m_track.collideBarriers(v.position(), v.collisionRadius(), push)) {
            auto pos = v.position();
            pos += push;
            v.setPosition(pos);
            math::Vec3 n = push.normalized();
            math::Vec3 vel = v.velocity();
            float vn = math::Vec3::dot(vel, n);
            if (vn < 0.0f) {
                vel = vel - n * (vn * 1.5f);
                v.setVelocity(vel);
            }
            if (v.isPlayer()) {
                m_cameraShake = 0.25f;
                m_particles.emitSparks(v.position() + math::Vec3{0, 0.5f, 0}, 6);
            }
        }
    };

    if (m_player) resolveVehicleBarrier(*m_player);
    for (auto& ai : m_aiVehicles) {
        if (ai) resolveVehicleBarrier(*ai);
    }

    auto collidePair = [&](VehicleController& a, VehicleController& b) {
        math::Vec3 ca = a.collisionCenter();
        math::Vec3 cb = b.collisionCenter();
        math::Vec3 d = ca - cb;
        float dist = d.length();
        float minD = a.collisionRadius() + b.collisionRadius();
        if (dist < minD && dist > 1e-4f) {
            math::Vec3 n = d / dist;
            float pen = minD - dist;
            a.setPosition(a.position() + n * (pen * 0.5f));
            b.setPosition(b.position() - n * (pen * 0.5f));
            math::Vec3 va = a.velocity();
            math::Vec3 vb = b.velocity();
            float imp = 2.0f;
            a.setVelocity(va + n * imp);
            b.setVelocity(vb - n * imp);
            if (a.isPlayer() || b.isPlayer()) {
                m_cameraShake = 0.2f;
                m_particles.emitSparks((ca + cb) * 0.5f, 5);
            }
        }
    };

    if (m_player) {
        for (auto& ai : m_aiVehicles) {
            if (ai) collidePair(*m_player, *ai);
        }
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            if (m_aiVehicles[i] && m_aiVehicles[j])
                collidePair(*m_aiVehicles[i], *m_aiVehicles[j]);
        }
    }
}

void RaceManager::checkRespawns(float dt) {
    auto tryRespawn = [&](VehicleController& v, int timerIndex) {
        if (v.finished()) return;
        bool off = m_track.isOffTrack(v.position(), 22.0f);
        if (off) {
            m_offTrackTimers[static_cast<size_t>(timerIndex)] += dt;
        } else {
            m_offTrackTimers[static_cast<size_t>(timerIndex)] = 0.0f;
        }
        if (off && m_offTrackTimers[static_cast<size_t>(timerIndex)] > 1.5f) {
            int cp = std::max(0, v.checkpointIndex() - 1);
            auto rp = m_track.getRespawnForCheckpoint(cp);
            v.reset(rp.position, rp.yaw);
            v.setCheckpointIndex(rp.checkpointIndex);
            m_offTrackTimers[static_cast<size_t>(timerIndex)] = 0.0f;
            NV_LOGI("Vehicle respawned at checkpoint %d", rp.checkpointIndex);
        }
    };
    if (m_player) tryRespawn(*m_player, 0);
    for (int i = 0; i < 3; ++i) {
        if (m_aiVehicles[i]) tryRespawn(*m_aiVehicles[i], i + 1);
    }
}

void RaceManager::syncVehicleVisuals(Scene& scene) {
    auto syncOne = [&](VehicleController& v, const std::string& prefix) {
        math::Vec3 pos = v.position();
        float yaw = v.yaw();
        struct Part { const char* suffix; math::Vec3 local; math::Vec3 euler; };
        Part parts[] = {
            {"_body", {0, 0.45f, 0}, {0,0,0}},
            {"_cabin", {0, 0.85f, -0.2f}, {0,0,0}},
            {"_ws", {0, 0.95f, 0.35f}, {0,0,0}},
            {"_spoiler", {0, 0.95f, -1.7f}, {0,0,0}},
            {"_hl", {0, 0.4f, 1.75f}, {0,0,0}},
            {"_rl", {0, 0.4f, -1.75f}, {0,0,0}},
            {"_ex", {0.5f, 0.2f, -1.9f}, {0,0,0}},
            {"_wheel0", {-0.85f, 0.32f, 1.15f}, {0,0, math::HALF_PI}},
            {"_wheel1", {0.85f, 0.32f, 1.15f}, {0,0, math::HALF_PI}},
            {"_wheel2", {-0.85f, 0.32f, -1.15f}, {0,0, math::HALF_PI}},
            {"_wheel3", {0.85f, 0.32f, -1.15f}, {0,0, math::HALF_PI}},
        };
        math::Quat q = math::Quat::fromEuler(0, yaw, 0);
        for (const auto& p : parts) {
            auto obj = scene.findObject(prefix + p.suffix);
            if (!obj) continue;
            math::Vec3 worldLocal = q.rotate(p.local);
            obj->transform().setPosition(pos + worldLocal);
            obj->transform().setRotationEuler(p.euler.x, yaw + p.euler.y, p.euler.z);
        }
    };

    if (m_player) syncOne(*m_player, "Player");
    for (int i = 0; i < 3; ++i) {
        if (m_aiVehicles[i]) syncOne(*m_aiVehicles[i], "AI" + std::to_string(i));
    }
}

void RaceManager::updateCamera(Camera& camera, float dt) {
    if (!m_player) return;
    math::Vec3 ppos = m_player->position();
    float yaw = m_player->yaw();
    math::Vec3 back{-std::sin(yaw), 0, -std::cos(yaw)};
    float speedFactor = math::clamp(m_player->speed() / 50.0f, 0.0f, 1.0f);
    float dist = 10.0f + speedFactor * 3.0f;
    float height = 4.5f + speedFactor * 1.0f;

    math::Vec3 desired = ppos + back * dist + math::Vec3{0, height, 0};
    if (m_cameraShake > 0.0f) {
        desired.x += std::sin(m_raceTimer * 40.0f) * m_cameraShake * 0.4f;
        desired.y += std::cos(m_raceTimer * 35.0f) * m_cameraShake * 0.25f;
        m_cameraShake = std::max(0.0f, m_cameraShake - dt * 2.0f);
    }

    math::Vec3 cur = camera.transform().position();
    float smooth = 6.0f;
    cur = math::Vec3::lerp(cur, desired, math::clamp(smooth * dt, 0.0f, 1.0f));
    camera.transform().setPosition(cur);
    camera.lookAt(ppos + math::Vec3{0, 1.0f, 0});
}

void RaceManager::finishRace() {
    if (m_state == RaceState::Finished || m_state == RaceState::Results) return;

    // Force final progress + position calculation at finish moment
    if (m_player) {
        int wp = m_track.nearestWaypoint(m_player->position());
        m_player->setWaypointIndex(wp);
        float prog = static_cast<float>(m_player->lap()) +
                     m_track.progressAlongTrack(m_player->position(), wp);
        m_player->setTrackProgress(prog);
    }
    for (auto& ai : m_aiVehicles) {
        if (!ai) continue;
        int wp = m_track.nearestWaypoint(ai->position());
        ai->setWaypointIndex(wp);
        float prog = static_cast<float>(ai->lap()) +
                     m_track.progressAlongTrack(ai->position(), wp);
        ai->setTrackProgress(prog);
    }
    updatePositions();

    m_results.playerPosition = m_hud.position;
    m_results.raceTime = m_raceTimer;
    m_results.bestLapTime = m_player ? m_player->bestLapTime() : 0.0f;
    m_results.totalLaps = m_settings.totalLaps;
    m_results.carName = VehicleConfig::byId(m_settings.selectedCarId).name;
    m_results.difficulty = m_settings.difficulty;

    auto& save = LocalSave::instance().data();
    if (save.bestRaceTime <= 0.0f || m_results.raceTime < save.bestRaceTime) {
        save.bestRaceTime = m_results.raceTime;
    }
    if (m_results.bestLapTime > 0.0f &&
        (save.bestLapTime <= 0.0f || m_results.bestLapTime < save.bestLapTime)) {
        save.bestLapTime = m_results.bestLapTime;
    }
    LocalSave::instance().save();

    // Mirror into HUD for UI
    m_hud.finalTime = m_results.raceTime;
    m_hud.bestLap = m_results.bestLapTime;
    m_hud.finalPosition = m_results.playerPosition;
    m_hud.position = m_results.playerPosition;
    m_hud.raceTime = m_results.raceTime;
    std::strncpy(m_hud.carName, m_results.carName.c_str(), sizeof(m_hud.carName) - 1);

    setState(RaceState::Results);
    NV_LOGI("Race finished! Pos %d Time %.2f BestLap %.2f",
            m_results.playerPosition, m_results.raceTime, m_results.bestLapTime);
}

void RaceManager::updateHud() {
    m_hud.state = static_cast<int>(m_state);
    m_hud.totalLaps = m_settings.totalLaps;
    m_hud.paused = (m_state == RaceState::Paused);
    m_hud.difficulty = static_cast<int>(m_settings.difficulty);

    if (m_player) {
        m_hud.speedKmh = m_player->speedKmh();
        m_hud.currentLap = std::max(0, m_player->lap());
        m_hud.nitro = m_player->nitroNormalized();
        if (m_state != RaceState::Results) {
            m_hud.bestLap = m_player->bestLapTime();
        }
    }

    if (m_state == RaceState::Results) {
        m_hud.raceTime = m_results.raceTime;
        m_hud.finalTime = m_results.raceTime;
        m_hud.bestLap = m_results.bestLapTime;
        m_hud.finalPosition = m_results.playerPosition;
        m_hud.position = m_results.playerPosition;
        std::strncpy(m_hud.carName, m_results.carName.c_str(), sizeof(m_hud.carName) - 1);
    } else {
        m_hud.raceTime = m_raceTimer;
        m_hud.finalTime = 0.0f;
        m_hud.finalPosition = m_hud.position;
    }

    m_hud.countdown = (m_state == RaceState::Countdown) ? m_countdownValue : -1;
    m_hud.message[0] = '\0';
    if (m_state == RaceState::Countdown) {
        if (m_countdownValue > 0)
            std::snprintf(m_hud.message, sizeof(m_hud.message), "%d", m_countdownValue);
        else
            std::snprintf(m_hud.message, sizeof(m_hud.message), "GO!");
    }
}

void RaceManager::update(float dt) {
    if (!m_initialized || !m_scene) return;
    dt = std::min(dt, 0.05f);

    switch (m_state) {
        case RaceState::MainMenu:
        case RaceState::CarSelection:
        case RaceState::Settings:
        case RaceState::Results:
            break;

        case RaceState::Countdown: {
            m_countdownTimer -= dt;
            if (m_countdownTimer <= 0.0f) {
                m_countdownValue--;
                m_countdownTimer = 1.0f;
                if (m_countdownValue < 0) {
                    setState(RaceState::Racing);
                    m_countdownValue = -1;
                }
            }
            if (m_player) m_player->setEnabled(false);
            for (auto& ai : m_aiVehicles) if (ai) ai->setEnabled(false);
            syncVehicleVisuals(*m_scene);
            updateCamera(m_scene->camera(), dt);
            updateHud();
            break;
        }

        case RaceState::Paused:
            // Timer frozen, simulation frozen, inputs cleared on pause entry
            updateHud();
            break;

        case RaceState::Racing: {
            m_raceTimer += dt;
            if (m_player) m_player->setEnabled(true);
            for (auto& ai : m_aiVehicles) if (ai) ai->setEnabled(true);

            updatePlayerInput(dt);

            for (int i = 0; i < 3; ++i) {
                m_aiControllers[i].update(dt);
            }

            // SINGLE authoritative lap timer increment (dt only, once per frame)
            if (m_player && !m_player->finished() && m_player->lap() >= 1) {
                m_player->setCurrentLapTime(m_player->currentLapTime() + dt);
            }
            for (auto& ai : m_aiVehicles) {
                if (ai && !ai->finished() && ai->lap() >= 1) {
                    ai->setCurrentLapTime(ai->currentLapTime() + dt);
                }
            }

            if (m_player) updateCheckpoints(*m_player, true);
            for (auto& ai : m_aiVehicles) {
                if (ai) updateCheckpoints(*ai, false);
            }

            updateCollisions(dt);
            checkRespawns(dt);
            updatePositions();
            m_particles.update(dt);
            syncVehicleVisuals(*m_scene);
            updateCamera(m_scene->camera(), dt);
            updateHud();
            break;
        }

        case RaceState::Finished:
            setState(RaceState::Results);
            break;

        default:
            break;
    }
}

} // namespace nexvora
