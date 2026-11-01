#pragma once

using GameTime_t = std::float_t;
using GameTick_t = std::int32_t;

using CFiringModeFloat = std::float_t[2];
using CSkillFloat = std::float_t[4];

struct BoneData_t
{
	Vector vecPosition;
	float flScale;
	Vector4D vecRotation;
};

class CEntitySubClassVDataBase
{
public:
	template <typename T> requires std::derived_from<T, CEntitySubClassVDataBase>
	inline T* As()
	{
		return (T*)this;
	}
};

struct PlayerContext_t
{
	int32_t m_nPlayerRenderTick;
	float_t m_flRenderTickFraction;
	MEM_PAD(0x4);
	int32_t m_nSomeTickCount;
	Vector m_vecSomeVector;
	MEM_PAD(971);
};

class CCollisionProperty;
class CEntityInstance;
class CEntityIdentity
{
public:

	SCHEMA_ADD_OFFSET(std::uint32_t, nIndex, 0x10);
	SCHEMA_ADD_FIELD(const char*, m_designerName, "CEntityIdentity->m_designerName");
	SCHEMA_ADD_FIELD(std::uint32_t, m_flags, "CEntityIdentity->m_flags");

	[[nodiscard]] bool IsValid()
	{
		return nIndex() != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int GetEntryIndex()
	{
		if (!IsValid())
			return ENT_ENTRY_MASK;

		return nIndex() & ENT_ENTRY_MASK;
	}

	[[nodiscard]] int GetSerialNumber()
	{
		if (nIndex() == INVALID_EHANDLE_INDEX || nIndex() == 0)
			return 0;

		return nIndex() >> NUM_SERIAL_NUM_SHIFT_BITS;
	}

	CEntityInstance* m_pInstance;
};

class CEntityInstance
{
public:
	void GetSchemaClassInfo(SchemaClassInfoData_t** pReturn)
	{
		return Memory::CallVFunc<void, 44>(this, pReturn);
	}

	[[nodiscard]] CBaseHandle GetRefEHandle()
	{
		CEntityIdentity* pIdentity = m_pEntity();
		if (pIdentity == nullptr)
			return CBaseHandle();

		return CBaseHandle(pIdentity->GetEntryIndex(), pIdentity->GetSerialNumber() - (pIdentity->m_flags() & 1));
	}

	void OnPreDataChanged(EDataUpdateType nUpdateType)
	{
		return Memory::CallVFunc<void, 4U>(this, nUpdateType);
	}

	void OnDataChanged(EDataUpdateType nUpdateType)
	{
		return Memory::CallVFunc<void, 5U>(this, nUpdateType);
	}

	void PreDataUpdate(EDataUpdateType nUpdateType)
	{
		return Memory::CallVFunc<void, 6U>(this, nUpdateType);
	}

	void PostDataUpdate(EDataUpdateType nUpdateType)
	{
		return Memory::CallVFunc<void, 7U>(this, nUpdateType);
	}

	SCHEMA_ADD_FIELD(bool, m_bVisibleinPVS, "CEntityInstance->m_bVisibleinPVS");

	SCHEMA_ADD_FIELD(CEntityIdentity*, m_pEntity, "CEntityInstance->m_pEntity");
};

class VPhysicsCollisionAttribute_t
{
	MEM_PAD( 0x10 );
	uint64 m_nInteractsWith;
};

class CCollisionProperty
{
public:
	std::uint16_t CollisionMask()
	{
		return *reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x38);
	}

	void SetCollisionBounds(Vector& vecMins, Vector& vecMaxs)
	{
		static auto fnSetCollisionBounds = reinterpret_cast<void(__fastcall*)(CCollisionProperty*, Vector*, Vector*)>(Memory::FindPattern(CLIENT_DLL, "48 83 EC ? F2 0F 10 02 8B 42 08"));
		fnSetCollisionBounds(this, &vecMins, &vecMaxs);
	}

public:
	SCHEMA_ADD_FIELD(VPhysicsCollisionAttribute_t, m_collisionAttribute, "CCollisionProperty->m_collisionAttribute");
	SCHEMA_ADD_FIELD(float, m_flCapsuleRadius, "CCollisionProperty->m_flCapsuleRadius");

	SCHEMA_ADD_FIELD(std::uint8_t, m_usSolidFlags, "CCollisionProperty->m_usSolidFlags");
	SCHEMA_ADD_FIELD(std::uint8_t, m_CollisionGroup, "CCollisionProperty->m_CollisionGroup");

	SCHEMA_ADD_FIELD(Vector, m_vecMins, "CCollisionProperty->m_vecMins");
	SCHEMA_ADD_FIELD(Vector, m_vecMaxs, "CCollisionProperty->m_vecMaxs");
	SCHEMA_ADD_FIELD(Vector, m_vCapsuleCenter1, "CCollisionProperty->m_vCapsuleCenter1");
	SCHEMA_ADD_FIELD(Vector, m_vCapsuleCenter2, "CCollisionProperty->m_vCapsuleCenter2");
};

class CTransform
{
public:
	VectorAligned vecPosition;
	QuaternionAligned quatOrientation;
};
static_assert(alignof(CTransform) == 0x10);

struct HitboxData_t {
	Vector m_vMins;
	Vector m_vMaxs;
	Vector m_vCenter;
	float m_flRadius;
	int m_nHitboxIndex;
	int m_nShapeType;
	bool m_bMultipoint;

	bool SegmentIntersectsCapsule(Vector vStart, Vector vPoint) {

		Vector capsuleStart = m_vMins;
		Vector capsuleEnd = m_vMaxs;

		Vector d1 = vPoint - vStart;
		Vector d2 = capsuleEnd - capsuleStart;
		Vector r = vStart - capsuleStart;

		float a = d1.DotProduct(d1);
		float e = d2.DotProduct(d2);
		float f = d2.DotProduct(r);

		float s, t;
		float c = d1.DotProduct(r);
		float b = d1.DotProduct(d2);
		float denom = a * e - b * b;

		if (denom != 0.0f) {
			s = (b * f - c * e) / denom;
		}
		else {
			s = 0.0f;
		}

		s = std::clamp(s, 0.0f, 1.0f);

		t = (b * s + f) / e;
		t = std::clamp(t, 0.0f, 1.0f);

		Vector closestPoint1 = vStart + d1 * s;
		Vector closestPoint2 = capsuleStart + d2 * t;

		float distSqr = (closestPoint1 - closestPoint2).LengthSqr();

		return distSqr <= (m_flRadius * m_flRadius);

	}

};

class CHitBox
{
public:
	const char* m_name;
	const char* m_sSurfaceProperty;
	const char* m_sBoneName;
	Vector m_vecMinBounds;
	Vector m_vecMaxBounds;
	float m_flShapeRadius;
	std::uint32_t m_nBoneNameHash;
	std::int32_t m_nGroupId;
	std::uint8_t m_nShapeType;
	bool m_bTranslationOnly;
	uint8_t __pad003e[0x2];
	std::uint32_t m_CRC;
	Color m_cRenderColor;
	std::uint16_t m_nHitboxIndex;
	MEM_PAD(0x26);

	HitboxData_t ConstructHitboxData(BoneData_t boneData, bool multipoint) {
		HitboxData_t returnData = {};
		returnData.m_nShapeType = this->m_nShapeType;
		returnData.m_flRadius = this->m_flShapeRadius;
		returnData.m_nHitboxIndex = this->m_nHitboxIndex;
		returnData.m_vMins = boneData.vecRotation.RotateVector(this->m_vecMinBounds * boneData.flScale) + boneData.vecPosition;
		returnData.m_vMaxs = boneData.vecRotation.RotateVector(this->m_vecMaxBounds * boneData.flScale) + boneData.vecPosition;
		returnData.m_vCenter = (returnData.m_vMins + returnData.m_vMaxs) / 2.f;
		returnData.m_bMultipoint = multipoint;
		return returnData;
	}
};

class CHitBoxSets
{
public:
	MEM_PAD(0x20);
	std::uint32_t m_nNameHash;
	MEM_PAD(0x4);
	std::int32_t m_nHitboxCount;
	MEM_PAD(0x4);
	CHitBox* m_pHitBox;
	MEM_PAD(0x18);
};

class CRenderMesh
{
public:
	MEM_PAD(0x150);
	CHitBoxSets* m_pHitboxSets;
	std::int32_t	m_nHitboxSets;
};

class CRenderMeshes
{
public:
	CRenderMesh* m_pMeshes;
};

class CModel
{
public:
	CHitBox* GetHitBox(const std::int32_t nIndex);
	const char* GetHitBoxName(const std::int32_t nIndex);
	int GetHitBoxCount();
public:
	MEM_PAD(0x70);
	std::int32_t m_nRendermeshCount;
	MEM_PAD(0x4);
	CRenderMeshes* m_pRenderMeshes;
};

class CCSPlayer_CameraServices {
public:

};

class CModelState
{
public:
	MEM_PAD(0x80);
	BoneData_t* m_pBoneArray;
	MEM_PAD(0x18);
	CStrongHandle<CModel> m_modelHandle;
public:
	SCHEMA_ADD_FIELD(CUtlSymbolLarge, m_ModelName, "CModelState->m_ModelName");
	SCHEMA_ADD_FIELD(CModel*, m_hModel, "CModelState->m_hModel");
	SCHEMA_ADD_FIELD_OFFSET(CModel*, m_hNewModel, "CModelState->m_hModel", 0x10);
};

class CSkeletonInstance;
class CGameSceneNode
{
public:
	CSkeletonInstance* GetSkeletonInstance()
	{
		return Memory::CallVFunc<CSkeletonInstance*, 8U>(this);
	}

	void SetMeshGroupMask(std::uint64_t uMeshGroup)
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnSetMeshGroupMask != nullptr);
		#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnSetMeshGroupMask, this, uMeshGroup);
	}

	bool GetBonePositionAndRotation(const EBones eBoneIndex, Vector& vecBonePositionOut, Vector4D& vecBoneRotationOut);
	Vector GetBonePosition(const std::int32_t eBoneIndex);
