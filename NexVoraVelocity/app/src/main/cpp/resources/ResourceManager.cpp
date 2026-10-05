#include "ResourceManager.h"
#include "Logger.h"

namespace nexvora {

ResourceManager& ResourceManager::instance() {
    static ResourceManager inst;
    return inst;
}

void ResourceManager::setAssetManager(void* assetManager) {
    m_assetManager = assetManager;
    NV_LOGI("ResourceManager: AssetManager set");
}

void ResourceManager::clear() {
    m_genericCache.clear();
    NV_LOGD("ResourceManager cleared");
}

} // namespace nexvora
