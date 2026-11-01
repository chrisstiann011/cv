#include "../Precompiled.h"

void CResourceManager::LoadResources(std::unordered_map<int, std::string> mapResources)
{
    for (const auto& [key, path] : mapResources)
    {
        LoadResourceOnce(path);
    }
}

void CResourceManager::ReloadResources(std::vector<std::unordered_map<int, std::string>> vecResources)
{
    m_mapLoadedResources.clear();

    for (auto& mapResource : vecResources)
    {
        LoadResources(mapResource);
    }
}

void CResourceManager::InvalidateResources()
{
    for (auto& objResource : m_mapLoadedResources)
    {
        objResource.second = false;
    }
}

void CResourceManager::LoadResourceOnce(const std::string& name)
{
    auto it = m_mapLoadedResources.find(name);
    if (it != m_mapLoadedResources.end())
        return;

    Interfaces::m_pResourceSystem->LoadResource(CBufferString(name.c_str(), 0), "");
    auto niga = CBufferString(name.c_str(), 0x78746576);
    Interfaces::m_pResourceSystem->BlockingLoadResourceByName(&niga, name.c_str());

    m_mapLoadedResources[name] = true;
}

bool CResourceManager::GetResource(const std::string& name)
{
    auto it = m_mapLoadedResources.find(name);
    if (it != m_mapLoadedResources.end())
        return it->second;

    return false;
}
