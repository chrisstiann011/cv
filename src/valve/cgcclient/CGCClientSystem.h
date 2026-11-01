#pragma once

class CGCClientSystem
{
public:
    static CGCClientSystem* GetClientSystem()
    {
        #ifdef CS_PARANOID
        CS_ASSERT(Functions::fnGetClientSystem != nullptr);
        #endif

        return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnGetClientSystem);
	}

    CGCClient* GetCGCClient()
    {
        return reinterpret_cast<CGCClient*>(reinterpret_cast<std::uintptr_t>(this) + 0xB8);
    }
};