public:
	SCHEMA_ADD_FIELD(CTransform, m_nodeToWorld, "CGameSceneNode->m_nodeToWorld");
	SCHEMA_ADD_FIELD(CEntityInstance*, m_pOwner, "CGameSceneNode->m_pOwner");
	SCHEMA_ADD_FIELD(CGameSceneNode*, m_pChild, "CGameSceneNode->m_pChild");
	SCHEMA_ADD_FIELD(CGameSceneNode*, m_pNextSibling, "CGameSceneNode->m_pNextSibling");
	SCHEMA_ADD_FIELD(bool, m_bDormant, "CGameSceneNode->m_bDormant");

	SCHEMA_ADD_FIELD(CNetworkOriginCellCoordQuantizedVector, m_vecOrigin, "CGameSceneNode->m_vecOrigin");
	SCHEMA_ADD_FIELD(Vector, m_vecAbsOrigin, "CGameSceneNode->m_vecAbsOrigin");
	SCHEMA_ADD_FIELD(Vector, m_vRenderOrigin, "CGameSceneNode->m_vRenderOrigin");

	SCHEMA_ADD_FIELD(QAngle, m_angRotation, "CGameSceneNode->m_angRotation");
	SCHEMA_ADD_FIELD(QAngle, m_angAbsRotation, "CGameSceneNode->m_angAbsRotation");

	struct InterpolationInformation_t
	{
		int m_nSrcTick;
		int m_nDstTick;
		float m_flFraction;
	};

	struct InterpolationTickFraction_t
	{
		int m_nTick;
		float m_flFraction;
	};

	bool CalculateInterpolationInfo(InterpolationInformation_t* pClientInterp, InterpolationInformation_t* pServerInterp0, InterpolationInformation_t* pServerInterp1, InterpolationTickFraction_t* pTickFraction)
	{
		using fnCalculateInterpolation = bool(__fastcall*)(CGameSceneNode*, InterpolationInformation_t*, InterpolationInformation_t*, InterpolationInformation_t*, InterpolationTickFraction_t*);
		static fnCalculateInterpolation oCalculateInterpolation = reinterpret_cast<fnCalculateInterpolation>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 8B 45 EF 3B 45 57")), 0x1, 0x0));
		return oCalculateInterpolation(this, pClientInterp, pServerInterp0, pServerInterp1, pTickFraction);
	}
};

class CSkeletonInstance : public CGameSceneNode
{
public:
	void CalculateWorldSpaceBones(std::uint32_t uBoneMask)
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnCalculateWorldSpaceBones != nullptr);
		#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnCalculateWorldSpaceBones, this, uBoneMask);
	}

	void CalculateAnimationState(std::uint32_t nFlags)
	{
		static auto fn = reinterpret_cast<__int64(__fastcall*)(CSkeletonInstance*, int)>(Memory::FindPattern(CLIENT_DLL, "40 55 56 57 41 54 41 55 41 56 41 57 B8 ? ? ? ? E8 ? ? ? ? 48 2B E0 48 8D 6C 24 ? 48 8B F1 48 C7 45 ? ? ? ? ? 48 8D 0D ? ? ? ? 48"));

		fn(this, nFlags);
	}

public:
	MEM_PAD(0x210);
	Matrix4x2_t* m_pBoneCache;

	SCHEMA_ADD_FIELD(CModelState, m_modelState, "CSkeletonInstance->m_modelState");
	SCHEMA_ADD_FIELD(std::uint8_t, m_nHitboxSet, "CSkeletonInstance->m_nHitboxSet");
};

class CBodyComponent;
class C_BaseEntity : public CEntityInstance
{
public:
	[[nodiscard]] bool IsPlayerPawn()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSPlayerPawn");
	}

	[[nodiscard]] bool IsCS2HudModelWeapon()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CS2HudModelWeapon");
	}

	[[nodiscard]] bool IsBasePlayerController()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSPlayerController");
	}

	[[nodiscard]] bool IsWeapon()
	{
		static SchemaClassInfoData_t* pWeaponBaseClass = nullptr;
		if (!pWeaponBaseClass)
			Interfaces::m_pSchemaSystem->FindTypeScopeForModule(X("client.dll"))->FindDeclaredClass(&pWeaponBaseClass, X("C_CSWeaponBase"));

		if (!pWeaponBaseClass)
			return false;

		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (pClassInfo == nullptr)
			return false;

		return (pClassInfo->InheritsFrom(pWeaponBaseClass));
	}

	[[nodiscard]] bool IsViewModel()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo || !pClassInfo->szName)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_CSGOViewModel");
	}

	[[nodiscard]] bool IsViewModelAttachment()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo || !pClassInfo->szName)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_ViewmodelAttachmentModel");
	}

	[[nodiscard]] bool IsKnife()
	{
		SchemaClassInfoData_t* pClassInfo = nullptr;
		GetSchemaClassInfo(&pClassInfo);
		if (!pClassInfo || !pClassInfo->szName)
			return false;

		return FNV1A::Hash(pClassInfo->szName) == FNV1A::HashConst("C_Knife");
	}

	[[nodiscard]] int LookupBone(const char* szBone)
	{
#		ifdef CS_PARANOID
		CS_ASSERT(Functions::fnLookupBone != nullptr);
		#endif

		return Functions::fnLookupBone(this, szBone);
	}

public:
	SCHEMA_ADD_FIELD_OFFSET(CEntitySubClassVDataBase*, m_pVDataBase, "C_BaseEntity->m_nSubclassID", 0x390);
	SCHEMA_ADD_FIELD(CGameSceneNode*, m_pGameSceneNode, "C_BaseEntity->m_pGameSceneNode");
	SCHEMA_ADD_FIELD(CCollisionProperty*, m_pCollision, "C_BaseEntity->m_pCollision");
	SCHEMA_ADD_FIELD(CBodyComponent*, m_CBodyComponent, "C_BaseEntity->m_CBodyComponent");

	SCHEMA_ADD_FIELD(CUtlStringToken, m_nSubclassID, "C_BaseEntity->m_nSubclassID");

	SCHEMA_ADD_FIELD(bool, m_bTakesDamage, "C_BaseEntity->m_bTakesDamage");

	SCHEMA_ADD_FIELD(std::float_t, m_flFriction, "C_BaseEntity->m_flFriction");
	SCHEMA_ADD_FIELD(std::float_t, m_flWaterLevel, "C_BaseEntity->m_flWaterLevel");
	SCHEMA_ADD_FIELD(std::float_t, m_flSimulationTime, "C_BaseEntity->m_flSimulationTime");
	SCHEMA_ADD_FIELD(int, m_nSimulationTick, "C_BaseEntity->m_nSimulationTick");

	SCHEMA_ADD_FIELD(std::float_t, m_flAnimTime, "C_BaseModelEntity->m_flAnimTime");
	SCHEMA_ADD_FIELD(std::float_t, m_flGravityScale, "C_BaseEntity->m_flGravityScale");

	SCHEMA_ADD_FIELD(std::int32_t, m_iEFlags, "C_BaseEntity->m_iEFlags");
	SCHEMA_ADD_FIELD(int, m_iHealth, "C_BaseEntity->m_iHealth");
	SCHEMA_ADD_FIELD(int, m_iMaxHealth, "C_BaseEntity->m_iMaxHealth");

	SCHEMA_ADD_FIELD(std::uint8_t, m_nActualMoveType, "C_BaseEntity->m_nActualMoveType");
	SCHEMA_ADD_FIELD(std::uint8_t, m_lifeState, "C_BaseEntity->m_lifeState");
	SCHEMA_ADD_FIELD(std::uint8_t, m_iTeamNum, "C_BaseEntity->m_iTeamNum");

	SCHEMA_ADD_FIELD(std::uint32_t, m_fFlags, "C_BaseEntity->m_fFlags");

	SCHEMA_ADD_FIELD(GameTick_t, m_nNextThinkTick, "C_BaseEntity->m_nNextThinkTick");

	SCHEMA_ADD_FIELD(Vector, m_vecBaseVelocity, "C_BaseEntity->m_vecBaseVelocity");
	SCHEMA_ADD_FIELD(Vector, m_vecAbsVelocity, "C_BaseEntity->m_vecAbsVelocity");
	SCHEMA_ADD_FIELD(Vector, m_vecVelocity, "C_BaseEntity->m_vecVelocity");

	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hGroundEntity, "C_BaseEntity->m_hGroundEntity");
	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hOwnerEntity, "C_BaseEntity->m_hOwnerEntity");
};

class CBaseAnimGraphController {
public:
	SCHEMA_ADD_FIELD(bool, m_bSequenceFinished, "CBaseAnimGraphController->m_bSequenceFinished");
	SCHEMA_ADD_FIELD(float, m_flSoundSyncTime, "CBaseAnimGraphController->m_flSoundSyncTime");
	SCHEMA_ADD_FIELD(std::uintptr_t, m_nActiveIKChainMask, "CBaseAnimGraphController->m_nActiveIKChainMask");
	SCHEMA_ADD_FIELD(int, m_hSequence, "CBaseAnimGraphController->m_hSequence");
	SCHEMA_ADD_FIELD(int, m_flSeqStartTime, "CBaseAnimGraphController->m_flSeqStartTime");
	SCHEMA_ADD_FIELD(int, m_flSeqFixedCycle, "CBaseAnimGraphController->m_flSeqFixedCycle");
	SCHEMA_ADD_FIELD(int, m_nAnimLoopMode, "CBaseAnimGraphController->m_nAnimLoopMode");
	SCHEMA_ADD_FIELD(float, m_flPlaybackRate, "CBaseAnimGraphController->m_flPlaybackRate");
	SCHEMA_ADD_FIELD(bool, m_bNetworkedAnimationInputsChanged, "CBaseAnimGraphController->m_bNetworkedAnimationInputsChanged");
	SCHEMA_ADD_FIELD(bool, m_bNetworkedSequenceChanged, "CBaseAnimGraphController->m_bNetworkedSequenceChanged");
	SCHEMA_ADD_FIELD(bool, m_bLastUpdateSkipped, "CBaseAnimGraphController->m_bLastUpdateSkipped");

	SCHEMA_ADD_OFFSET(float, m_flCachedSeqCycleRate, 0x14CC);

	void WriteNewSequence(int& sequence) {
		sequence = m_hSequence();
	}
};

class
{
public:
	SCHEMA_ADD_FIELD(CGameSceneNode*, m_pSceneNode, "CGlowProperty->m_pSceneNode");
public:
	CBaseAnimGraphController* GetBaseAnimGraphController()
	{
		return reinterpret_cast<CBaseAnimGraphController*>(std::uintptr_t(this) + 0x460);
	}
};

class CGlowProperty
{
public:
	SCHEMA_ADD_FIELD(bool, m_bFlashing, "CGlowProperty->m_bFlashing");
	SCHEMA_ADD_FIELD(bool, m_bGlowing, "CGlowProperty->m_bGlowing");

	SCHEMA_ADD_FIELD(std::int32_t, m_iGlowType, "CGlowProperty->m_iGlowType");
	SCHEMA_ADD_FIELD(std::int32_t, m_iGlowTeam, "CGlowProperty->m_iGlowTeam");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowRange, "CGlowProperty->m_nGlowRange");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowRangeMin, "CGlowProperty->m_nGlowRangeMin");

	SCHEMA_ADD_FIELD(std::float_t, m_flGlowTime, "CGlowProperty->m_flGlowTime");
	SCHEMA_ADD_FIELD(std::float_t, m_flGlowStartTime, "CGlowProperty->m_flGlowStartTime");

	SCHEMA_ADD_FIELD(Vector, m_fGlowColor, "CGlowProperty->m_fGlowColor");

	SCHEMA_ADD_FIELD(Color, m_glowColorOverride, "CGlowProperty->m_glowColorOverride");

	MEM_PAD(0x18);
	C_CSPlayerPawn* m_pOwner;
};

