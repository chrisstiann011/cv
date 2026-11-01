#pragma once

class CEconItem
{
public:
	void SetDynamicAttributeValue(int nIndex, void* pValue)
	{
		CEconItemSchema* pItemSchema = Interfaces::m_pClient->GetEconItemSystem()->GetEconItemSchema();
		if (!pItemSchema)
			return;

		void* pAttributeDefinitionInterface = pItemSchema->GetAttributeDefinitionInterface(nIndex);
		if (!pAttributeDefinitionInterface)
			return;

		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnSetDynamicAttributeValue != nullptr);
		#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnSetDynamicAttributeValue, this, pAttributeDefinitionInterface, pValue);
	}

	void SetDynamicAttributeValueString(int nIndex, const char* szValue)
	{
		CEconItemSchema* pItemSchema = Interfaces::m_pClient->GetEconItemSystem()->GetEconItemSchema();
		if (!pItemSchema)
			return;

		void* pAttributeDefinitionInterface = pItemSchema->GetAttributeDefinitionInterface(nIndex);
		if (!pAttributeDefinitionInterface)
			return;

		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnCAttributeStringInit != nullptr);
		CS_ASSERT(Functions::fnCAttributeStringFill != nullptr);
		CS_ASSERT(Functions::fnSetDynamicAttributeValueString != nullptr);
		#endif

		std::uint8_t uAttributeStringData[0x100] = {};
		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnCAttributeStringInit, uAttributeStringData, 0Ui64, false);

		*reinterpret_cast<uint32_t*>(uAttributeStringData + 0x10) = 1;
		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnCAttributeStringFill, reinterpret_cast<void*>(uAttributeStringData + 0x20), szValue);

		*reinterpret_cast<void**>(uAttributeStringData + 0x18) = reinterpret_cast<void*>(uAttributeStringData + 0x20);
		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnSetDynamicAttributeValueString, this, pAttributeDefinitionInterface, uAttributeStringData);
	}

	static CEconItem* CreateInstance()
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnCreateSharedObjectSubclassEconItem != nullptr);
		#endif

		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnCreateSharedObjectSubclassEconItem);
	}

	void Destruct()
	{
		return Memory::CallVFunc<void, 1U>(this, true);
	}

	void SetPaintKit(float flPaintKit)
	{
		SetDynamicAttributeValue(6, &flPaintKit);
	}

	void SetPaintSeed(float flSeed)
	{
		SetDynamicAttributeValue(7, &flSeed);
	}

	void SetPaintWear(float flWear)
	{
		SetDynamicAttributeValue(8, &flWear);
	}

	void SetStatTrak(int nCount)
	{
		SetDynamicAttributeValue(80, &nCount);
	}

	void SetStatTrakType(int nType)
	{
		SetDynamicAttributeValue(81, &nType);
	}

	void SetCustomName(const char* szName)
	{
		SetDynamicAttributeValueString(111, szName);
	}

	void SetSticker(int nSlot, int nDefinitionIndex, float flWear = 0.0f, float flScale = 1.0f, float flRotation = 0.0f, float OffsetX = 0.0f, float OffsetY = 0.0f)
	{
		if (nSlot < 0 || nSlot > 4)
			return;

		SetDynamicAttributeValue(114 + (nSlot * 4), &nDefinitionIndex);
		SetDynamicAttributeValue(115 + (nSlot * 4), &flWear);
		SetDynamicAttributeValue(116 + (nSlot * 4), &flScale);
		SetDynamicAttributeValue(117 + (nSlot * 4), &flRotation);

		SetDynamicAttributeValue(278 + (nSlot * 2), &OffsetX);
		SetDynamicAttributeValue(279 + (nSlot * 2), &OffsetY);

	}

public:
	MEM_PAD(0x10);
	std::uint64_t m_uID;
	std::uint64_t m_uOriginalID;
	void* m_pCustomDataOptimizedObject;
	std::uint32_t m_uAccountID;
	std::uint32_t m_uInventory;
	std::uint16_t m_uDefinitionIndex;
	std::uint16_t m_nOrigin : 5;
	std::uint16_t m_nQuality : 4;
	std::uint16_t m_nLevel : 2;
	std::uint16_t m_nRarity : 4;
	std::uint16_t m_uDirtyBitInUse : 1;
	std::int16_t m_iItemSet;
	int m_bSOUpdateFrame;
	std::uint8_t m_uFlags;
};