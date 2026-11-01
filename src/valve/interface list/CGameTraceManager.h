#pragma once
#define TRACE_MASK 0x1C3003
#define TRACE_MASK_VISIBLE 0x1C1003
#define TRACE_MASK_AUTOWALL 0x1C300B
#define TRACE_MASK_GRENADE 0x2000C3001

struct HandleBulletDataObj_t
{
	float m_flDmg;
	float m_flPen;
	float m_flRangeMod;
	float m_flRange;

	int m_iPenCount;

	bool m_bFailed;
};

class CVPhys2World
{
public:
	bool TraceShape(Ray_t* pRay, Vector vecStart, Vector vecEnd, TraceFilter_t* pFilter, GameTrace_t* pGameTrace)
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnTraceShape != nullptr);
#endif

		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnTraceShape, this, pRay, std::ref(vecStart), std::ref(vecEnd), pFilter, pGameTrace);
	}

	void ClipTraceToPlayers(Vector vecStart, Vector vecEnd, TraceFilter_t* pFilter, GameTrace_t* pGameTrace, float flMinimum, float flLength, float flMaximum)
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnClipTraceToPlayers != nullptr);
#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnClipTraceToPlayers, std::ref(vecStart), std::ref(vecEnd), pFilter, pGameTrace, flMinimum, flLength, flMaximum);
	}

	bool ClipRayToEntity(Ray_t* pRay, Vector vecStart, Vector vecEnd, C_CSPlayerPawn* pPawn, TraceFilter_t* pFilter, GameTrace_t* pGameTrace)
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnClipRayToEntity != nullptr);
#endif

		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnClipRayToEntity, this, pRay, std::ref(vecStart), std::ref(vecEnd), pPawn, pFilter, pGameTrace);
	}

	void TraceToExit(TraceData_t* const trace, const Vector start, const Vector end, const TraceFilter_t& filler, const int penetration_count) {
		using func_t = void(__fastcall*)(TraceData_t*, Vector, Vector, TraceFilter_t, void*, void*, void*, void*, int);

		static func_t fn = (func_t)Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? F2 0F 10 02"));

		return fn(trace, start, end, filler, nullptr, nullptr, nullptr, nullptr, penetration_count);
	}

	void GetTraceInfo(TraceData_t* pTraceData, GameTrace_t* pTrace, float fl, PVOID unk)
	{
		using func_t = void(__fastcall*)(TraceData_t*, GameTrace_t*, float, PVOID unk);
		static func_t fn = (func_t)Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B E9 0F 29 74 24 ? 48 8B CA"));

		return fn(pTraceData, pTrace, fl, unk);
	}

	void InitFilter(TraceFilter_t& filter, C_CSPlayerPawn* skip, uint64_t mask, uint8_t layer, uint16_t idk)
	{
		using func_t = TraceFilter_t * (__fastcall*)(TraceFilter_t&, void*, uint64_t, uint8_t, uint16_t);

		static func_t fn = (func_t)Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 0F ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 0C 49 C6 41"));

		fn(filter, skip, mask, layer, idk);
	}

	bool HandleBulletPenetration(TraceData_t* pTraceData, HandleBulletDataObj_t* objHandleBulletData, UpdateValue_t* mod_value, int nTeamNum)
	{
		using fn_t = bool(__fastcall*)(TraceData_t*, HandleBulletDataObj_t*, UpdateValue_t*, int, PVOID);

		static fn_t fn = (fn_t)Memory::FindPattern(CLIENT_DLL, X("48 8B C4 44 89 48 ? 48 89 50 ? 48 89 48 ? 55 57"));

		return fn(pTraceData, objHandleBulletData, mod_value, nTeamNum, nullptr);
	}

	void InitTraceInfo(GameTrace_t* hit) {
		using func_t = void(__fastcall*)(GameTrace_t*);

		static func_t fn = (func_t)Memory::FindPattern(CLIENT_DLL, X("40 55 41 55 41 57 48 83 EC"));

		return fn(hit);
	}

	void InitGameTrace(GameTrace_t* trace) {
		using fn = void(__fastcall*)(GameTrace_t*);
		static auto x = (fn)(Memory::FindPattern(CLIENT_DLL, "40 55 41 55 41 57 48 83 EC"));
		x(trace);
	}

	void InitPlayerMovementTraceFilter(TraceFilter_t* pFilter, void* pHandleEntity_CEntityInstance, uint64_t interactWith, int collisionGroup)
	{
		using func_t = void(__fastcall*)(TraceFilter_t*, void*, uint64_t, int);

		static func_t fn = (func_t)Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 0F B6 41 ? 33 FF 66 C7 41"));

		return fn(pFilter, pHandleEntity_CEntityInstance, interactWith, collisionGroup);
	}

	bool TracePlayerBBox(Vector* vStart, Vector* vEnd, void* bounds, TraceFilter_t* pFilter, GameTrace_t* pGameTrace)
	{
		using func_t = bool(__fastcall*)(Vector*, Vector*, void*, TraceFilter_t*, GameTrace_t*);

		static func_t fn = (func_t)Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 55 57 41 54 41 55 41 56"));

		return fn(vStart, vEnd, bounds, pFilter, pGameTrace);
	}
};