class C_BaseModelEntity : public C_BaseEntity
{
public:
	void SetModel(const char* szName)
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnSetModel != nullptr);
		#endif

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnSetModel, this, szName);
	}

	void SetAbsOrigin(const Vector& vecOrigin)
	{
		using fnSetAbsOrigin = void(__fastcall*)(C_BaseEntity*, Vector);
		static const auto oSetAbsOrigin = reinterpret_cast<fnSetAbsOrigin>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 08 57 48 83 EC 40 48 8B 01 48 8B FA")));

#ifdef CS_PARANOID
		CS_ASSERT(oSetAbsOrigin != nullptr);
#endif

		return oSetAbsOrigin(this, vecOrigin);
	}
public:
	SCHEMA_ADD_FIELD(Vector, m_vecViewOffset, "C_BaseModelEntity->m_vecViewOffset");

	SCHEMA_ADD_FIELD(GameTime_t, m_flCreateTime, "C_BaseModelEntity->m_flCreateTime");
	SCHEMA_ADD_FIELD(GameTick_t, m_nCreationTick, "C_BaseModelEntity->m_nCreationTick");

	SCHEMA_ADD_FIELD(CGlowProperty, m_Glow, "C_BaseModelEntity->m_Glow");

	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hOldMoveParent, "C_BaseModelEntity->m_hOldMoveParent");
};

struct PhysicsRagdollPose_t
{
	MEM_PAD(0x48);
	CHandle<C_BaseEntity> m_hOwner;
	MEM_PAD(0x20);
	bool m_bDirty;
};

class CBaseAnimGraph : public C_BaseModelEntity
{
public:
	SCHEMA_ADD_FIELD(bool, m_bInitiallyPopulateInterpHistory, "CBaseAnimGraph->m_bInitiallyPopulateInterpHistory");
	SCHEMA_ADD_FIELD(bool, m_bShouldAnimateDuringGameplayPause, "CBaseAnimGraph->m_bShouldAnimateDuringGameplayPause");
	SCHEMA_ADD_FIELD(bool, m_bSuppressAnimEventSounds, "CBaseAnimGraph->m_bSuppressAnimEventSounds");
	SCHEMA_ADD_FIELD(bool, m_bAnimGraphUpdateEnabled, "CBaseAnimGraph->m_bAnimGraphUpdateEnabled");
	SCHEMA_ADD_FIELD(bool, m_bBuiltRagdoll, "CBaseAnimGraph->m_bBuiltRagdoll");
	SCHEMA_ADD_FIELD(bool, m_bClientRagdoll, "CBaseAnimGraph->m_bClientRagdoll");
	SCHEMA_ADD_FIELD(bool, m_bHasAnimatedMaterialAttributes, "CBaseAnimGraph->m_bHasAnimatedMaterialAttributes");

	SCHEMA_ADD_FIELD(std::float_t, m_flMaxSlopeDistance, "CBaseAnimGraph->m_flMaxSlopeDistance");

	SCHEMA_ADD_FIELD(std::int32_t, m_nForceBone, "CBaseAnimGraph->m_nForceBone");

	SCHEMA_ADD_FIELD(Vector, m_vecForce, "CBaseAnimGraph->m_vecForce");
	SCHEMA_ADD_FIELD(Vector, m_vLastSlopeCheckPos, "CBaseAnimGraph->m_vLastSlopeCheckPos");

	SCHEMA_ADD_FIELD(CBaseAnimGraph*, m_pClientsideRagdoll, "CBaseAnimGraph->m_pClientsideRagdoll");
	SCHEMA_ADD_FIELD(PhysicsRagdollPose_t*, m_pRagdollPose, "CBaseAnimGraph->m_pRagdollPose");
};

class C_BaseFlex : public CBaseAnimGraph
{

};

class C_BaseToggle : public C_BaseModelEntity
{
public:
};

class C_BaseTrigger : public C_BaseToggle
{
	SCHEMA_ADD_FIELD(bool, m_bDisabled, "C_BaseTrigger->m_bDisabled");
	SCHEMA_ADD_FIELD(bool, m_bClientSidePredicted, "C_BaseTrigger->m_bClientSidePredicted");
};

class C_PostProcessingVolume : public C_BaseTrigger
{
public:
	SCHEMA_ADD_FIELD(bool, m_bExposureControl, "C_PostProcessingVolume->m_bExposureControl");

	SCHEMA_ADD_FIELD(std::float_t, m_flFadeDuration, "C_PostProcessingVolume->m_flFadeDuration");
	SCHEMA_ADD_FIELD(std::float_t, m_flMinExposure, "C_PostProcessingVolume->m_flMinExposure");
	SCHEMA_ADD_FIELD(std::float_t, m_flMaxExposure, "C_PostProcessingVolume->m_flMaxExposure");

	SCHEMA_ADD_FIELD(std::float_t, m_flExposureFadeSpeedUp, "C_PostProcessingVolume->m_flExposureFadeSpeedUp");
	SCHEMA_ADD_FIELD(std::float_t, m_flExposureFadeSpeedDown, "C_PostProcessingVolume->m_flExposureFadeSpeedDown");
};

class CBaseProp : public CBaseAnimGraph
{
public:
	SCHEMA_ADD_FIELD(bool, m_bModelOverrodeBlockLOS, "CBaseProp->m_bModelOverrodeBlockLOS");
	SCHEMA_ADD_FIELD(bool, m_bConformToCollisionBounds, "CBaseProp->m_bConformToCollisionBounds");

	SCHEMA_ADD_FIELD(std::int32_t, m_iShapeType, "CBaseProp->m_iShapeType");

	SCHEMA_ADD_FIELD(Matrix3x4_t, m_mPreferredCatchTransform, "CBaseProp->m_mPreferredCatchTransform");
};

class C_BreakableProp : public CBaseProp
{
public:
};

class C_DynamicProp : public C_BreakableProp
{
public:
	SCHEMA_ADD_FIELD(bool, m_bUseHitboxesForRenderBox, "C_BreakableProp->m_bUseHitboxesForRenderBox");
	SCHEMA_ADD_FIELD(bool, m_bUseAnimGraph, "C_BreakableProp->m_bUseAnimGraph");
	SCHEMA_ADD_FIELD(bool, m_bAnimateOnServer, "C_BreakableProp->m_bAnimateOnServer");
	SCHEMA_ADD_FIELD(bool, m_bRandomizeCycle, "C_BreakableProp->m_bRandomizeCycle");
	SCHEMA_ADD_FIELD(bool, m_bStartDisabled, "C_BreakableProp->m_bStartDisabled");
	SCHEMA_ADD_FIELD(bool, m_bScriptedMovement, "C_BreakableProp->m_bScriptedMovement");
	SCHEMA_ADD_FIELD(bool, m_bFiredStartEndOutput, "C_BreakableProp->m_bFiredStartEndOutput");
	SCHEMA_ADD_FIELD(bool, m_bForceNpcExclude, "C_BreakableProp->m_bForceNpcExclude");
	SCHEMA_ADD_FIELD(bool, m_bCreateNonSolid, "C_BreakableProp->m_bCreateNonSolid");
	SCHEMA_ADD_FIELD(bool, m_bIsOverrideProp, "C_BreakableProp->m_bIsOverrideProp");

	SCHEMA_ADD_FIELD(std::int32_t, m_iInitialGlowState, "C_BreakableProp->m_iInitialGlowState");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowRange, "C_BreakableProp->m_nGlowRange");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowRangeMin, "C_BreakableProp->m_nGlowRangeMin");
	SCHEMA_ADD_FIELD(std::int32_t, m_nGlowTeam, "C_BreakableProp->m_nGlowTeam");
	SCHEMA_ADD_FIELD(std::int32_t, m_iCachedFrameCount, "C_BreakableProp->m_iCachedFrameCount");

	SCHEMA_ADD_FIELD(Vector, m_vecCachedRenderMins, "C_BreakableProp->m_vecCachedRenderMins");
	SCHEMA_ADD_FIELD(Vector, m_vecCachedRenderMaxs, "C_BreakableProp->m_vecCachedRenderMaxs");

	SCHEMA_ADD_FIELD(Color, m_glowColor, "C_BreakableProp->m_glowColor");
};

class C_Chicken : public C_DynamicProp
{
public:

};

class C_RagdollProp : public CBaseAnimGraph
{
	SCHEMA_ADD_FIELD(std::float_t, m_flBlendWeight, "C_RagdollProp->m_flBlendWeight");
	SCHEMA_ADD_FIELD(std::float_t, m_flBlendWeightCurrent, "C_RagdollProp->m_flBlendWeightCurrent");

	SCHEMA_ADD_FIELD(CUtlVector<std::int32_t>, m_parentPhysicsBoneIndices, "C_RagdollProp->m_parentPhysicsBoneIndices");
	SCHEMA_ADD_FIELD(CUtlVector<std::int32_t>, m_worldSpaceBoneComputationOrder, "C_RagdollProp->m_worldSpaceBoneComputationOrder");

	SCHEMA_ADD_FIELD(C_NetworkUtlVectorBase<Vector>, m_ragPos, "C_RagdollProp->m_ragPos");
	SCHEMA_ADD_FIELD(C_NetworkUtlVectorBase<QAngle>, m_ragAngles, "C_RagdollProp->m_ragAngles");

	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hRagdollSource, "C_RagdollProp->m_hRagdollSource");
};

class C_ClientRagdoll : public CBaseAnimGraph
{
public:

};

class CCSPlayer_ItemServices
{
public:
	SCHEMA_ADD_FIELD(bool, m_bHasDefuser, "CCSPlayer_ItemServices->m_bHasDefuser");
	SCHEMA_ADD_FIELD(bool, m_bHasHelmet, "CCSPlayer_ItemServices->m_bHasHelmet");
};

class CPlayer_ObserverServices
{
public:
	SCHEMA_ADD_FIELD(std::uint8_t, m_iObserverMode, "CPlayer_ObserverServices->m_iObserverMode");

	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hObserverTarget, "CPlayer_ObserverServices->m_hObserverTarget");
};

class CCSObserver_ObserverServices : public CPlayer_ObserverServices
{
public:
};

class C_EconWearable;
class C_BaseCombatCharacter : public C_BaseFlex
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_bloodColor, "C_BaseCombatCharacter->m_bloodColor");
};

class CPlayer_WeaponServices;
class CPlayer_CameraServices
{
public:
	SCHEMA_ADD_FIELD(CBaseHandle, m_hActivePostProcessingVolume, "CPlayer_CameraServices->m_hActivePostProcessingVolume");
	SCHEMA_ADD_FIELD(Vector, m_vecCsViewPunchAngle, "CPlayer_CameraServices->m_vecCsViewPunchAngle");
	SCHEMA_ADD_FIELD( Vector, m_vClientScopeInaccuracy, "CCSPlayer_CameraServices->m_vClientScopeInaccuracy" );

};

class CBasePlayerController;
class CCSPlayer_MovementServices;
class C_CSWeaponBase;
class C_BasePlayerPawn : public C_BaseCombatCharacter
{
public:
	SCHEMA_ADD_FIELD(std::float_t, m_flOldSimulationTime, "C_BasePlayerPawn->m_flOldSimulationTime");

