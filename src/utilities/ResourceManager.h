#pragma once
#include <unordered_map>
#include <string>

class CResourceManager
{
public:
    void LoadResources(std::unordered_map<int, std::string> mapResources);
    void ReloadResources(std::vector<std::unordered_map<int, std::string>> vecResources);
    void InvalidateResources();

    bool GetResource(const std::string& name);

    void LoadResourceOnce(const std::string& name);

private:
    std::unordered_map<std::string, bool> m_mapLoadedResources;
};
inline auto g_ResourceManager = std::make_unique<CResourceManager>();