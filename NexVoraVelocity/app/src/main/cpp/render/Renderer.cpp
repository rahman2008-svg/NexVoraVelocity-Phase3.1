#include "Renderer.h"
#include "ShaderManager.h"
#include "Logger.h"
#include "debug/DebugOverlay.h"
#include <GLES3/gl3.h>

namespace nexvora {

static const char* kLitVS = R"(#version 300 es
precision highp float;
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;
out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;
void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vUV = aUV;
    gl_Position = uMVP * vec4(aPosition, 1.0);
}
)";

static const char* kLitFS = R"(#version 300 es
precision mediump float;
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
uniform vec3 uColor;
uniform vec3 uAmbient;
uniform vec3 uViewPos;
uniform vec3 uDirLightDir;
uniform vec3 uDirLightColor;
uniform float uDirLightIntensity;
uniform vec3 uPointLightPos;
uniform vec3 uPointLightColor;
uniform float uPointLightIntensity;
uniform float uPointLightRange;
uniform float uSpecularPower;
uniform float uFogStart;
uniform float uFogEnd;
uniform vec3 uFogColor;
out vec4 fragColor;
void main() {
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vWorldPos);

    // Ambient
    vec3 ambient = uAmbient;

    // Directional
    vec3 Ldir = normalize(-uDirLightDir);
    float NdotL = max(dot(N, Ldir), 0.0);
    vec3 diffuse = uDirLightColor * uDirLightIntensity * NdotL;
    vec3 H = normalize(Ldir + V);
    float spec = pow(max(dot(N, H), 0.0), uSpecularPower);
    vec3 specular = uDirLightColor * uDirLightIntensity * spec * 0.4;

    // Point light
    vec3 toPoint = uPointLightPos - vWorldPos;
    float dist = length(toPoint);
    float atten = 1.0 / (1.0 + (dist * dist) / (uPointLightRange * uPointLightRange));
    vec3 Lp = normalize(toPoint);
    float NdotLp = max(dot(N, Lp), 0.0);
    diffuse += uPointLightColor * uPointLightIntensity * NdotLp * atten;
    vec3 Hp = normalize(Lp + V);
    specular += uPointLightColor * uPointLightIntensity * pow(max(dot(N, Hp), 0.0), uSpecularPower) * 0.3 * atten;

    vec3 finalColor = uColor * (ambient + diffuse) + specular;

    // Distance fog
    float fogFactor = clamp((uFogEnd - length(uViewPos - vWorldPos)) / (uFogEnd - uFogStart), 0.0, 1.0);
    finalColor = mix(uFogColor, finalColor, fogFactor);

    fragColor = vec4(finalColor, 1.0);
}
)";

static const char* kUnlitVS = R"(#version 300 es
precision highp float;
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
uniform mat4 uMVP;
out vec2 vUV;
void main() {
    vUV = aUV;
    gl_Position = uMVP * vec4(aPosition, 1.0);
}
)";

static const char* kUnlitFS = R"(#version 300 es
precision mediump float;
in vec2 vUV;
uniform vec3 uColor;
out vec4 fragColor;
void main() {
    fragColor = vec4(uColor, 1.0);
}
)";

bool Renderer::initialize() {
    NV_LOGI("Renderer::initialize");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.45f, 0.55f, 0.70f, 1.0f); // sky-ish, matches fog

    if (!createShaders()) {
        NV_LOGE("Failed to create shaders");
        return false;
    }

    m_ready = true;
    DebugOverlay::instance().setRendererReady(true);
    NV_LOGI("Renderer initialized successfully");
    return true;
}

void Renderer::shutdown() {
    m_litShader.reset();
    m_unlitShader.reset();
    ShaderManager::instance().clear();
    m_ready = false;
    DebugOverlay::instance().setRendererReady(false);
    NV_LOGI("Renderer shut down");
}

bool Renderer::createShaders() {
    m_litShader = ShaderManager::instance().getOrCreate("lit", kLitVS, kLitFS);
    m_unlitShader = ShaderManager::instance().getOrCreate("unlit", kUnlitVS, kUnlitFS);
    return m_litShader && m_litShader->isValid() && m_unlitShader && m_unlitShader->isValid();
}

