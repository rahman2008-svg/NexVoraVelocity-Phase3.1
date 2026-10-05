#pragma once

#include "Texture.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace nexvora {

class TextureManager {
public:
    static TextureManager& instance();

    std::shared_ptr<Texture> createSolid(const std::string& name,
                                         uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    std::shared_ptr<Texture> get(const std::string& name) const;

    void clear();

private:
    TextureManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
};

} // namespace nexvora