	SCHEMA_ADD_FIELD(CPlayer_WeaponServices*, m_pWeaponServices, "C_BasePlayerPawn->m_pWeaponServices");
	SCHEMA_ADD_FIELD(CCSPlayer_ItemServices*, m_pItemServices, "C_BasePlayerPawn->m_pItemServices");
	SCHEMA_ADD_FIELD(CCSObserver_ObserverServices*, m_pObserverServices, "C_BasePlayerPawn->m_pObserverServices");
	SCHEMA_ADD_FIELD(CPlayer_CameraServices*, m_pCameraServices, "C_BasePlayerPawn->m_pCameraServices");
	SCHEMA_ADD_FIELD(CCSPlayer_MovementServices*, m_pMovementServices, "C_BasePlayerPawn->m_pMovementServices");
	SCHEMA_ADD_FIELD(C_CSWeaponBase*, m_pClippingWeapon, "C_BasePlayerPawn->m_pClippingWeapon");
	SCHEMA_ADD_FIELD( Vector, v_angle, "C_BasePlayerPawn->v_angle" );

	SCHEMA_ADD_FIELD(CHandle<CBasePlayerController>, m_hController, "C_BasePlayerPawn->m_hController");

	[[nodiscard]] Vector GetEyePosition(bool bLocal = false)
	{
		Vector vecEyePosition = Vector(0, 0, 0);

		if (bLocal)
		{
			if (!Globals::m_NetClientInfo.m_pLocalData)
				return vecEyePosition;

			vecEyePosition = Globals::m_NetClientInfo.m_pLocalData->m_vecShootPostion;
		}
		else
		{
			Memory::CallVFunc<void, 170U>(this, &vecEyePosition);
		}

		return vecEyePosition;
	}
};

class CAnimGraphNetworkedVariables;
class CAnimationGraphInstance
{
public:
	MEM_PAD(0x2E0);
	CAnimGraphNetworkedVariables* m_pAnimGraphNetworkedVariables;
};

class C_BaseViewModel : public CBaseAnimGraph
{
public:
	SCHEMA_ADD_FIELD(CHandle<C_BasePlayerWeapon>, m_hWeapon, "C_BaseViewModel->m_hWeapon");
};

enum EMaterialMagicNumber : std::uint32_t
{
	MATERIAL_MAGIC_NUMBER_GLOVE = 0xF143B82A,
};

struct MaterialRecord_t
{
	std::uint32_t m_uUnknown;
	EMaterialMagicNumber m_eIdentifier;
	std::uint32_t m_uHandle;
	MEM_PAD(0x4);
	std::uint32_t m_uTypeIndex;
};

struct MaterialInfo_t
{
	MaterialRecord_t* m_pMaterialRecords;
	std::uint32_t m_nCount;
};

class C_CSGOViewModel : public C_BaseViewModel
{
public:
	MEM_PAD(0xD08);
	CAnimationGraphInstance* m_pAnimationGraphInstance;

	SCHEMA_ADD_PFIELD_OFFSET(MaterialInfo_t, m_MaterialInfo, "C_CSGOViewModel->m_bNeedToQueueHighResComposite", 0x8);
};

class CCSPlayer_ViewModelServices
{
public:
	SCHEMA_ADD_FIELD(CHandle<C_CSGOViewModel>[3], m_hViewModel, "CCSPlayer_ViewModelServices->m_hViewModel");
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
	SCHEMA_ADD_FIELD(CCSPlayer_ViewModelServices*, m_pViewModelServices, "C_CSPlayerPawnBase->m_pViewModelServices");

	SCHEMA_ADD_FIELD(bool, m_bGunGameImmunity, "C_CSPlayerPawn->m_bGunGameImmunity");
	SCHEMA_ADD_FIELD(std::float_t, m_flLowerBodyYawTarget, "C_CSPlayerPawnBase->m_flLowerBodyYawTarget");
	SCHEMA_ADD_FIELD_OFFSET(std::float_t, m_flFlashAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha", -0x8);
	SCHEMA_ADD_FIELD(std::float_t, m_flFlashMaxAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha");
	SCHEMA_ADD_FIELD(std::float_t, m_flFlashDuration, "C_CSPlayerPawnBase->m_flFlashDuration");

	SCHEMA_ADD_FIELD(GameTime_t, m_flLastSpawnTimeIndex, "C_CSPlayerPawnBase->m_flLastSpawnTimeIndex");

	SCHEMA_ADD_FIELD(int, m_nSurvivalTeam, "C_CSPlayerPawnBase->m_nSurvivalTeam");

	SCHEMA_ADD_FIELD(Vector, m_vLastSmokeOverlayColor, "C_CSPlayerPawnBase->m_vLastSmokeOverlayColor");

	SCHEMA_ADD_FIELD(QAngle, m_angEyeAngles, "C_CSPlayerPawnBase->m_angEyeAngles");
public:
	bool IsPlayerOutOfView()
	{
		return Memory::CallVFunc<bool, 296U>(this);
	}
};

class C_CSObserverPawn : public C_CSPlayerPawnBase
{
public:
};

class C_EconItemView;
class CCSPlayer_BulletServices;

class CCSPlayer_HostageServices {
public:
	SCHEMA_ADD_FIELD(CHandle<C_BaseEntity>, m_hCarriedHostage, "CCSPlayer_HostageServices->m_hCarriedHostage");
};
class C_CS2HudModelWeapon : public C_BaseEntity
{
public:
};

class C_CS2HudModelArms : public C_BaseEntity
{
public:
};

class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
	[[nodiscard]] bool IsEnemy(C_CSPlayerPawn* pOther);
	[[nodiscard]] bool IsVisible(C_CSPlayerPawn* pPawn, Vector vecPoint, bool bSmokeCheck = true, float flMaxDensity = 1.0f);
	[[nodiscard]] int GetAssociatedTeam();

	C_CSWeaponBase* GetLocalActiveWeapon();

	[[nodiscard]] std::uint16_t GetCollisionMask();
	[[nodiscard]] std::uint32_t GetOwnerHandle();
	[[nodiscard]] std::uint32_t GetEntityHandle();

	[[nodiscard]] void CreateShockEffect();

	[[nodiscard]] bool IsArmored(int nHitGroup);

	[[nodiscard]] bool HasHeavyArmor()
	{

		return false;
	}

	OFFSET( float, m_flMaxPlayerMovementSpeed, 0x1254 );

	Vector GetWorldSpaceCenter()
	{
		Vector vecResult;
		Memory::CallVFunc<Vector, 77U>(this, &vecResult);
		return vecResult;
	}

	float GetSomeTiming(int a1, int a2) {
		static auto fnSomeTimingFromPawn = reinterpret_cast<float(__fastcall*)(void*, int, int)>((Memory::FindPattern(CLIENT_DLL, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 49 63 D8 48 8B F1")));
		return fnSomeTimingFromPawn(this, a1, a2);
	}

	void SetAbsOrigin(const Vector& vecOrigin)
	{
		using fnSetAbsOrigin = void(__fastcall*)(C_BaseEntity*, Vector);
		static const auto oSetAbsOrigin = reinterpret_cast<fnSetAbsOrigin>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 57 48 83 EC ? ? ? ? 48 8B FA 48 8B D9 FF 90 ? ? ? ? 84 C0 0F 85")));

#ifdef CS_PARANOID
		CS_ASSERT(oSetAbsOrigin != nullptr);
#endif

		return oSetAbsOrigin(this, vecOrigin);
	}

public:
	SCHEMA_ADD_FIELD(bool, m_bNeedToReApplyGloves, "C_CSPlayerPawn->m_bNeedToReApplyGloves");
	SCHEMA_ADD_FIELD(bool, m_bHasFemaleVoice, "C_CSPlayerPawn->m_bHasFemaleVoice");
	SCHEMA_ADD_FIELD(bool, m_bIsScoped, "C_CSPlayerPawn->m_bIsScoped");
	SCHEMA_ADD_FIELD(bool, m_bResumeZoom, "C_CSPlayerPawn->m_bResumeZoom");
	SCHEMA_ADD_FIELD(bool, m_bIsWalking, "C_CSPlayerPawn->m_bIsWalking");
	SCHEMA_ADD_FIELD(bool, m_bIsDefusing, "C_CSPlayerPawn->m_bIsDefusing");
	SCHEMA_ADD_FIELD(bool, m_bIsGrabbingHostage, "C_CSPlayerPawn->m_bIsGrabbingHostage");
	SCHEMA_ADD_FIELD(bool, m_bOnGroundLastTick, "C_CSPlayerPawn->m_bOnGroundLastTick");

	SCHEMA_ADD_FIELD(std::int32_t, m_iShotsFired, "C_CSPlayerPawn->m_iShotsFired");
	SCHEMA_ADD_FIELD(std::int32_t, m_ArmorValue, "C_CSPlayerPawn->m_ArmorValue");

	SCHEMA_ADD_FIELD(GameTime_t, m_flHealthShotBoostExpirationTime, "C_CSPlayerPawn->m_flHealthShotBoostExpirationTime");

	SCHEMA_ADD_FIELD(QAngle, m_aimPunchAngle, "C_CSPlayerPawn->m_aimPunchAngle");

	SCHEMA_ADD_PFIELD(C_EconItemView, m_EconGloves, "C_CSPlayerPawn->m_EconGloves");

	SCHEMA_ADD_FIELD(CCSPlayer_BulletServices*, m_pBulletServices, "C_CSPlayerPawn->m_pBulletServices");
	SCHEMA_ADD_FIELD(CCSPlayer_HostageServices*, m_pHostageServices, "C_CSPlayerPawn->m_pHostageServices");
	SCHEMA_ADD_FIELD(CHandle<C_CS2HudModelArms>, m_hHudModelArms, "C_CSPlayerPawn->m_hHudModelArms");
	std::vector<C_CS2HudModelWeapon*> GetViewModels();
	C_CS2HudModelWeapon* GetViewModel();
	QAngle GetRemovedAimPunch()
	{
		QAngle aimPunch = {};
		static const auto getAimPunchfn = reinterpret_cast<void(__fastcall*)(void*, QAngle*, bool)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("E8 ? ? ? ? 4C 8B C0 48 8D 55 ? 48 8B CB E8 ? ? ? ? 48 8D 0D")), 0x1, 0x0));
		getAimPunchfn(this, &aimPunch, true);
		return aimPunch;
	}

	struct WeaponData {
		int weaponId;
		float weaponTime;
	};

};

class CUserCmdManager
{
public:
	CUserCmd m_arrCommands[150];
	MEM_PAD(0x260);
	int m_iPreviousSequenceNumber;
	MEM_PAD(0x8C);
	int m_iSequenceNumber;

	CUserCmd* GetUserCmd()
	{
		return &m_arrCommands[m_iSequenceNumber % MULTIPLAYER_BACKUP];
	}

	CUserCmd* GetCommandBySequence(int nSequence)
	{
		return &m_arrCommands[nSequence % MULTIPLAYER_BACKUP];
	}

};

class CBasePlayerController : public C_BaseModelEntity
{
public:
	SCHEMA_ADD_FIELD(bool, m_bIsLocalPlayerController, "CBasePlayerController->m_bIsLocalPlayerController");

	SCHEMA_ADD_FIELD(std::uint64_t, m_steamID, "CBasePlayerController->m_steamID");
	SCHEMA_ADD_FIELD(std::uint32_t, m_nFinalPredictedTick, "CBasePlayerController->m_nFinalPredictedTick");

