#include "../../Precompiled.h"

CCSInventoryManager* CCSInventoryManager::GetInventoryManager()
{
	if ( !Functions::fnGetInventoryManager )
		return nullptr;

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnGetInventoryManager);
}