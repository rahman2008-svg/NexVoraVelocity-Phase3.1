#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace nexvora {

/**
 * Lightweight audio abstraction.
 * Phase 2 provides the interface and graceful no-op fallback.
 * Actual playback is bridged via JNI when assets are present.
 */
class AudioManager {
public:
    static AudioManager& instance();

    void initialize();
    void shutdown();

    // Returns a handle (>=0) or -1 on failure
    int loadSound(const std::string& path);
    int loadMusic(const std::string& path);

    void playSound(int handle, float volume = 1.0f);
    void playMusic(int handle, bool loop = true, float volume = 1.0f);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    void setMasterVolume(float v);
    float masterVolume() const { return m_masterVolume; }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

private:
    AudioManager() = default;
    bool m_initialized = false;
    bool m_enabled = true;
    float m_masterVolume = 1.0f;
    int m_nextHandle = 1;
    std::unordered_map<std::string, int> m_soundCache;
    std::unordered_map<std::string, int> m_musicCache;
};

} // namespace nexvora