	SCHEMA_ADD_FIELD(std::uint32_t, m_nTickBase, "CBasePlayerController->m_nTickBase");
	SCHEMA_ADD_FIELD(std::uint32_t, m_iDesiredFOV, "CBasePlayerController->m_iDesiredFOV");

	SCHEMA_ADD_FIELD(CHandle<C_BasePlayerPawn>, m_hPawn, "CBasePlayerController->m_hPawn");

	SCHEMA_ADD_FIELD_OFFSET(CUserCmd*, GetCurrentCommand, "CBasePlayerController->m_steamID", -0x8);

	CUserCmd* GetUserCmd(void* local_controller)
	{
		if (!local_controller)
			return nullptr;

		static bool s_bInitFailed = false;
		if (s_bInitFailed)
			return nullptr;

		int player_idx = 0;

		using get_entity_index_t = void* (__fastcall*)(void*, int*);
		static get_entity_index_t get_entity_index = nullptr;
		if (!get_entity_index)
		{
			auto p = Memory::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 8B 8D ? ? ? ? 8D 51");
			if (!p) { s_bInitFailed = true; return nullptr; }
			get_entity_index = reinterpret_cast<get_entity_index_t>(Memory::GetAbsoluteAddress(p, 0x1));
		}
		get_entity_index(local_controller, &player_idx);

		int entity_index = player_idx - 1;
		if (player_idx == -1)
			entity_index = -1;

		static void* (__fastcall* construct_input_data)(void*, int) = nullptr;
		static void* sig_poo = nullptr;
		if (!construct_input_data || !sig_poo)
		{
			auto p1 = Memory::FindPattern(CLIENT_DLL, "85 D2 0F 85 ? ? ? ? 48 8B C4 44 88 40");
			auto p2 = Memory::FindPattern(CLIENT_DLL, "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B CF 48 8B F0");
			if (!p1 || !p2) { s_bInitFailed = true; return nullptr; }
			construct_input_data = reinterpret_cast<void* (__fastcall*)(void*, int)>(Memory::GetAbsoluteAddress(p1, 0x1));
			sig_poo = *reinterpret_cast<void**>(Memory::GetAbsoluteAddress(p2, 0x3));
		}

		void* array_inputs = construct_input_data(sig_poo, entity_index);
		if (!array_inputs) return nullptr;
		int sequence_number = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(array_inputs) + 0x59A8);

		static CUserCmd* (__fastcall* poo)(void*, int) = nullptr;
		if (!poo)
		{
			auto p = Memory::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 48 8B 0D ? ? ? ? 45 33 E4 48 89 45");
			if (!p) { s_bInitFailed = true; return nullptr; }
			poo = reinterpret_cast<CUserCmd* (__fastcall*)(void*, int)>(Memory::GetAbsoluteAddress(p, 0x1));
		}

		return poo(local_controller, sequence_number);
	};

	float GetPlayerInterp() {
		static auto fnGetPlayerInterp = reinterpret_cast<float(__fastcall*)(CBasePlayerController*)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, "40 53 48 83 EC ? 48 8B D9 48 8B 0D ? ? ? ? 48 83 C1")));
		return fnGetPlayerInterp(this);
	}

};

class C_CSWeaponBaseGun;
class CCSPlayerController_InGameMoneyServices;
class CCSPlayerController : public CBasePlayerController
{
public:
	[[nodiscard]] static CCSPlayerController* GetLocalPlayerController();
	[[nodiscard]] C_CSWeaponBase* GetPlayerWeapon(C_CSPlayerPawn* pPlayer);
	[[nodiscard]] bool CanShoot(C_CSWeaponBaseGun* pBaseWeapon);
	[[nodiscard]] bool IsFiring(C_BasePlayerWeapon* pBaseWeapon);
	bool IsThrowingGrenade(C_CSWeaponBase* pBaseWeapon);
public:
	SCHEMA_ADD_FIELD(CCSPlayerController_InGameMoneyServices*, m_pInGameMoneyServices, "CCSPlayerController->m_pInGameMoneyServices")

	SCHEMA_ADD_FIELD(bool, m_bPawnHasDefuser, "CCSPlayerController->m_bPawnHasDefuser");
	SCHEMA_ADD_FIELD(bool, m_bPawnHasHelmet, "CCSPlayerController->m_bPawnHasHelmet");
	SCHEMA_ADD_FIELD(bool, m_bPawnIsAlive, "CCSPlayerController->m_bPawnIsAlive");

	SCHEMA_ADD_FIELD(std::uint32_t, m_iPing, "CCSPlayerController->m_iPing");

	SCHEMA_ADD_FIELD(const char*, m_sSanitizedPlayerName, "CCSPlayerController->m_sSanitizedPlayerName");

	SCHEMA_ADD_FIELD(CHandle<C_CSPlayerPawn>, m_hPlayerPawn, "CCSPlayerController->m_hPlayerPawn");
	SCHEMA_ADD_FIELD(CHandle<C_CSObserverPawn>, m_hObserverPawn, "CCSPlayerController->m_hObserverPawn");

	void PhysicsRunThink() {
		static auto fn = reinterpret_cast<void* (__fastcall*)(void*)>(Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 ? 57 48 81 EC ? ? ? ? ? ? ? 48 8B F9 FF 90")));
		fn(this);
	}

};

class C_PlantedC4 : public CBaseAnimGraph
{
public:
	__forceinline float GetTimer(const float flServerTime)
	{
		return std::clamp(this->m_flC4Blow() - flServerTime, 0.0f, this->m_flTimerLength());
	}

	__forceinline float GetDefuseTimer(const float flServerTime)
	{
		return std::clamp(this->m_flDefuseCountDown() - flServerTime, 0.0f, this->m_flDefuseLength());
	}
public:
	SCHEMA_ADD_FIELD(bool, m_bBombTicking, "C_PlantedC4->m_bBombTicking");
	SCHEMA_ADD_FIELD(bool, m_bCannotBeDefused, "C_PlantedC4->m_bCannotBeDefused");
	SCHEMA_ADD_FIELD(bool, m_bBeingDefused, "C_PlantedC4->m_bBeingDefused");

	SCHEMA_ADD_FIELD(std::int32_t, m_nBombSite, "C_PlantedC4->m_nBombSite");

	SCHEMA_ADD_FIELD(std::float_t, m_flTimerLength, "C_PlantedC4->m_flTimerLength");
	SCHEMA_ADD_FIELD(std::float_t, m_flDefuseLength, "C_PlantedC4->m_flDefuseLength");

	SCHEMA_ADD_FIELD(GameTime_t, m_flC4Blow, "C_PlantedC4->m_flC4Blow");
	SCHEMA_ADD_FIELD(GameTime_t, m_flDefuseCountDown, "C_PlantedC4->m_flDefuseCountDown");

	SCHEMA_ADD_FIELD(CHandle<C_CSPlayerPawn>, m_hBombDefuser, "C_PlantedC4->m_hBombDefuser");
};

class CAttributeManager
{
public:
};

class IEconItemInterface
{
public:
};

enum EStickerAttributeType : int
{
	STICKER_ATTRIBUTE_INDEX,
	STICKER_ATTRIBUTE_WEAR,
	STICKER_ATTRIBUTE_SCALE,
	STICKER_ATTRIBUTE_ROTATION
};

class C_EconItemView : public IEconItemInterface
{
public:
	CEconItem* GetSOCData()
	{
		CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
		if (!pInventory)
			return nullptr;

		return pInventory->GetSOCDataForItem(m_iItemID());
	}

	[[nodiscard]] int GetCustomPaintKitIndex()
	{
		return Memory::CallVFunc<int, 2U>(this);
	}

	[[nodiscard]] int GetCustomPaintKitSeed()
	{
		return Memory::CallVFunc<int, 3U>(this);
	}

	[[nodiscard]] float GetCustomPaintKitWear(float flWearDefault = 0.0f)
	{
		return Memory::CallVFunc<float, 4U>(this, flWearDefault);
	}

	[[nodiscard]] int GetStickerAttributeBySlotIndexFloat(int nSlot, EStickerAttributeType eStickerAttribute, int nDefinitionIndex)
	{
		return Memory::CallVFunc<int, 6U>(this, nSlot, eStickerAttribute, nDefinitionIndex);
	}

	[[nodiscard]] CEconItemDefinition* GetStaticData()
	{
		return Memory::CallVFunc<CEconItemDefinition*, 13U>(this);
	}
public:
	SCHEMA_ADD_FIELD(bool, m_bInitialized, "C_EconItemView->m_bInitialized");
	SCHEMA_ADD_FIELD(bool, m_bDisallowSOC, "C_EconItemView->m_bDisallowSOC");
	SCHEMA_ADD_FIELD(bool, m_bRestoreCustomMaterialAfterPrecache, "C_EconItemView->m_bRestoreCustomMaterialAfterPrecache");

	SCHEMA_ADD_FIELD(char[161], m_szCustomName, "C_EconItemView->m_szCustomName");
	SCHEMA_ADD_FIELD(char[161], m_szCustomNameOverride, "C_EconItemView->m_szCustomNameOverride");

	SCHEMA_ADD_FIELD(std::int32_t, m_iEntityQuality, "C_EconItemView->m_iEntityQuality");
	SCHEMA_ADD_FIELD(std::int32_t, m_iQualityOverride, "C_EconItemView->m_iQualityOverride");

	SCHEMA_ADD_FIELD(std::uint16_t, m_iItemDefinitionIndex, "C_EconItemView->m_iItemDefinitionIndex");

	SCHEMA_ADD_FIELD(std::uint32_t, m_iItemIDHigh, "C_EconItemView->m_iItemIDHigh");
	SCHEMA_ADD_FIELD(std::uint32_t, m_iItemIDLow, "C_EconItemView->m_iItemIDLow");
	SCHEMA_ADD_FIELD(std::uint32_t, m_iAccountID, "C_EconItemView->m_iAccountID");
	SCHEMA_ADD_FIELD(std::uint32_t, m_iInventoryPosition, "C_EconItemView->m_iInventoryPosition");

	SCHEMA_ADD_FIELD(std::uint64_t, m_iItemID, "C_EconItemView->m_iItemID");
};

class C_AttributeContainer : public CAttributeManager
{
public:
	SCHEMA_ADD_PFIELD(C_EconItemView, m_Item, "C_AttributeContainer->m_Item");
};

class C_EconEntity : public C_BaseFlex
{
public:
	[[nodiscard]] std::uint64_t GetOriginalOwnerXuid()
	{
		return (static_cast<std::uint64_t>(m_OriginalOwnerXuidHigh()) << 32) | m_OriginalOwnerXuidLow();
	}
public:
	SCHEMA_ADD_PFIELD(C_AttributeContainer, m_AttributeManager, "C_EconEntity->m_AttributeManager");

	SCHEMA_ADD_FIELD(std::int32_t, m_nFallbackPaintKit, "C_EconEntity->m_nFallbackPaintKit");
	SCHEMA_ADD_FIELD(std::int32_t, m_nFallbackSeed, "C_EconEntity->m_nFallbackSeed");
	SCHEMA_ADD_FIELD(std::int32_t, m_nFallbackStatTrak, "C_EconEntity->m_nFallbackStatTrak");

