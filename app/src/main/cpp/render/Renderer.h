#pragma once

#include "Shader.h"
#include "Mesh.h"
#include "Scene.h"
#include "Math.h"
#include "Light.h"
#include <memory>

namespace nexvora {

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    bool initialize();
    void shutdown();

    void resize(int width, int height);
    void beginFrame();
    void renderScene(Scene& scene);
    void endFrame();

    int width() const { return m_width; }
    int height() const { return m_height; }
    bool isReady() const { return m_ready; }

    uint32_t lastDrawCalls() const { return m_drawCalls; }

private:
    bool createShaders();

    int m_width = 1;
    int m_height = 1;
    bool m_ready = false;
    uint32_t m_drawCalls = 0;

    std::shared_ptr<Shader> m_litShader;
    std::shared_ptr<Shader> m_unlitShader;

    // Fog
    float m_fogStart = 40.0f;
    float m_fogEnd = 120.0f;
    math::Vec3 m_fogColor{0.45f, 0.55f, 0.70f};
};

} // namespace nexvora
