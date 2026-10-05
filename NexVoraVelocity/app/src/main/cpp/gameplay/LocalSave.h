#pragma once

#include "RaceState.h"
#include <string>

namespace nexvora {

struct SaveData {
    std::string selectedCarId = "velocity_x";
    int difficulty = 1;
    int graphicsQuality = 1;
    float soundVolume = 1.0f;
    float musicVolume = 0.7f;
    bool vibration = true;
    float controlSensitivity = 1.0f;
    float bestLapTime = 0.0f;
    float bestRaceTime = 0.0f;
    int unlockedCars = 7; // bit flags: 1=velocity_x, 2=nova_gt, 4=cyber_r
};

class LocalSave {
public:
    static LocalSave& instance();

    void setSavePath(const std::string& path);
    bool load();
    bool save();

    SaveData& data() { return m_data; }
    const SaveData& data() const { return m_data; }

    void applyToSettings(RaceSettings& settings) const;
    void captureFromSettings(const RaceSettings& settings);

private:
    LocalSave() = default;
    std::string m_path;
    SaveData m_data;
    bool m_loaded = false;
};

} // namespace nexvora