	SCHEMA_ADD_FIELD(std::uint32_t, m_OriginalOwnerXuidLow, "C_EconEntity->m_OriginalOwnerXuidLow");
	SCHEMA_ADD_FIELD(std::uint32_t, m_OriginalOwnerXuidHigh, "C_EconEntity->m_OriginalOwnerXuidHigh");

	SCHEMA_ADD_FIELD(std::float_t, m_flFallbackWear, "C_EconEntity->m_flFallbackWear");
};

class CBasePlayerWeaponVData : public CEntitySubClassVDataBase
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_iMaxClip1, "CBasePlayerWeaponVData->m_iMaxClip1");
};

class CCSWeaponBaseVData : public CBasePlayerWeaponVData
{
public:
	SCHEMA_ADD_FIELD(EWeaponType, m_WeaponType, "CCSWeaponBaseVData->m_WeaponType");
	SCHEMA_ADD_FIELD(EWeaponSilencerType, m_eSilencerType, "CCSWeaponBaseVData->m_eSilencerType");

	SCHEMA_ADD_FIELD(bool, m_bIsFullAuto, "CCSWeaponBaseVData->m_bIsFullAuto");
	SCHEMA_ADD_FIELD(bool, m_bIsRevolver, "CCSWeaponBaseVData->m_bIsRevolver");
	SCHEMA_ADD_FIELD(float, m_flSpread, "CCSWeaponBaseVData->m_flSpread");

	SCHEMA_ADD_FIELD(const char*, m_szName, "CCSWeaponBaseVData->m_szName");
	SCHEMA_ADD_FIELD(std::int32_t, m_nZoomLevels, "CCSWeaponBaseVData->m_nZoomLevels");
	SCHEMA_ADD_FIELD(std::int32_t, m_nDamage, "CCSWeaponBaseVData->m_nDamage");
	SCHEMA_ADD_FIELD(int32_t, m_nNumBullets, "CCSWeaponBaseVData->m_nNumBullets");

	SCHEMA_ADD_FIELD(std::float_t, m_flRange, "CCSWeaponBaseVData->m_flRange");
	SCHEMA_ADD_FIELD(std::float_t, m_flRangeModifier, "CCSWeaponBaseVData->m_flRangeModifier");
	SCHEMA_ADD_FIELD(std::float_t, m_flHeadshotMultiplier, "CCSWeaponBaseVData->m_flHeadshotMultiplier");
	SCHEMA_ADD_FIELD(std::float_t, m_flArmorRatio, "CCSWeaponBaseVData->m_flArmorRatio");
	SCHEMA_ADD_FIELD(std::float_t, m_flPenetration, "CCSWeaponBaseVData->m_flPenetration");
	SCHEMA_ADD_FIELD(std::float_t, m_flThrowVelocity, "CCSWeaponBaseVData->m_flThrowVelocity");

	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flCycleTime, "CCSWeaponBaseVData->m_flCycleTime");
	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flMaxSpeed, "CCSWeaponBaseVData->m_flMaxSpeed");
	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flInaccuracyStand, "CCSWeaponBaseVData->m_flInaccuracyStand");
	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flInaccuracyCrouch, "CCSWeaponBaseVData->m_flInaccuracyCrouch");
	SCHEMA_ADD_FIELD(CFiringModeFloat, m_flInaccuracyJump, "CCSWeaponBaseVData->m_flInaccuracyJump");
	SCHEMA_ADD_FIELD( CFiringModeFloat, m_flInaccuracyMove, "CCSWeaponBaseVData->m_flInaccuracyMove" );
	SCHEMA_ADD_FIELD( float, m_flInaccuracyReload, "CCSWeaponBaseVData->m_flInaccuracyReload" );

	const bool IsGun() noexcept
	{
		switch (this->m_WeaponType())
		{
			case EWeaponType::WEAPONTYPE_PISTOL:
			case EWeaponType::WEAPONTYPE_SUBMACHINEGUN:
			case EWeaponType::WEAPONTYPE_RIFLE:
			case EWeaponType::WEAPONTYPE_SHOTGUN:
			case EWeaponType::WEAPONTYPE_SNIPER_RIFLE:
			case EWeaponType::WEAPONTYPE_MACHINEGUN:
				return true;
			default:
				return false;
		}
	}

	[[nodiscard]] const bool IsKnifeOrTaser() noexcept
	{
		if (CRT::StringCompare(this->m_szName(), X("weapon_knife")) == 0 || CRT::StringCompare(this->m_szName(), X("weapon_taser")) == 0)
			return true;
		else
			return false;
	}

	[[nodiscard]] const bool IsTaser() noexcept
	{
		if (CRT::StringCompare(this->m_szName(), X("weapon_taser")) == 0)
			return true;
		else
			return false;
	}

	void UpdateWeaponData()
	{
		Memory::CallVFunc<void*, 193U>(this);
	}
};

class C_BasePlayerWeapon : public C_EconEntity
{
public:
	CCSWeaponBaseVData* GetWeaponBaseVData() noexcept
	{
		return this->m_pVDataBase()->As<CCSWeaponBaseVData>();
	}

	SCHEMA_ADD_FIELD(std::int32_t, m_iClip1, "C_BasePlayerWeapon->m_iClip1");
	SCHEMA_ADD_FIELD(std::int32_t, m_pReserveAmmo, "C_BasePlayerWeapon->m_pReserveAmmo");

	SCHEMA_ADD_FIELD(GameTick_t, m_nNextPrimaryAttackTick, "C_BasePlayerWeapon->m_nNextPrimaryAttackTick");
	SCHEMA_ADD_FIELD(GameTick_t, m_nNextSecondaryAttackTick, "C_BasePlayerWeapon->m_nNextSecondaryAttackTick");

	SCHEMA_ADD_FIELD(std::float_t, m_flNextPrimaryAttackTickRatio, "C_BasePlayerWeapon->m_flNextPrimaryAttackTickRatio");
};

class CPlayer_WeaponServices
{
public:
	[[nodiscard]] Vector GetShootPosition()
	{
		Vector vecTemp = Vector(0.0f, 0.0f, 0.0f);
		Memory::CallVFunc<void, 26>(this, &vecTemp);
		return vecTemp;
	}
public:
	SCHEMA_ADD_FIELD(CHandle<C_CSWeaponBase>, m_hActiveWeapon, "CPlayer_WeaponServices->m_hActiveWeapon");
	SCHEMA_ADD_FIELD(C_NetworkUtlVectorBase<CHandle<C_BasePlayerWeapon>>, m_hMyWeapons, "CPlayer_WeaponServices->m_hMyWeapons");
};

class CCSPlayer_WeaponServices : public CPlayer_WeaponServices
{
public:
	SCHEMA_ADD_FIELD(bool, m_bIsLookingAtWeapon, "CCSPlayer_WeaponServices->m_bIsLookingAtWeapon");
	SCHEMA_ADD_FIELD(bool, m_bIsHoldingLookAtWeapon, "CCSPlayer_WeaponServices->m_bIsHoldingLookAtWeapon");

	SCHEMA_ADD_FIELD(GameTime_t, m_flNextAttack, "CCSPlayer_WeaponServices->m_flNextAttack");
};

class CPlayer_MovementServices
{
public:
	void SetPredictionCommand(CUserCmd* pCmd)
	{
		Memory::CallVFunc<void, 39U>(this, pCmd);
	}

	void RunCommand(CUserCmd* pCmd)
	{
		Memory::CallVFunc<void, 27U>(this, pCmd);
	}

	void ResetPredictionCommand()
	{
		Memory::CallVFunc<void, 40U>(this);
	}

	bool UnkVFunc()
	{
		return Memory::CallVFunc<bool, 38U>(this);
	}

	__int64 ProcessMovement(CUserCmd* pCmd)
	{
		static auto fn = reinterpret_cast<__int64(__fastcall*)(CPlayer_MovementServices*, CUserCmd*)>(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, "E8 ? ? ? ? 48 8B 06 48 8B CE FF 90 ? ? ? ? 44 38 63 45"), 1));
		return fn(this, pCmd);
	}

	void ParachuteAccelerate(Vector& vecOutVelo, float& flStamina, float& flFriction, Vector vecMoveImpulses, float flFrametime);
	void RunCommand_Rebuilt(CUserCmd* pCmd);
public:
	SCHEMA_ADD_FIELD(float, m_flMaxspeed, "CPlayer_MovementServices->m_flMaxspeed");
	SCHEMA_ADD_FIELD(Vector2D, m_vecLastMovementImpulses, "CPlayer_MovementServices->m_vecLastMovementImpulses");
};

class CPlayer_MovementServices_Humanoid : public CPlayer_MovementServices
{
public:
	void PostThink()
	{
		Memory::CallVFunc<void, 5U>(this);
	}
	SCHEMA_ADD_FIELD(float, m_flSurfaceFriction, "CPlayer_MovementServices_Humanoid->m_flSurfaceFriction");
	SCHEMA_ADD_FIELD(bool, m_bDucking, "CPlayer_MovementServices_Humanoid->m_bDucking");
	SCHEMA_ADD_FIELD(bool, m_bDucked, "CPlayer_MovementServices_Humanoid->m_bDucked");
};

struct TouchList_t {
	Vector deltavelocity;

	MEM_PAD(0x108);
};

class C_SubtickMove_
{
public:
	float m_flWhen;
	uint64_t m_uButton;

	union
	{
		bool m_bPressed;
		struct
		{
			float m_flAnalogForwardDelta;
			float m_flAnalogLeftDelta;
		} m_AnalogMove;
	};
};

class CMoveData
{
public:
	uint8_t m_nFlags;
	int32_t m_nSomeEntIndex;
	Vector m_vAbsViewAngles;
	Vector m_vViewAngles;
	Vector m_vLastMovementImpulses;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;
	Vector m_vAbsVelocity;
	Vector m_vAngles;
	CUtlVector<C_SubtickMove_> m_SubtickMoves;
	CUtlVector<C_SubtickMove_> m_AttackSubtickMoves;
	bool m_bHasSubtickInputs;
	float m_flLatestSubtickFraction;
	CUtlVector<void*> m_TouchList;
	Vector m_vCollisionNormal;
	Vector m_vGroundNormal;
	Vector m_vAbsOrigin;
	int32_t m_nTickCount;
	int32_t m_nTargetTick;
	float m_flSubtickEndFraction;
	float m_flSubtickStartFraction;
	Vector m_vOutWishVelocity;
	Vector m_vOldAngles;
	float m_flMaxSpeed;
	float m_flClientMaxSpeed;
	float m_flFrictionDecel;
	bool m_bJumpedThisTick;
	bool m_bGameCodeMovedPlayer;
};
class CCSPlayer_MovementServices : public CPlayer_MovementServices_Humanoid
{
public:
	SCHEMA_ADD_OFFSET( C_CSPlayerPawn*, m_pPawn, 0x30 );

	CMoveData* GetMoveData()
	{
		return Memory::CallVFunc<CMoveData*, 29>(this);
	}

