#pragma once

class CBufferString;
namespace Functions
{
	inline bool(__fastcall* fnCBufferStringInit)(CBufferString*, const char*) = NULL;
	inline void(__fastcall* fnPurgeCBufferString)(CBufferString*, std::uint32_t) = nullptr;
}

struct ResourceBinding_t;
class CBufferString
{
	std::uint32_t m_nUnknown1 = 0;
	std::uint32_t m_nUnknown2 = 0xC00000C8;
	union
	{
		std::uintptr_t m_uStrPtr;
		std::uint8_t data[0xC8];
	};
	std::uint64_t m_nUnknown3 = 0;
	std::uint64_t m_nUnknown4 = 0;
public:
	void LoadPath(const char* szPath) noexcept
	{
		using fnLoadPath = void(__fastcall*)(CBufferString*, const char*);
		static const fnLoadPath oLoadPath = reinterpret_cast<fnLoadPath>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 8B 44 24 2C")), 0x1, 0x0));

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, oLoadPath, this, szPath);
	}

	CBufferString(const char* szString, const uint64_t uExt)
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnCBufferStringInit != nullptr);
		#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnCBufferStringInit, this, szString);
		m_nUnknown4 = uExt;
	}

	~CBufferString()
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnPurgeCBufferString != nullptr);
		#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pTier0Gadget, Functions::fnPurgeCBufferString, this, 0U);
	}
};

class IResourceSystem
{
public:
	void* QueryInterface(const char* szInterfaceName)
	{
		return Memory::CallVFunc<void*, 2U>(this, szInterfaceName);
	}

	void* GetLoadedResource(CBufferString path) const
	{
		return Memory::CallVFunc<void*, 31U>(this, path, 0);
	}

	void EnumerateMaterials(uint64_t iTypeHash, MaterialArray_t* pResult, uint8_t Flag) const
	{
		Memory::CallVFunc<void, 38>(this, iTypeHash, pResult, Flag);
	}

	ResourceBinding_t* BlockingLoadResourceByName(CBufferString* path, const char* szName)
	{
		return Memory::CallVFunc<ResourceBinding_t*, 41>(this, path, szName);
	}

	void* LoadResource(CBufferString path, const char* szName) const
	{
		return Memory::CallVFunc<void*, 42>(this, path, szName);
	}
};

class CResourceHandleUtils
{
public:
	void DeleteResource(const ResourceBinding_t* pBinding)
	{
		Memory::CallVFunc<void, 2U>(this, pBinding);
	}
};