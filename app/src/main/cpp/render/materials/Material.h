#pragma once

#include "Math.h"
#include "Shader.h"
#include "Texture.h"
#include <memory>
#include <string>

namespace nexvora {

class Material {
public:
    Material() = default;
    explicit Material(const std::string& name) : m_name(name) {}

    void setShader(std::shared_ptr<Shader> shader) { m_shader = std::move(shader); }
    std::shared_ptr<Shader> shader() const { return m_shader; }

    void setBaseColor(const math::Vec3& c) { m_baseColor = c; }
    const math::Vec3& baseColor() const { return m_baseColor; }

    void setTexture(std::shared_ptr<Texture> tex) { m_texture = std::move(tex); }
    std::shared_ptr<Texture> texture() const { return m_texture; }

    void setRoughness(float r) { m_roughness = math::clamp(r, 0.0f, 1.0f); }
    float roughness() const { return m_roughness; }

    void setMetallic(float m) { m_metallic = math::clamp(m, 0.0f, 1.0f); }
    float metallic() const { return m_metallic; }

    void setOpacity(float o) { m_opacity = math::clamp(o, 0.0f, 1.0f); }
    float opacity() const { return m_opacity; }

    void setSpecularPower(float p) { m_specularPower = p; }
    float specularPower() const { return m_specularPower; }

    const std::string& name() const { return m_name; }

    void bind() const {
        if (m_shader) m_shader->use();
        if (m_texture && m_texture->isValid()) {
            m_texture->bind(0);
        }
    }

private:
    std::string m_name;
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture> m_texture;
    math::Vec3 m_baseColor{1.0f, 1.0f, 1.0f};
    float m_roughness = 0.5f;
    float m_metallic = 0.0f;
    float m_opacity = 1.0f;
    float m_specularPower = 32.0f;
};

} // namespace nexvora