void Renderer::resize(int width, int height) {
    m_width = width > 0 ? width : 1;
    m_height = height > 0 ? height : 1;
    glViewport(0, 0, m_width, m_height);
    DebugOverlay::instance().setScreenSize(m_width, m_height);
    NV_LOGD("Viewport resized to %dx%d", m_width, m_height);
}

void Renderer::beginFrame() {
    m_drawCalls = 0;
    DebugOverlay::instance().beginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::renderScene(Scene& scene) {
    if (!m_ready || !m_litShader) return;

    Camera& cam = scene.camera();
    cam.setAspect(static_cast<float>(m_width) / static_cast<float>(m_height));

    math::Mat4 view = cam.viewMatrix();
    math::Mat4 proj = cam.projectionMatrix();
    math::Mat4 viewProj = proj * view;
    math::Vec3 viewPos = cam.transform().position();

    // Gather lights
    math::Vec3 dirDir{-0.35f, -1.0f, -0.25f};
    math::Vec3 dirColor{1.0f, 0.98f, 0.92f};
    float dirIntensity = 1.1f;
    math::Vec3 pointPos{5.0f, 4.0f, 5.0f};
    math::Vec3 pointColor{1.0f, 0.6f, 0.3f};
    float pointIntensity = 1.5f;
    float pointRange = 25.0f;

    for (const auto& l : scene.lights()) {
        if (!l.enabled) continue;
        if (l.type == LightType::Directional) {
            dirDir = l.direction;
            dirColor = l.color;
            dirIntensity = l.intensity;
        } else if (l.type == LightType::Point) {
            pointPos = l.position;
            pointColor = l.color;
            pointIntensity = l.intensity;
            pointRange = l.range;
        }
    }

    math::Vec3 ambient = scene.ambient().color * scene.ambient().intensity;

    m_litShader->use();
    m_litShader->setVec3("uAmbient", ambient);
    m_litShader->setVec3("uViewPos", viewPos);
    m_litShader->setVec3("uDirLightDir", dirDir.normalized());
    m_litShader->setVec3("uDirLightColor", dirColor);
    m_litShader->setFloat("uDirLightIntensity", dirIntensity);
    m_litShader->setVec3("uPointLightPos", pointPos);
    m_litShader->setVec3("uPointLightColor", pointColor);
    m_litShader->setFloat("uPointLightIntensity", pointIntensity);
    m_litShader->setFloat("uPointLightRange", pointRange);
    m_litShader->setFloat("uFogStart", m_fogStart);
    m_litShader->setFloat("uFogEnd", m_fogEnd);
    m_litShader->setVec3("uFogColor", m_fogColor);

    uint32_t activeObjs = 0;
    for (const auto& obj : scene.objects()) {
        if (!obj || !obj->isVisible() || !obj->isEnabled() || !obj->mesh()) continue;
        activeObjs++;

        auto mesh = obj->mesh();
        if (!mesh->isUploaded()) mesh->upload();

        math::Mat4 model = obj->transform().worldMatrix();
        math::Mat4 mvp = viewProj * model;

        float nx[9] = {
            model.m[0], model.m[1], model.m[2],
            model.m[4], model.m[5], model.m[6],
            model.m[8], model.m[9], model.m[10]
        };

        math::Vec3 color = obj->color();
        float specPower = 32.0f;
        if (obj->material()) {
            color = obj->material()->baseColor();
            specPower = obj->material()->specularPower();
        }

        m_litShader->setMat4("uMVP", mvp);
        m_litShader->setMat4("uModel", model);
        GLint loc = glGetUniformLocation(m_litShader->program(), "uNormalMatrix");
        if (loc >= 0) glUniformMatrix3fv(loc, 1, GL_FALSE, nx);
        m_litShader->setVec3("uColor", color);
        m_litShader->setFloat("uSpecularPower", specPower);

        mesh->bind();
        mesh->draw();
        mesh->unbind();

        m_drawCalls++;
        DebugOverlay::instance().recordDrawCall(static_cast<uint32_t>(mesh->indexCount() / 3));
    }

    DebugOverlay::instance().setActiveObjects(activeObjs);
}

void Renderer::endFrame() {
    // Present handled by GLSurfaceView
}

} // namespace nexvora