	void ForceButtonsDown( __int64 a2 )
	{
		using fnForceButtonsDown = void* ( __fastcall* )( void*, __int64 );
		static fnForceButtonsDown ForceButtonsDown_ = ( fnForceButtonsDown )( Memory::FindPattern( CLIENT_DLL, "40 53 56 41 56 48 81 EC ? ? ? ? 4C 8B 41" ) );

		ForceButtonsDown_( this, a2 );
	}

	void SetupMovementMoves( CMoveData* pMoveData )
	{
		using fnSetupMovementMoves = void* ( __fastcall* )( void*, CMoveData* );
		static fnSetupMovementMoves SetupMovementMoves_ = ( fnSetupMovementMoves )( Memory::FindPattern( CLIENT_DLL, "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 83 EC ? 49 8B F8 4C 8B F2" ) );

		SetupMovementMoves_( this, pMoveData );
	}

	void SetupMove( CUserCmd* pCmd, CMoveData* pMoveData );
	void FinishMove( CUserCmd* pCmd, CMoveData* pMoveData )
	{
		Memory::CallVFunc<void, 33>( this, pCmd, pMoveData );
	}
	bool IsNoclipDuringPauseDisallowed()
	{
		return Memory::CallVFunc<bool, 38>(this);
	}

	void GetBBoxBounds( BBox_t* bounds )
	{

		using fnGetBBox = BBox_t*( __fastcall* )( void );
		static fnGetBBox GetBBox = (fnGetBBox)( Memory::FindPattern( CLIENT_DLL, "48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 48 FF A0 ? ? ? ? 48 8D 05" ) );

		char* bbox = { };

		if ( this->m_bDucked( ) )
		{
			bbox = ((char*)GetBBox( ) + 36);
		}
		else
		{
			bbox = (char*)GetBBox( ) + 12;
		}

		bounds = (BBox_t*)bbox;
	}

	float GetGroundPosition( CMoveData* mv );

	void GetBBoxBounds2( BBox_t* bounds )
	{
		using fnGetBBoxFromMovementServices = BBox_t*( __fastcall* )( void* );
		static fnGetBBoxFromMovementServices GetBBoxFromMovementServices = (fnGetBBoxFromMovementServices)( Memory::FindPattern( CLIENT_DLL, "40 53 56 48 83 EC ? 48 8B 59" ) );

		bounds = GetBBoxFromMovementServices( this );
	}
public:
	SCHEMA_ADD_FIELD(std::float_t, m_flDuckAmount, "CCSPlayer_MovementServices->m_flDuckAmount");
	SCHEMA_ADD_FIELD(float, m_flStamina, "CCSPlayer_MovementServices->m_flStamina");
	SCHEMA_ADD_FIELD(bool, m_bOldJumpPressed, "CCSPlayer_MovementServices->m_bOldJumpPressed");
	SCHEMA_ADD_FIELD(float&, m_flForwardMove, ("CPlayer_MovementServices->m_flForwardMove"));
	SCHEMA_ADD_FIELD(float&, m_flLeftMove, ("CPlayer_MovementServices->m_flLeftMove"));
	SCHEMA_ADD_FIELD(float&, m_flUpMove, ("CPlayer_MovementServices->m_flUpMove"));

	SCHEMA_ADD_FIELD(float, m_flOffsetTickCompleteTime, "CCSPlayer_MovementServices->m_flOffsetTickCompleteTime");
	SCHEMA_ADD_FIELD(float, m_flOffsetTickStashedSpeed, "CCSPlayer_MovementServices->m_flOffsetTickStashedSpeed");
	SCHEMA_ADD_FIELD( int, m_nGameCodeHasMovedPlayerAfterCommand, "CCSPlayer_MovementServices->m_nGameCodeHasMovedPlayerAfterCommand" );

};

struct BulletInfo_t
{

	BulletInfo_t()
	{
		this->m_vecPosition = Vector(0, 0, 0);
		this->m_flTimeStamp = 0.0f;
		this->m_flExpireTime = -1.0f;
	}

	BulletInfo_t(Vector vecPosition, float flTimeStamp, float flExpireTime)
	{
		this->m_vecPosition = vecPosition;
		this->m_flTimeStamp = flTimeStamp;
		this->m_flExpireTime = flExpireTime;
	}

	Vector	m_vecPosition = Vector(0, 0, 0);
	float	m_flTimeStamp = 0.0f;
	float	m_flExpireTime = 0.0f;
};

class CCSPlayer_BulletServices
{
public:
	[[nodiscard]] CUtlVector<BulletInfo_t>& GetBulletData()
	{
		return *reinterpret_cast<CUtlVector<BulletInfo_t>*>(reinterpret_cast<std::uintptr_t>(this) + 0x48);
	}
};

class C_CSWeaponBase : public C_BasePlayerWeapon
{
public:
	void AddStattrakEntity()
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnAddStatTrackEntity != nullptr);
		#endif

		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnAddStatTrackEntity, this, this->m_AttributeManager()->m_Item());
	}

	void AddNametagEntity()
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnAddNametagEntity != nullptr);
		#endif

		return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnAddNametagEntity, this, this->m_AttributeManager()->m_Item());
	}

	[[nodiscard]] float GetSpread()
	{
		return Memory::CallVFunc<float, 364>(this);
	}

	void* Shoot()
	{
		return Memory::CallVFunc<void*, 392>(this);
	}

	struct CSBaseGunFireData {
		char pad00[0x14];
		Vector m_vecShootPos;
		char pad01[52];
	};

	CSBaseGunFireData GetWeaponAttackTime() {

		static auto fn = reinterpret_cast<CSBaseGunFireData * (__fastcall*)(C_CSWeaponBase*, CSBaseGunFireData*, int)>(Memory::FindPattern(CLIENT_DLL, "48 8B C4 55 53 56 57 41 55 41 56 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 A8 33"));
		CSBaseGunFireData data = {};
		fn(this, &data, 0);
		return data;

	}

	struct WeaponInaccuracyData_t {
		float flTotalInaccuracy;
		float flMoveInaccuracy;
		float flAirInaccuracy;
	};

	[[nodiscard]] WeaponInaccuracyData_t GetInaccuracy()
	{
		WeaponInaccuracyData_t ret{};
		ret.flTotalInaccuracy = Memory::CallVFunc<float, 411>(this, &ret.flMoveInaccuracy, &ret.flAirInaccuracy);
		return ret;
	}

	float GetWeaponInAccuracyRecoveryTime()
	{
		using fnGetWeaponInAccuracyRecoveryTime = float(__fastcall*)(C_CSWeaponBase*);
		static fnGetWeaponInAccuracyRecoveryTime GetWeaponInAccuracyRecoveryTime = (fnGetWeaponInAccuracyRecoveryTime)(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, "E8 ? ? ? ? F3 0F 10 B7 ? ? ? ? F3 0F 5E F8"), 1));
		return GetWeaponInAccuracyRecoveryTime(this);
	}

	void UpdateTurningInAccuracy()
	{
		using fnUpdateTurningInAccuracy = void(__fastcall*)(C_CSWeaponBase*);
		static fnUpdateTurningInAccuracy UpdateTurningInAccuracy = (fnUpdateTurningInAccuracy)(Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, "E8 ? ? ? ? F3 0F 10 87 ? ? ? ? 44 0F 2F C8"), 1));
		UpdateTurningInAccuracy(this);
	}

	float GetMaxSpeed()
	{
		return Memory::CallVFunc<float, 340>(this);
	}

	float GetCycleTime(int nWeaponMode)
	{
		return Memory::CallVFunc<float, 341>(this, nWeaponMode);
	}

	float GetDuckInAccuracy(int nWeaponMode)
	{
		return Memory::CallVFunc<float, 343>(this, nWeaponMode);
	}

	float GetStandInAccuracy(int nWeaponMode)
	{
		return Memory::CallVFunc<float, 344>(this, nWeaponMode);
	}

	float GetJumpInAccuracy(int nWeaponMode)
	{
		return Memory::CallVFunc<float, 345>(this, nWeaponMode);
	}

	float GetLadderInAccuracy(int nWeaponMode)
	{
		return Memory::CallVFunc<float, 347>(this, nWeaponMode);
	}

	void UpdateAccuracyPenalty()
	{
		using fn_UpdateAccuracyPenalty_t = void(__fastcall*)(void*);
		static fn_UpdateAccuracyPenalty_t fn = reinterpret_cast<fn_UpdateAccuracyPenalty_t>(Memory::FindPattern(CLIENT_DLL, X("40 57 41 56 48 83 EC ? 48 8B F9")));

		fn(this);
	}
	void UpdateAccuracyPenalty_Rebuilt( );

	void UpdateSubClass()
	{
#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnUpdateSubClass != nullptr);
#endif

		return Functions::fnUpdateSubClass(this);
	}

public:
	SCHEMA_ADD_FIELD(bool, m_bInReload, "C_CSWeaponBase->m_bInReload");
	SCHEMA_ADD_FIELD(bool, m_bBurstMode, "C_CSWeaponBase->m_bBurstMode");
	SCHEMA_ADD_FIELD(bool, m_bUIWeapon, "C_CSWeaponBase->m_bUIWeapon");
	SCHEMA_ADD_FIELD(int, m_weaponMode, "C_CSWeaponBase->m_weaponMode");

	SCHEMA_ADD_FIELD(std::int32_t, m_iOriginalTeamNumber, "C_CSWeaponBase->m_iOriginalTeamNumber");
	SCHEMA_ADD_FIELD(float, m_flRecoilIndex, "C_CSWeaponBase->m_flRecoilIndex");
	SCHEMA_ADD_FIELD(float, m_fAccuracyPenalty, "C_CSWeaponBase->m_fAccuracyPenalty");
	SCHEMA_ADD_FIELD(float, m_flLastAccuracyUpdateTime, "C_CSWeaponBase->m_flLastAccuracyUpdateTime");

	SCHEMA_ADD_FIELD(std::float_t, m_flPostponeFireReadyFrac, "C_CSWeaponBase->m_flPostponeFireReadyFrac");
	SCHEMA_ADD_FIELD(std::float_t, m_flWatTickOffset, "C_CSWeaponBase->m_flWatTickOffset");

	SCHEMA_ADD_FIELD(GameTick_t, m_nPostponeFireReadyTicks, "C_CSWeaponBase->m_nPostponeFireReadyTicks");
	SCHEMA_ADD_FIELD(GameTime_t, m_fLastShotTime, "C_CSWeaponBase->m_fLastShotTime");
	SCHEMA_ADD_FIELD(std::int32_t, m_zoomLevel, "C_CSWeaponBaseGun->m_zoomLevel");

	SCHEMA_ADD_PFIELD_OFFSET(void, m_hStattrakAttachment, "C_CSWeaponBase->m_iNumEmptyAttacks", 0x4);
	SCHEMA_ADD_PFIELD_OFFSET(void, m_hNametagAttachment, "C_CSWeaponBase->m_iNumEmptyAttacks", 0x14);
};

class C_CSWeaponBaseGun : public C_CSWeaponBase
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_zoomLevel, "C_CSWeaponBaseGun->m_zoomLevel");
	SCHEMA_ADD_FIELD(std::int32_t, m_iBurstShotsRemaining, "C_CSWeaponBaseGun->m_iBurstShotsRemaining");

	bool CanPrimaryAttack(const int nWeaponType, const float flServerTime);
};

