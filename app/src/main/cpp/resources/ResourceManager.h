#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace nexvora {

class ResourceManager {
public:
    static ResourceManager& instance();

    void setAssetManager(void* assetManager);
    void* assetManager() const { return m_assetManager; }

    // Generic cache helpers
    template<typename T>
    void cache(const std::string& key, std::shared_ptr<T> resource) {
        m_genericCache[key] = resource;
    }

    template<typename T>
    std::shared_ptr<T> get(const std::string& key) const {
        auto it = m_genericCache.find(key);
        if (it != m_genericCache.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    bool has(const std::string& key) const {
        return m_genericCache.count(key) > 0;
    }

    void release(const std::string& key) {
        m_genericCache.erase(key);
    }

    void clear();

private:
    ResourceManager() = default;
    void* m_assetManager = nullptr;
    std::unordered_map<std::string, std::shared_ptr<void>> m_genericCache;
};

} // namespace nexvora
