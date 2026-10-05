#include "ShaderManager.h"
#include "Logger.h"

namespace nexvora {

ShaderManager& ShaderManager::instance() {
    static ShaderManager inst;
    return inst;
}

std::shared_ptr<Shader> ShaderManager::getOrCreate(const std::string& name,
                                                   const char* vsSrc,
                                                   const char* fsSrc) {
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        return it->second;
    }

    auto shader = std::make_shared<Shader>();
    if (!shader->loadFromSource(vsSrc, fsSrc)) {
        NV_LOGE("Failed to create shader: %s", name.c_str());
        return nullptr;
    }

    m_shaders[name] = shader;
    NV_LOGI("Shader registered: %s", name.c_str());
    return shader;
}

std::shared_ptr<Shader> ShaderManager::get(const std::string& name) const {
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        return it->second;
    }
    return nullptr;
}

void ShaderManager::clear() {
    m_shaders.clear();
}

} // namespace nexvora
