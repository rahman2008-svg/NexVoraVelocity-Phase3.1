#pragma once

#include "Renderer.h"
#include "Scene.h"
#include "InputManager.h"
#include "Time.h"
#include "audio/AudioManager.h"
#include "debug/DebugOverlay.h"
#include "gameplay/RaceManager.h"
#include "gameplay/LocalSave.h"
#include <memory>
#include <atomic>
#include <string>

namespace nexvora {

class Engine {
public:
    static Engine& instance();

    bool initialize();
    void shutdown();

    void onSurfaceCreated();
    void onSurfaceChanged(int width, int height);
    void onDrawFrame(float deltaTime);

    void onPause();
    void onResume();
    void onDestroy();

    void onTouch(int action, int pointerId, float x, float y);

    // Game control from Kotlin UI
    void uiStartRace();
    void uiSelectCar(const char* carId);
    void uiSetDifficulty(int difficulty);
    void uiPause();
    void uiResume();
    void uiRestart();
    void uiMainMenu();
    void uiOpenCarSelect();
    void uiOpenSettings();
    void uiSetGraphics(int quality);
    void uiSetSoundVolume(float v);
    void uiSetMusicVolume(float v);
    void uiSetSensitivity(float s);

    int uiGetState() const;
    // Fills out[12]: speed,lap,totalLaps,pos,raceTime,nitro,countdown,state,bestLap,finalTime,finalPos,difficulty
    void uiGetHud(float* out12);

    void setFilesDir(const char* path);

    Scene& scene() { return m_scene; }
    Renderer& renderer() { return m_renderer; }
    bool isInitialized() const { return m_initialized; }
    bool isPaused() const { return m_paused; }

private:
    Engine() = default;

    Renderer m_renderer;
    Scene m_scene;
    bool m_initialized = false;
    std::atomic<bool> m_paused{false};
    int m_width = 1;
    int m_height = 1;
    std::string m_filesDir;
};

} // namespace nexvora
