#pragma once

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS 64
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS

class C_BaseEntity;
class CGameEntitySystem
{
public:

	template <typename T = C_BaseEntity>
	T* Get( int nIndex )
	{
		return reinterpret_cast< T* >( this->GetEntityByIndex( nIndex ) );
	}

	template <typename T = C_BaseEntity>
	T* Get( const CBaseHandle hHandle )
	{
		if (!hHandle.IsValid( ))
			return nullptr;

		return reinterpret_cast< T* >( this->GetEntityByIndex( hHandle.GetEntryIndex( ) ) );
	}

	int GetHighestEntityIndex( )
	{
		return *reinterpret_cast< int* >( reinterpret_cast< std::uintptr_t >( this ) + 0x20A0);
	}

	template<class T>
	T* CreateEntityByClassName(const char* szName) {

		using fn_t = void* (__fastcall*)(void*, int, const char*, int, int, int, char);
		static fn_t fn = (fn_t)Memory::FindPattern(CLIENT_DLL, "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC ? 49 8B F8 44 8B F2");
		return reinterpret_cast<T*>(fn(this, -1, szName, 0, -1, -1, 0));
	}

private:
	void* GetEntityByIndex( int nIndex )
	{

		if (Interfaces::m_pEntityList)
		{
			std::uintptr_t entity_list = *Interfaces::m_pEntityList;
			if (!entity_list)
				return nullptr;
			std::uintptr_t entry = *reinterpret_cast<std::uintptr_t*>(entity_list + 0x8 * ((nIndex & 0x7FFF) >> 9));
			if (!entry)
				return nullptr;
			return reinterpret_cast<void*>(*reinterpret_cast<std::uintptr_t*>(entry + 112 * (nIndex & 0x1FF)));
		}
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnGetBaseEntity != nullptr);
		#endif
		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnGetBaseEntity, this, nIndex);
	}
};