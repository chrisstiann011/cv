#pragma once

class CSharedObject;
class C_EconItemView;
class CEconItem;
class CCSPlayerInventory
{
public:
	static CCSPlayerInventory* GetPlayerInventory();

	bool AddEconItem(CEconItem* pItem);
	void RemoveEconItem(CEconItem* pItem);

	std::pair<uint64_t, uint32_t> GetHighestIDs();

	C_EconItemView* GetItemViewForItem(uint64_t itemID);
	CEconItem* GetSOCDataForItem(uint64_t itemID);

	void SOCreated(SOID_t owner, CSharedObject* pObject, ESOCacheEvent eEvent)
	{
		return Memory::CallVFunc<void, 0U>(this, owner, pObject, eEvent);
	}

	void SOUpdated(SOID_t owner, CSharedObject* pObject, ESOCacheEvent eEvent)
	{
		return Memory::CallVFunc<void, 1U>(this, owner, pObject, eEvent);
	}

	void SODestroyed(SOID_t owner, CSharedObject* pObject, ESOCacheEvent eEvent)
	{
		return Memory::CallVFunc<void, 2U>(this, owner, pObject, eEvent);
	}

	C_EconItemView* GetItemInLoadout(int nClass, int nSlot)
	{
		return Memory::CallVFunc < C_EconItemView*, 8U>(this, nClass, nSlot);
	}

	SOID_t GetOwner()
	{
		if (!this || (reinterpret_cast<std::uintptr_t>(this) >> 47) != 0)
			return SOID_t{};
		return *reinterpret_cast<SOID_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x10);
	}

	auto& GetItemVector()
	{
		return *reinterpret_cast<CUtlVector<C_EconItemView*>*>((uintptr_t)(this) + 0x20);
	}

	auto GetBaseTypeCache() -> CGCClientSharedObjectTypeCache*;
};

