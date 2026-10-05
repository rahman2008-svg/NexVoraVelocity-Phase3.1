#include "LocalSave.h"
#include "Math.h"
#include "Logger.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>

namespace nexvora {

LocalSave& LocalSave::instance() {
    static LocalSave inst;
    return inst;
}

void LocalSave::setSavePath(const std::string& path) {
    m_path = path;
}

void LocalSave::applyToSettings(RaceSettings& settings) const {
    settings.selectedCarId = m_data.selectedCarId;
    settings.difficulty = static_cast<Difficulty>(m_data.difficulty);
    settings.graphicsQuality = m_data.graphicsQuality;
    settings.soundVolume = m_data.soundVolume;
    settings.musicVolume = m_data.musicVolume;
    settings.vibration = m_data.vibration;
    settings.controlSensitivity = m_data.controlSensitivity;
}

void LocalSave::captureFromSettings(const RaceSettings& settings) {
    m_data.selectedCarId = settings.selectedCarId;
    m_data.difficulty = static_cast<int>(settings.difficulty);
    m_data.graphicsQuality = settings.graphicsQuality;
    m_data.soundVolume = settings.soundVolume;
    m_data.musicVolume = settings.musicVolume;
    m_data.vibration = settings.vibration;
    m_data.controlSensitivity = settings.controlSensitivity;
}

bool LocalSave::load() {
    if (m_path.empty()) {
        NV_LOGW("LocalSave: no path set, using defaults");
        m_loaded = true;
        return false;
    }
    std::ifstream in(m_path);
    if (!in) {
        NV_LOGI("LocalSave: no save file, defaults");
        m_loaded = true;
        return false;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        try {
            if (key == "selectedCarId") m_data.selectedCarId = val;
            else if (key == "difficulty") m_data.difficulty = std::stoi(val);
            else if (key == "graphicsQuality") m_data.graphicsQuality = std::stoi(val);
            else if (key == "soundVolume") m_data.soundVolume = std::stof(val);
            else if (key == "musicVolume") m_data.musicVolume = std::stof(val);
            else if (key == "vibration") m_data.vibration = (val == "1" || val == "true");
            else if (key == "controlSensitivity") m_data.controlSensitivity = std::stof(val);
            else if (key == "bestLapTime") m_data.bestLapTime = std::stof(val);
            else if (key == "bestRaceTime") m_data.bestRaceTime = std::stof(val);
            else if (key == "unlockedCars") m_data.unlockedCars = std::stoi(val);
        } catch (...) {
            NV_LOGW("LocalSave: bad value for %s", key.c_str());
        }
    }
    // Sanity
    m_data.difficulty = math::clamp(m_data.difficulty, 0, 2);
    m_data.graphicsQuality = math::clamp(m_data.graphicsQuality, 0, 2);
    m_data.soundVolume = math::clamp(m_data.soundVolume, 0.0f, 1.0f);
    m_data.musicVolume = math::clamp(m_data.musicVolume, 0.0f, 1.0f);
    m_data.controlSensitivity = math::clamp(m_data.controlSensitivity, 0.3f, 2.0f);
    if (m_data.selectedCarId.empty()) m_data.selectedCarId = "velocity_x";
    m_loaded = true;
    NV_LOGI("LocalSave loaded from %s", m_path.c_str());
    return true;
}

bool LocalSave::save() {
    if (m_path.empty()) return false;
    std::ofstream out(m_path);
    if (!out) {
        NV_LOGE("LocalSave: failed to write %s", m_path.c_str());
        return false;
    }
    out << "# NexVora Velocity Save\n";
    out << "selectedCarId=" << m_data.selectedCarId << "\n";
    out << "difficulty=" << m_data.difficulty << "\n";
    out << "graphicsQuality=" << m_data.graphicsQuality << "\n";
    out << "soundVolume=" << m_data.soundVolume << "\n";
    out << "musicVolume=" << m_data.musicVolume << "\n";
    out << "vibration=" << (m_data.vibration ? "1" : "0") << "\n";
    out << "controlSensitivity=" << m_data.controlSensitivity << "\n";
    out << "bestLapTime=" << m_data.bestLapTime << "\n";
    out << "bestRaceTime=" << m_data.bestRaceTime << "\n";
    out << "unlockedCars=" << m_data.unlockedCars << "\n";
    NV_LOGI("LocalSave written");
    return true;
}

} // namespace nexvora
