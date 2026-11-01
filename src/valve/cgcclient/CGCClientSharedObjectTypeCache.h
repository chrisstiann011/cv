#pragma once

class CSharedObject;
class CGCClientSharedObjectTypeCache
{
public:
    bool AddObject(CSharedObject* pObject)
    {
        return Memory::CallVFunc<bool, 1U>(this, pObject);
    }

    CSharedObject* RemoveObject(CSharedObject* soIndex)
    {
        return Memory::CallVFunc<CSharedObject*, 3U>(this, soIndex);
    }

    template <typename T>
    auto& GetVecObjects()
    {
        return *reinterpret_cast<CUtlVector<T>*>(reinterpret_cast<std::uintptr_t>(this) + 0x8);
    }
};
