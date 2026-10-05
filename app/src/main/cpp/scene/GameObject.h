#pragma once

#include "Transform.h"
#include "Mesh.h"
#include "materials/Material.h"
#include <string>
#include <memory>
#include <vector>
#include <cstdint>

namespace nexvora {

class GameObject {
public:
    GameObject() = default;
    explicit GameObject(const std::string& name) : m_name(name) {}

    uint32_t id() const { return m_id; }
    void setId(uint32_t id) { m_id = id; }

    Transform& transform() { return m_transform; }
    const Transform& transform() const { return m_transform; }

    void setMesh(std::shared_ptr<Mesh> mesh) { m_mesh = std::move(mesh); }
    std::shared_ptr<Mesh> mesh() const { return m_mesh; }

    void setMaterial(std::shared_ptr<Material> mat) { m_material = std::move(mat); }
    std::shared_ptr<Material> material() const { return m_material; }

    // Legacy color fallback when no material
    void setColor(const math::Vec3& color) { m_color = color; }
    const math::Vec3& color() const { return m_color; }

    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    const std::string& name() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    // Hierarchy helpers (via Transform)
    void setParent(GameObject* parent) {
        m_parent = parent;
        if (parent) {
            m_transform.setParent(&parent->m_transform);
        } else {
            m_transform.setParent(nullptr);
        }
    }
    GameObject* parent() const { return m_parent; }

    void update(float /*dt*/) {
        // Component update hook for future systems
    }

private:
    uint32_t m_id = 0;
    std::string m_name;
    Transform m_transform;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;
    math::Vec3 m_color{1.0f, 1.0f, 1.0f};
    bool m_visible = true;
    bool m_enabled = true;
    GameObject* m_parent = nullptr;
};

} // namespace nexvora
