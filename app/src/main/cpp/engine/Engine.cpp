#include "Engine.h"
#include "Logger.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include <cstring>

namespace nexvora {

Engine& Engine::instance() {
    static Engine eng;
    return eng;
}

bool Engine::initialize() {
    if (m_initialized) return true;
    NV_LOGI("Engine initializing (Phase 3 Racing)...");

    if (!m_renderer.initialize()) {
        NV_LOGE("Renderer failed");
        return false;
    }

    AudioManager::instance().initialize();
    DebugOverlay::instance().setEnabled(true);

    auto& input = InputManager::instance();
    input.createJoystick("Steer", 0.18f, 0.75f, 0.14f);
    input.createButton("Accelerate", 0.85f, 0.65f, 0.09f, 0.09f);
    input.createButton("Brake", 0.85f, 0.88f, 0.09f, 0.07f);
    input.createButton("Nitro", 0.72f, 0.88f, 0.07f, 0.07f);

    if (!m_filesDir.empty()) {
        LocalSave::instance().setSavePath(m_filesDir + "/nexvora_save.txt");
    }
    LocalSave::instance().load();

    RaceManager::instance().initialize(m_scene);

    m_initialized = true;
    m_paused = false;
    NV_LOGI("Engine initialized (Phase 3)");
    return true;
}

void Engine::shutdown() {
    if (!m_initialized) return;
    RaceManager::instance().shutdown();
    m_scene.clear();
    m_renderer.shutdown();
    TextureManager::instance().clear();
    ShaderManager::instance().clear();
    InputManager::instance().reset();
    AudioManager::instance().shutdown();
    m_initialized = false;
}

void Engine::setFilesDir(const char* path) {
    if (path) m_filesDir = path;
}

void Engine::onSurfaceCreated() {
    NV_LOGI("onSurfaceCreated");
    if (!m_initialized) {
        initialize();
    } else {
        m_renderer.initialize();
        for (auto& obj : m_scene.objects()) {
            if (obj && obj->mesh()) obj->mesh()->release();
        }
    }
}

void Engine::onSurfaceChanged(int width, int height) {
    m_width = width > 0 ? width : 1;
    m_height = height > 0 ? height : 1;
    m_renderer.resize(m_width, m_height);
    InputManager::instance().setScreenSize(m_width, m_height);
    m_scene.camera().setAspect(static_cast<float>(m_width) / static_cast<float>(m_height));
}

void Engine::onDrawFrame(float deltaTime) {
    if (!m_initialized || m_paused) return;

    Time::update(deltaTime);
    InputManager::instance().update(deltaTime);

    RaceManager::instance().update(deltaTime);

    m_renderer.beginFrame();
    m_renderer.renderScene(m_scene);
    m_renderer.endFrame();

    DebugOverlay::instance().endFrame(deltaTime);
    auto& hud = RaceManager::instance().hud();
    DebugOverlay::instance().setActiveObjects(static_cast<uint32_t>(m_scene.objectCount()));
    DebugOverlay::instance().logIfNeeded(deltaTime);

    // Extra debug: speed / state
    static float dbgT = 0;
    dbgT += deltaTime;
    if (dbgT > 2.0f && RaceManager::instance().state() == RaceState::Racing) {
        NV_LOGI("RACE | Speed: %.0f km/h | Lap: %d/%d | Pos: %d | Nitro: %.0f%% | State: %s",
                hud.speedKmh, hud.currentLap, hud.totalLaps, hud.position,
                hud.nitro * 100.0f, raceStateName(RaceManager::instance().state()));
        dbgT = 0;
    }
}

void Engine::onPause() {
    m_paused = true;
    if (RaceManager::instance().state() == RaceState::Racing ||
        RaceManager::instance().state() == RaceState::Countdown) {
        RaceManager::instance().pauseRace();
    } else {
        RaceManager::instance().clearTouchState();
    }
    AudioManager::instance().pauseMusic();
}

void Engine::onResume() {
    m_paused = false;
    AudioManager::instance().resumeMusic();
}

void Engine::onDestroy() {
    shutdown();
}

void Engine::onTouch(int action, int pointerId, float x, float y) {
    RaceManager::instance().onTouch(action, pointerId, x, y, m_width, m_height);
}

void Engine::uiStartRace() { RaceManager::instance().startRace(); }
void Engine::uiSelectCar(const char* carId) {
    if (carId) RaceManager::instance().selectCar(carId);
}
void Engine::uiSetDifficulty(int d) {
    RaceManager::instance().setDifficulty(static_cast<Difficulty>(math::clamp(d, 0, 2)));
}
void Engine::uiPause() { RaceManager::instance().pauseRace(); }
void Engine::uiResume() { RaceManager::instance().resumeRace(); }
void Engine::uiRestart() { RaceManager::instance().restartRace(); }
void Engine::uiMainMenu() { RaceManager::instance().returnToMenu(); }
void Engine::uiOpenCarSelect() { RaceManager::instance().setState(RaceState::CarSelection); }
void Engine::uiOpenSettings() { RaceManager::instance().setState(RaceState::Settings); }
void Engine::uiSetGraphics(int q) {
    auto s = RaceManager::instance().settings();
    s.graphicsQuality = math::clamp(q, 0, 2);
    RaceManager::instance().applySettings(s);
}
void Engine::uiSetSoundVolume(float v) {
    auto s = RaceManager::instance().settings();
    s.soundVolume = math::clamp(v, 0.0f, 1.0f);
    RaceManager::instance().applySettings(s);
}
void Engine::uiSetMusicVolume(float v) {
    auto s = RaceManager::instance().settings();
    s.musicVolume = math::clamp(v, 0.0f, 1.0f);
    RaceManager::instance().applySettings(s);
}
void Engine::uiSetSensitivity(float s) {
    auto st = RaceManager::instance().settings();
    st.controlSensitivity = math::clamp(s, 0.3f, 2.0f);
    RaceManager::instance().applySettings(st);
}

int Engine::uiGetState() const {
    return static_cast<int>(RaceManager::instance().state());
}

void Engine::uiGetHud(float* out12) {
    const auto& h = RaceManager::instance().hud();
    if (!out12) return;
    out12[0] = h.speedKmh;
    out12[1] = static_cast<float>(h.currentLap);
    out12[2] = static_cast<float>(h.totalLaps);
    out12[3] = static_cast<float>(h.position);
    out12[4] = h.raceTime;
    out12[5] = h.nitro;
    out12[6] = static_cast<float>(h.countdown);
    out12[7] = static_cast<float>(h.state);
    out12[8] = h.bestLap;
    out12[9] = h.finalTime > 0.0f ? h.finalTime : h.raceTime;
    out12[10] = static_cast<float>(h.finalPosition > 0 ? h.finalPosition : h.position);
    out12[11] = static_cast<float>(h.difficulty);
}

} // namespace nexvora
