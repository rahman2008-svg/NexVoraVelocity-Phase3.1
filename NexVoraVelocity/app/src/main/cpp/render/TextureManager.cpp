#include "TextureManager.h"
#include "Logger.h"

namespace nexvora {

TextureManager& TextureManager::instance() {
    static TextureManager inst;
    return inst;
}

std::shared_ptr<Texture> TextureManager::createSolid(const std::string& name,
                                                     uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        return it->second;
    }

    auto tex = std::make_shared<Texture>();
    if (!tex->createSolidColor(r, g, b, a)) {
        NV_LOGE("Failed to create solid texture: %s", name.c_str());
        return nullptr;
    }

    m_textures[name] = tex;
    return tex;
}

std::shared_ptr<Texture> TextureManager::get(const std::string& name) const {
    auto it = m_textures.find(name);
    if (it != m_textures.end()) {
        return it->second;
    }
    return nullptr;
}

void TextureManager::clear() {
    m_textures.clear();
}

} // namespace nexvora
