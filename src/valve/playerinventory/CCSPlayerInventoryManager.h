#pragma once

class CCSPlayerInventory;
class CCSInventoryManager
{
public:
    static CCSInventoryManager* GetInventoryManager();

    bool EquipItemInLoadout(int nTeam, int nSlot, uint64_t nItemID)
    {
        return Memory::CallVFunc<bool, 67U>(this, nTeam, nSlot, nItemID);
    }

    CCSPlayerInventory* GetLocalInventory()
    {
        return *reinterpret_cast<CCSPlayerInventory**>(reinterpret_cast<uintptr_t>(this) + 70);
    }
};
