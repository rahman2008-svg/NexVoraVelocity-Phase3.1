#include "DebugOverlay.h"
#include "Logger.h"
#include "Time.h"

namespace nexvora {

DebugOverlay& DebugOverlay::instance() {
    static DebugOverlay inst;
    return inst;
}

void DebugOverlay::beginFrame() {
    m_frameDrawCalls = 0;
    m_frameTriangles = 0;
}

void DebugOverlay::endFrame(float deltaTime) {
    m_stats.drawCalls = m_frameDrawCalls;
    m_stats.triangleCount = m_frameTriangles;
    m_stats.fps = Time::fps();
    m_stats.frameTimeMs = deltaTime * 1000.0f;
}

void DebugOverlay::recordDrawCall(uint32_t triangles) {
    m_frameDrawCalls++;
    m_frameTriangles += triangles;
}

void DebugOverlay::logIfNeeded(float deltaTime) {
    if (!m_enabled) return;
    m_logTimer += deltaTime;
    if (m_logTimer >= 2.0f) {
        NV_LOGI("DBG | FPS: %.1f | Frame: %.2fms | Draws: %u | Tris: %u | Objs: %u | %dx%d | GL:%s",
                m_stats.fps,
                m_stats.frameTimeMs,
                m_stats.drawCalls,
                m_stats.triangleCount,
                m_stats.activeObjects,
                m_stats.screenWidth,
                m_stats.screenHeight,
                m_stats.rendererReady ? "OK" : "NO");
        m_logTimer = 0.0f;
    }
}

} // namespace nexvora
