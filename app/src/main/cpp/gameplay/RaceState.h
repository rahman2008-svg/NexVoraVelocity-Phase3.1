#pragma once

#include <cstdint>
#include <string>

namespace nexvora {

enum class RaceState : int32_t {
    MainMenu = 0,
    CarSelection = 1,
    Settings = 2,
    Countdown = 3,
    Racing = 4,
    Paused = 5,
    Finished = 6,
    Results = 7
};

inline const char* raceStateName(RaceState s) {
    switch (s) {
        case RaceState::MainMenu: return "MainMenu";
        case RaceState::CarSelection: return "CarSelection";
        case RaceState::Settings: return "Settings";
        case RaceState::Countdown: return "Countdown";
        case RaceState::Racing: return "Racing";
        case RaceState::Paused: return "Paused";
        case RaceState::Finished: return "Finished";
        case RaceState::Results: return "Results";
        default: return "Unknown";
    }
}

enum class Difficulty : int32_t {
    Easy = 0,
    Normal = 1,
    Hard = 2
};

struct RaceSettings {
    int totalLaps = 3;
    Difficulty difficulty = Difficulty::Normal;
    std::string selectedCarId = "velocity_x";
    int graphicsQuality = 1; // 0 low, 1 med, 2 high
    float soundVolume = 1.0f;
    float musicVolume = 0.7f;
    bool vibration = true;
    float controlSensitivity = 1.0f;
};

struct RaceResults {
    int playerPosition = 4;
    float raceTime = 0.0f;
    float bestLapTime = 0.0f;
    int totalLaps = 3;
    std::string carName;
    Difficulty difficulty = Difficulty::Normal;
};

} // namespace nexvora
