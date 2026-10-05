#pragma once

#include <cstdint>
#include <string>

namespace nexvora {

struct DebugStats {
    float fps = 0.0f;
    float frameTimeMs = 0.0f;
    uint32_t drawCalls = 0;
    uint32_t activeObjects = 0;
    uint32_t triangleCount = 0;
    int screenWidth = 0;
    int screenHeight = 0;
    bool rendererReady = false;
};

class DebugOverlay {
public:
    static DebugOverlay& instance();

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    void beginFrame();
    void endFrame(float deltaTime);
    void recordDrawCall(uint32_t triangles = 0);
    void setActiveObjects(uint32_t count) { m_stats.activeObjects = count; }
    void setScreenSize(int w, int h) { m_stats.screenWidth = w; m_stats.screenHeight = h; }
    void setRendererReady(bool ready) { m_stats.rendererReady = ready; }

    const DebugStats& stats() const { return m_stats; }

    // Called periodically to log to logcat
    void logIfNeeded(float deltaTime);

private:
    DebugOverlay() = default;
    bool m_enabled = true;
    DebugStats m_stats;
    float m_logTimer = 0.0f;
    uint32_t m_frameDrawCalls = 0;
    uint32_t m_frameTriangles = 0;
};

} // namespace nexvora