class C_C4 : public C_CSWeaponBase
{
public:
	SCHEMA_ADD_FIELD(char[32], m_szScreenText, "C_C4->m_szScreenText");
	SCHEMA_ADD_FIELD(bool, m_bBombPlanted, "C_C4->m_bBombPlanted");

};

class C_BaseCSGrenade : public C_CSWeaponBase
{
public:
	SCHEMA_ADD_FIELD(bool, m_bIsHeldByPlayer, "C_BaseCSGrenade->m_bIsHeldByPlayer");
	SCHEMA_ADD_FIELD(bool, m_bPinPulled, "C_BaseCSGrenade->m_bPinPulled");
	SCHEMA_ADD_FIELD(bool, m_bJumpThrow, "C_BaseCSGrenade->m_bJumpThrow");
	SCHEMA_ADD_FIELD(bool, m_bJustPulledPin, "C_BaseCSGrenade->m_bJustPulledPin");

	SCHEMA_ADD_FIELD(GameTime_t, m_fThrowTime, "C_BaseCSGrenade->m_fThrowTime");

	SCHEMA_ADD_FIELD(std::float_t, m_flThrowStrength, "C_BaseCSGrenade->m_flThrowStrength");
	SCHEMA_ADD_FIELD(std::float_t, m_flThrowStrengthApproach, "C_BaseCSGrenade->m_flThrowStrengthApproach");
};

class C_BaseGrenade : public C_BaseFlex
{
public:
	SCHEMA_ADD_FIELD(CHandle< C_CSPlayerPawn >, m_hThrower, "C_BaseGrenade->m_hThrower");
};

class C_BaseCSGrenadeProjectile : public C_BaseGrenade
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_nBounces, "C_BaseCSGrenadeProjectile->m_nBounces");
	SCHEMA_ADD_FIELD(std::int32_t, m_nExplodeEffectTickBegin, "C_BaseCSGrenadeProjectile->m_nExplodeEffectTickBegin");

	SCHEMA_ADD_FIELD(GameTime_t, m_flSpawnTime, "C_BaseCSGrenadeProjectile->m_flSpawnTime");
	SCHEMA_ADD_FIELD(bool, m_bCanCreateGrenadeTrail, "C_BaseCSGrenadeProjectile->m_bCanCreateGrenadeTrail");

	SCHEMA_ADD_FIELD(Vector, m_vInitialPosition, "C_BaseCSGrenadeProjectile->m_vInitialPosition");
	SCHEMA_ADD_FIELD(Vector, m_vInitialVelocity, "C_BaseCSGrenadeProjectile->m_vInitialVelocity");
	SCHEMA_ADD_FIELD(Vector, m_vecExplodeEffectOrigin, "C_BaseCSGrenadeProjectile->m_vecExplodeEffectOrigin");

	SCHEMA_ADD_FIELD(CUtlVector<Vector>, m_arrTrajectoryTrailPoints, "C_BaseCSGrenadeProjectile->m_arrTrajectoryTrailPoints");
	SCHEMA_ADD_FIELD(CUtlVector<std::float_t>, m_arrTrajectoryTrailPointCreationTimes, "C_BaseCSGrenadeProjectile->m_arrTrajectoryTrailPointCreationTimes");
};

class C_SmokeGrenadeProjectile : public C_BaseCSGrenadeProjectile
{
public:
	SCHEMA_ADD_FIELD(bool, m_bDidSmokeEffect, "C_SmokeGrenadeProjectile->m_bDidSmokeEffect");
	SCHEMA_ADD_FIELD(bool, m_bSmokeEffectSpawned, "C_SmokeGrenadeProjectile->m_bSmokeEffectSpawned");

	SCHEMA_ADD_FIELD(std::int32_t, m_nSmokeEffectTickBegin, "C_SmokeGrenadeProjectile->m_nSmokeEffectTickBegin");

	SCHEMA_ADD_FIELD(Vector, m_vSmokeColor, "C_SmokeGrenadeProjectile->m_vSmokeColor");
	SCHEMA_ADD_FIELD(Vector, m_vSmokeDetonationPos, "C_SmokeGrenadeProjectile->m_vSmokeDetonationPos");

	[[nodiscard]] inline constexpr std::float_t GetMaxTime()
	{
		return 18.f;
	}
};

class C_Inferno : public C_BaseModelEntity
{
public:
	SCHEMA_ADD_FIELD(bool[64], m_bFireIsBurning, "C_Inferno->m_bFireIsBurning");

	SCHEMA_ADD_FIELD(std::int32_t, m_fireCount, "C_Inferno->m_fireCount");
	SCHEMA_ADD_FIELD(std::int32_t, m_nFireEffectTickBegin, "C_Inferno->m_nFireEffectTickBegin");

	SCHEMA_ADD_FIELD(std::float_t, m_nFireLifetime, "C_Inferno->m_nFireLifetime");

	SCHEMA_ADD_FIELD(Vector[64], m_firePositions, "C_Inferno->m_firePositions");
	SCHEMA_ADD_FIELD(Vector[64], m_fireParentPositions, "C_Inferno->m_fireParentPositions");
	SCHEMA_ADD_FIELD(Vector, m_minBounds, "C_Inferno->m_minBounds");
	SCHEMA_ADD_FIELD(Vector, m_maxBounds, "C_Inferno->m_maxBounds");

	[[nodiscard]] inline std::float_t GetMaxTime()
	{
		return Convar::inferno_flame_lifetime->GetFloat();
	}

};

class CCSPlayerController_InGameMoneyServices
{
public:
	SCHEMA_ADD_FIELD(std::int32_t, m_iAccount, "CCSPlayerController_InGameMoneyServices->m_iAccount");
	SCHEMA_ADD_FIELD(std::int32_t, m_iCashSpentThisRound, "CCSPlayerController_InGameMoneyServices->m_iCashSpentThisRound");
	SCHEMA_ADD_FIELD(std::int32_t, m_iStartAccount, "CCSPlayerController_InGameMoneyServices->m_iStartAccount");
	SCHEMA_ADD_FIELD(std::int32_t, m_iTotalCashSpent, "CCSPlayerController_InGameMoneyServices->m_iTotalCashSpent");
	SCHEMA_ADD_FIELD(std::int32_t, m_nPreviousAccount, "CCSPlayerController_InGameMoneyServices->m_nPreviousAccount");
};

class C_EnvSky : public C_BaseModelEntity
{
public:
	SCHEMA_ADD_FIELD(bool, m_bStartDisabled, "C_EnvSky->m_bStartDisabled");
	SCHEMA_ADD_FIELD(bool, m_bEnabled, "C_EnvSky->m_bEnabled");

	SCHEMA_ADD_FIELD(std::float_t, m_flBrightnessScale, "C_EnvSky->m_flBrightnessScale");
	SCHEMA_ADD_FIELD(std::float_t, m_flFogMinStart, "C_EnvSky->m_flFogMinStart");
	SCHEMA_ADD_FIELD(std::float_t, m_flFogMinEnd, "C_EnvSky->m_flFogMinEnd");
	SCHEMA_ADD_FIELD(std::float_t, m_flFogMaxStart, "C_EnvSky->m_flFogMaxStart");
	SCHEMA_ADD_FIELD(std::float_t, m_flFogMaxEnd, "C_EnvSky->m_flFogMaxEnd");

	SCHEMA_ADD_FIELD(std::int32_t, m_nFogType, "C_EnvSky->m_nFogType");

	SCHEMA_ADD_FIELD(Color, m_vTintColor, "C_EnvSky->m_vTintColor");
	SCHEMA_ADD_FIELD(Color, m_vTintColorLightingOnly, "C_EnvSky->m_vTintColorLightingOnly");

	SCHEMA_ADD_FIELD(CStrongHandle<CMaterial2>, m_hSkyMaterial, "C_EnvSky->m_hSkyMaterial");
	SCHEMA_ADD_FIELD(CStrongHandle<CMaterial2>, m_hSkyMaterialLightingOnly, "C_EnvSky->m_hSkyMaterialLightingOnly");
};

class C_GradientFog : public C_BaseEntity
{
public:
	SCHEMA_ADD_FIELD(float, m_flFogMaxOpacity, "C_GradientFog->m_flFogMaxOpacity");
	SCHEMA_ADD_FIELD(Color, m_fogColor, "C_GradientFog->m_fogColor");
	SCHEMA_ADD_FIELD(bool, m_bIsEnabled, "C_GradientFog->m_bIsEnabled");
};

class C_EnvCubemapFog : public C_BaseEntity
{
public:
	SCHEMA_ADD_FIELD(float, m_flFogMaxOpacity, "C_EnvCubemapFog->m_flFogMaxOpacity");
};

enum EExplosionType : uint8_t
{
	EXPLOSION_TYPE_DEFAULT,
	EXPLOSION_TYPE_GRENADE,
	EXPLOSION_TYPE_MOLOTOV,
	EXPLOSION_TYPE_FIREWORKS,
	EXPLOSION_TYPE_GASCAN,
	EXPLOSION_TYPE_GASCYLINDER,
	EXPLOSION_TYPE_EXPLOSIVEBARREL,
	EXPLOSION_TYPE_ELECTRICAL,
	EXPLOSION_TYPE_EMP,
	EXPLOSION_TYPE_SHRAPNEL,
	EXPLOSION_TYPE_SMOKEGRENADE,
	EXPLOSION_TYPE_FLASHBANG,
	EXPLOSION_TYPE_TRIPMINE,
	EXPLOSION_TYPE_ICE,
	EXPLOSION_TYPE_NONE,
	EXPLOSION_TYPE_CUSTOM,
	EXPLOSION_TYPE_COUNT
};

class CEffectData
{
public:
	void* m_pVTable;
	Vector m_vecOrigin;
	Vector m_vecStart;
	Vector m_vecNormal;
	QAngle m_angAngles;
	uint32_t m_hEntity;
	uint32_t m_hOtherEntity;
	float m_flScale;
	float m_flMagnitude;
	float m_flRadius;
	int m_nSurfaceProp;
	void* pEffectIndex;
	uint32_t m_nEffectIndex;
	uint8_t m_nPenetrate;
	MEM_PAD(0x1);
	uint16_t m_nMaterial;
	uint16_t m_nHitBox;
	uint8_t m_nColor;
	uint8_t m_fFlags;
	uint8_t m_nAttachmentIndex;
	MEM_PAD(0x3);
	int m_nAttachmentName;
	uint16_t m_iEffectName;
	uint8_t m_nExplosionType;
	MEM_PAD(0x9);
public:
	void CreateEffect(const char* szEffectName)
	{

		static std::uint8_t* pVTable = Memory::GetAbsoluteAddress(Memory::FindPattern(CLIENT_DLL, X("4C 8B DC 53 48 81 EC ? ? ? ? F2 0F 10 05")));
		if (!pVTable)
			return;

		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnDispatchEffect != nullptr);
		#endif

		this->m_pVTable = pVTable;

		ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnDispatchEffect, szEffectName, this);
	};
};