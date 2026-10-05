#pragma once

#include "Shader.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace nexvora {

class ShaderManager {
public:
    static ShaderManager& instance();

    std::shared_ptr<Shader> getOrCreate(const std::string& name,
                                        const char* vsSrc,
                                        const char* fsSrc);

    std::shared_ptr<Shader> get(const std::string& name) const;

    void clear();

private:
    ShaderManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
};

} // namespace nexvora
