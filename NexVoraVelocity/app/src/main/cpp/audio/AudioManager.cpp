#include "AudioManager.h"
#include "Math.h"
#include "Logger.h"

namespace nexvora {

AudioManager& AudioManager::instance() {
    static AudioManager inst;
    return inst;
}

void AudioManager::initialize() {
    if (m_initialized) return;
    m_initialized = true;
    NV_LOGI("AudioManager initialized (stub backend - graceful no-op)");
}

void AudioManager::shutdown() {
    m_soundCache.clear();
    m_musicCache.clear();
    m_initialized = false;
    NV_LOGI("AudioManager shut down");
}

int AudioManager::loadSound(const std::string& path) {
    if (!m_enabled) return -1;
    auto it = m_soundCache.find(path);
    if (it != m_soundCache.end()) return it->second;
    int h = m_nextHandle++;
    m_soundCache[path] = h;
    NV_LOGD("AudioManager: registered sound '%s' handle=%d (no asset data yet)", path.c_str(), h);
    return h;
}

int AudioManager::loadMusic(const std::string& path) {
    if (!m_enabled) return -1;
    auto it = m_musicCache.find(path);
    if (it != m_musicCache.end()) return it->second;
    int h = m_nextHandle++;
    m_musicCache[path] = h;
    NV_LOGD("AudioManager: registered music '%s' handle=%d (no asset data yet)", path.c_str(), h);
    return h;
}

void AudioManager::playSound(int handle, float volume) {
    if (!m_enabled || handle < 0) return;
    // Graceful no-op until assets + JNI player are added
    (void)volume;
}

void AudioManager::playMusic(int handle, bool loop, float volume) {
    if (!m_enabled || handle < 0) return;
    (void)loop; (void)volume;
}

void AudioManager::stopMusic() {}
void AudioManager::pauseMusic() {}
void AudioManager::resumeMusic() {}

void AudioManager::setMasterVolume(float v) {
    m_masterVolume = math::clamp(v, 0.0f, 1.0f);
}

} // namespace nexvora
