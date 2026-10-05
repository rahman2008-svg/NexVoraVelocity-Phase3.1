#pragma once
#include <algorithm>

#include "GameObject.h"
#include "Camera.h"
#include "Light.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <cstdint>

namespace nexvora {

class Scene {
public:
    Scene() = default;

    Camera& camera() { return m_camera; }
    const Camera& camera() const { return m_camera; }

    AmbientLight& ambient() { return m_ambient; }
    const AmbientLight& ambient() const { return m_ambient; }

    std::vector<Light>& lights() { return m_lights; }
    const std::vector<Light>& lights() const { return m_lights; }

    void addLight(const Light& light) { m_lights.push_back(light); }
    void clearLights() { m_lights.clear(); }

    GameObject& createObject(const std::string& name = "") {
        auto obj = std::make_shared<GameObject>(name);
        obj->setId(m_nextId++);
        m_objects.push_back(obj);
        if (!name.empty()) m_nameLookup[name] = obj;
        return *obj;
    }

    std::shared_ptr<GameObject> addObject(std::shared_ptr<GameObject> obj) {
        if (!obj) return nullptr;
        obj->setId(m_nextId++);
        m_objects.push_back(obj);
        if (!obj->name().empty()) m_nameLookup[obj->name()] = obj;
        return obj;
    }

    void destroyObject(uint32_t id) {
        m_objects.erase(
            std::remove_if(m_objects.begin(), m_objects.end(),
                [id](const std::shared_ptr<GameObject>& o) {
                    return o && o->id() == id;
                }),
            m_objects.end());
        // Clean name lookup
        for (auto it = m_nameLookup.begin(); it != m_nameLookup.end(); ) {
            if (!it->second || it->second->id() == id)
                it = m_nameLookup.erase(it);
            else
                ++it;
        }
    }

    std::shared_ptr<GameObject> findObject(const std::string& name) const {
        auto it = m_nameLookup.find(name);
        if (it != m_nameLookup.end()) return it->second;
        return nullptr;
    }

    const std::vector<std::shared_ptr<GameObject>>& objects() const { return m_objects; }

    void update(float dt) {
        for (auto& obj : m_objects) {
            if (obj && obj->isEnabled()) obj->update(dt);
        }
    }

    void clear() {
        m_objects.clear();
        m_nameLookup.clear();
        m_lights.clear();
        m_nextId = 1;
    }

    size_t objectCount() const { return m_objects.size(); }

private:
    Camera m_camera;
    AmbientLight m_ambient;
    std::vector<Light> m_lights;
    std::vector<std::shared_ptr<GameObject>> m_objects;
    std::unordered_map<std::string, std::shared_ptr<GameObject>> m_nameLookup;
    uint32_t m_nextId = 1;
};

} // namespace nexvora
