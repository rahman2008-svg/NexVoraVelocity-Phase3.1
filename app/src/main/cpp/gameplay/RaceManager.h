#pragma once
#include <memory>

#include "RaceState.h"
#include "VehicleController.h"
#include "AIController.h"
#include "Track.h"
#include "ParticleSystem.h"
#include "Scene.h"
#include "Camera.h"
#include <vector>
#include <string>
#include <array>
#include <functional>
#include <unordered_map>

namespace nexvora {

// HUD snapshot for Kotlin UI
// Array layout for uiGetHud (12 floats):
// 0 speed, 1 lap, 2 totalLaps, 3 position, 4 raceTime, 5 nitro,
// 6 countdown, 7 state, 8 bestLap, 9 finalTime, 10 finalPosition, 11 difficulty
struct HudSnapshot {
    int state = 0;
    float speedKmh = 0.0f;
    int currentLap = 0;
    int totalLaps = 3;
    int position = 4;
    float raceTime = 0.0f;
    float nitro = 0.0f;
    int countdown = -1; // 3,2,1,0=GO,-1=none
    bool paused = false;
    int playerFinished = 0;
    float bestLap = 0.0f;
    float finalTime = 0.0f;
    int finalPosition = 4;
    int difficulty = 1;
    char carName[32] = {};
    char message[64] = {};
};

enum class TouchControl : int {
    None = 0,
    Steer = 1,
    Accel = 2,
    Brake = 3,
    Nitro = 4
};

class RaceManager {
public:
    static RaceManager& instance();

    void initialize(Scene& scene);
    void shutdown();

    void update(float dt);
    void onTouch(int action, int pointerId, float x, float y, int screenW, int screenH);

    void setState(RaceState state);
    RaceState state() const { return m_state; }

    void startRace();
    void pauseRace();
    void resumeRace();
    void restartRace();
    void returnToMenu();
    void selectCar(const std::string& carId);
    void setDifficulty(Difficulty d);
    void applySettings(const RaceSettings& s);

    RaceSettings& settings() { return m_settings; }
    const RaceSettings& settings() const { return m_settings; }
    const RaceResults& results() const { return m_results; }
    const HudSnapshot& hud() const { return m_hud; }

    VehicleController* player() { return m_player.get(); }
    Track& track() { return m_track; }
    ParticleSystem& particles() { return m_particles; }

    void updateCamera(Camera& camera, float dt);

    void clearTouchState();

private:
    RaceManager() = default;

    void setupRaceGrid();
    void updatePlayerInput(float dt);
    void updateCheckpoints(VehicleController& v, bool isPlayer);
    void updatePositions();
    void updateCollisions(float dt);
    void checkRespawns(float dt);
    void syncVehicleVisuals(Scene& scene);
    void updateHud();
    void finishRace();
    void recomputeInputsFromPointers();
    TouchControl zoneForNormalized(float nx, float ny) const;

    RaceState m_state = RaceState::MainMenu;
    RaceSettings m_settings;
    RaceResults m_results;
    HudSnapshot m_hud;

    Scene* m_scene = nullptr;
    Track m_track;
    ParticleSystem m_particles;

    std::unique_ptr<VehicleController> m_player;
    std::array<std::unique_ptr<VehicleController>, 3> m_aiVehicles;
    std::array<AIController, 3> m_aiControllers;

    float m_countdownTimer = 0.0f;
    int m_countdownValue = 3;
    float m_raceTimer = 0.0f;
    float m_cameraShake = 0.0f;

    // Independent off-track timers: [0]=player, [1..3]=AI
    std::array<float, 4> m_offTrackTimers{{0.0f, 0.0f, 0.0f, 0.0f}};

    // Input accumulation (derived from pointer map)
    float m_steerInput = 0.0f;
    float m_throttleInput = 0.0f;
    float m_brakeInput = 0.0f;
    bool m_nitroPressed = false;

    // Pointer ID -> control assignment
    std::unordered_map<int, TouchControl> m_pointerControls;
    // For steer, store normalized x of that pointer
    std::unordered_map<int, float> m_pointerNormX;

    bool m_initialized = false;
    std::vector<std::string> m_playerPartNames;
};

} // namespace nexvora
