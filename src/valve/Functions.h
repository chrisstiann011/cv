#pragma once

class CGameEntitySystem;

class CCSInventoryManager;

class CGCClient;
class CGCClientSharedObjectCache;
class CGCClientSharedObjectTypeCache;
class CGCClientSystem;

class CEconItem;
class CVPhys2World;
class CMaterial2;
class CKeyValues3;
class CDrawMesh;

class C_BaseEntity;
class C_BaseModelEntity;
class C_CSPlayerPawn;
class CEffectData;
class CSkeletonInstance;
class CModel;
class C_EconItemView;
class CCollisionProperty;
class C_BasePlayerWeapon;

struct MaterialKeyVar_t;
struct SOID_t;
struct Ray_t;
struct TraceFilter_t;
struct GameTrace_t;

struct CPaintKit;

struct InputKey_t;

class CGameParticleManager;
class CParticleEffect
{
public:
	const char* name{};
	char pad[0x30]{};
};
class CParticleSnapshot;
class C_CSWeaponBase;

namespace Functions
{
	bool Setup( );

	inline std::int64_t(__fastcall* fnSetViewAngle)(CCSGOInput*, std::int32_t, const QAngle) = NULL;
	inline std::int64_t(__fastcall* fnGetViewAngles)(CCSGOInput*, std::int32_t) = NULL;
	inline void* (*fnCEconItemSchema_GetAttributeDefinitionInterface)(void*, int) = NULL;
	inline void* (*fnCGCClientSharedObjectCache_FindTypeCache)(void*, int) = NULL;

	inline void* (__fastcall* fnGetBaseEntity)(CGameEntitySystem*, int) = nullptr;

	inline void(__fastcall* fnCalculateWorldSpaceBones)(CSkeletonInstance*, std::uint32_t) = NULL;

	inline int(__fastcall* fnLookupBone)(C_BaseEntity*, const char*) = NULL;

	inline std::int32_t(__fastcall* fnGetBoneFlags)(CModel*, std::int32_t) = NULL;
	inline std::int32_t(__fastcall* fnGetBoneParent)(CModel*, std::int32_t) = NULL;
	inline std::int32_t(__fastcall* fnGetBoneCount)(CModel*) = NULL;

	inline void(__fastcall* fnInterpolateShootPosition)(void*, Vector*, int, float) = NULL;

	inline bool(__fastcall* fnTraceShape)(CVPhys2World*, Ray_t*, const std::reference_wrapper<Vector>, const std::reference_wrapper<Vector>, TraceFilter_t*, GameTrace_t*) = NULL;

	inline void(__fastcall* fnClipTraceToPlayers)(const std::reference_wrapper<Vector>, const std::reference_wrapper<Vector>, TraceFilter_t*, GameTrace_t*, float, float, float) = NULL;
	inline bool(__fastcall* fnClipRayToEntity)(CVPhys2World*, Ray_t*, const std::reference_wrapper<Vector>, const std::reference_wrapper<Vector>, C_CSPlayerPawn*, TraceFilter_t*, GameTrace_t*) = NULL;
	inline std::uint64_t(__fastcall* fnGetSurfaceData)(void*) = NULL;

	inline void(__fastcall* fnUpdateSubClass)(C_BaseEntity*) = NULL;

	inline bool(__fastcall* fnUnknownParticleFunction)(CGameParticleManager*, int, unsigned int, const CStrongHandle<CParticleSnapshot>*) = NULL;
	inline int* (__fastcall* fnCacheParticleEffect)(CGameParticleManager*, unsigned int*, const char*, int, __int64, __int64, __int64, int) = nullptr;
	inline bool(__fastcall* fnCreateParticleEffect)(CGameParticleManager*, unsigned int, int, void*, int) = NULL;

	inline std::int64_t(__fastcall* fnCreateMaterial)(void*, void*, const char*, CKeyValues3*, unsigned int, unsigned int) = NULL;

	inline void(__fastcall* fnGetPunchAngle)(C_CSPlayerPawn*, QAngle*, float, bool) = NULL;
	inline float(__fastcall* fnTraceSmokeDensity)(Vector, Vector, std::nullptr_t) = NULL;
	inline bool(__fastcall* fnSetPlayerReady)(std::nullptr_t, const char*) = NULL;

	inline std::uint32_t(__fastcall* fnGetEntityHandle)(C_CSPlayerPawn*) = NULL;

	inline CCSInventoryManager*(__cdecl* fnGetInventoryManager)() = nullptr;

	inline void* (__fastcall* fnUpdateCompositeMaterial)(void*, int) = nullptr;
	inline void(__fastcall* fnRegenerateWeaponSkin)(C_BasePlayerWeapon*, bool) = nullptr;

	inline void(__fastcall* fnSetModel)(C_BaseModelEntity*, const char*) = NULL;

	inline void(__fastcall* fnSetMeshGroupMask)(CGameSceneNode*, std::uint64_t) = NULL;

	inline void(__thiscall* fnAddStatTrackEntity)(C_CSWeaponBase*, C_EconItemView*) = NULL;
	inline void(__thiscall* fnAddNametagEntity)(C_CSWeaponBase*, C_EconItemView*) = NULL;

	inline CEconItem*(__cdecl* fnCreateSharedObjectSubclassEconItem)() = nullptr;
	inline CGCClientSharedObjectCache*(__fastcall* fnFindSOCache)(void*, SOID_t, bool) = nullptr;
	inline CGCClientSharedObjectTypeCache*(__fastcall* fnCreateBaseTypeCache)(CGCClientSharedObjectCache*, int) = nullptr;
	inline CGCClientSystem*(__cdecl* fnGetClientSystem)() = nullptr;

	inline void*(__fastcall* fnSetDynamicAttributeValue)(CEconItem*, void*, void*) = nullptr;
	inline void*(__fastcall* fnCAttributeStringInit)(std::uint8_t[0x100], unsigned long long, bool) = nullptr;
	inline void*(__fastcall* fnCAttributeStringFill)(void*, const char*) = nullptr;
	inline void*(__fastcall* fnSetDynamicAttributeValueString)(CEconItem*, void*, std::uint8_t*) = nullptr;

	inline void(__fastcall* fnDispatchEffect)(const char*, CEffectData*) = nullptr;

	inline void*(__fastcall* fnLoadFileForMe)(const char*, int*);

	inline CSubtickMoveStep* (__fastcall* fnCreateNewSubtickMoveStep)(void*) = nullptr;

	inline void(__cdecl* fnConColorMsg)(const Color& col, const char* szFormat, ...) = nullptr;
}