#include "../Precompiled.h"

bool CGameSceneNode::GetBonePositionAndRotation( const EBones eBoneIndex, Vector& vecBonePositionOut, Vector4D& vecBoneRotationOut )
{

	if (static_cast<int>(eBoneIndex) < 0 || static_cast<int>(eBoneIndex) >= 128)
		return false;

	CSkeletonInstance* pSkeleton = GetSkeletonInstance( );
	if (pSkeleton == nullptr)
		return false;

	Matrix4x2_t* pBoneArray = pSkeleton->m_pBoneCache;
	if (pBoneArray == nullptr)
		return false;

	__try {
		vecBonePositionOut = pBoneArray->GetOrigin(eBoneIndex);
		vecBoneRotationOut = pBoneArray->GetRotation(eBoneIndex);

		if (!vecBonePositionOut.IsValid())
			return false;

		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		vecBonePositionOut = Vector(0, 0, 0);
		vecBoneRotationOut = Vector4D(0, 0, 0, 0);
		return false;
	}
}

Vector CGameSceneNode::GetBonePosition( const std::int32_t eBoneIndex )
{

	if (eBoneIndex < 0 || eBoneIndex >= 128)
		return Vector( 0, 0, 0 );

	CSkeletonInstance* pSkeleton = GetSkeletonInstance( );
	if (pSkeleton == nullptr)
		return Vector( 0, 0, 0 );

	Matrix4x2_t* pBoneArray = pSkeleton->m_pBoneCache;
	if (pBoneArray == nullptr)
		return Vector( 0, 0, 0 );

	__try {
		Vector result = pBoneArray->GetOrigin(eBoneIndex);

		if (!result.IsValid() || (result.x == 0.0f && result.y == 0.0f && result.z == 0.0f))
			return Vector( 0, 0, 0 );
		return result;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return Vector( 0, 0, 0 );
	}
}

CHitBox* CModel::GetHitBox(const std::int32_t nIndex)
{
	if (nIndex < 0 || nIndex >= 32)
		return nullptr;

	if (m_nRendermeshCount <= 0 || !m_pRenderMeshes)
		return nullptr;

	__try {
		CRenderMesh* pMeshes = m_pRenderMeshes->m_pMeshes;
		if (!pMeshes)
			return nullptr;

		CRenderMesh mesh = pMeshes[0];
		CHitBoxSets* pHitBoxSets = mesh.m_pHitboxSets;
		if (!pHitBoxSets)
			return nullptr;

		if (pHitBoxSets[0].m_nHitboxCount <= 0 || nIndex >= pHitBoxSets[0].m_nHitboxCount)
			return nullptr;

		CHitBox* pHitBox = pHitBoxSets[0].m_pHitBox;
		if (!pHitBox)
			return nullptr;

		return &pHitBox[nIndex];
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return nullptr;
	}
}

const char* CModel::GetHitBoxName(const std::int32_t nIndex)
{
	__try {
		CHitBox* pHitbox = GetHitBox(nIndex);
		if (!pHitbox)
			return nullptr;

		return pHitbox->m_name;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return nullptr;
	}
}

int CModel::GetHitBoxCount()
{
	if (m_nRendermeshCount <= 0 || !m_pRenderMeshes)
		return -1;

	__try {
		CRenderMesh* pMeshes = m_pRenderMeshes->m_pMeshes;
		if (!pMeshes)
			return -1;

		CRenderMesh mesh = pMeshes[0];
		CHitBoxSets* pHitBoxSets = mesh.m_pHitboxSets;
		if (!pHitBoxSets)
			return -1;

		int count = pHitBoxSets[0].m_nHitboxCount;

		if (count < 0 || count > 32)
			return -1;

		return count;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return -1;
	}
}

CCSPlayerController* CCSPlayerController::GetLocalPlayerController( )
{
	const int nIndex = Interfaces::m_pEngine->GetLocalPlayer( );
	return Interfaces::m_pGameResourceService->pGameEntitySystem->Get<CCSPlayerController>( nIndex );
}

C_CSWeaponBase* CCSPlayerController::GetPlayerWeapon(C_CSPlayerPawn* pPlayer)
{
	if (!pPlayer || !pPlayer->m_pWeaponServices())
		return nullptr;

	CBaseHandle hActiveWeapon = pPlayer->m_pWeaponServices()->m_hActiveWeapon();

	if (!hActiveWeapon.IsValid())
		return nullptr;

	C_CSWeaponBase* pWeapon = Interfaces::m_pGameResourceService->pGameEntitySystem->Get<C_CSWeaponBase>(hActiveWeapon);

	return pWeapon;
}

bool CCSPlayerController::CanShoot(C_CSWeaponBaseGun* pBaseWeapon)
{
	const float flServerTime = TICKS_TO_TIME(this->m_nTickBase());
	C_CSPlayerPawn* pPawn = this->m_hPlayerPawn( ).Get();
	if (!pPawn)
		return false;

	CCSPlayer_WeaponServices* pPlayerWeaponServices = static_cast<CCSPlayer_WeaponServices*>(pPawn->m_pWeaponServices());
	if (!pPlayerWeaponServices)
		return false;

	if (pBaseWeapon->m_iClip1() <= 0)
		return false;

	if (pPlayerWeaponServices->m_flNextAttack( ) > flServerTime)
		return false;

	const short nDefinitionIndex = pBaseWeapon->m_AttributeManager()->m_Item()->m_iItemDefinitionIndex( );

	if ((nDefinitionIndex == WEAPON_FAMAS || nDefinitionIndex == WEAPON_GLOCK_18) &&

		pBaseWeapon->m_bBurstMode() && pBaseWeapon->m_iBurstShotsRemaining() > 0)
		return true;

	if (pBaseWeapon->m_nNextPrimaryAttackTick() > static_cast<int>(this->m_nTickBase()))
		return false;

	if (nDefinitionIndex == WEAPON_R8_REVOLVER && pBaseWeapon->m_flPostponeFireReadyFrac() > flServerTime)
		return false;

	return true;
}

bool CCSPlayerController::IsFiring(C_BasePlayerWeapon* pBaseWeapon)
{
	if (!pBaseWeapon || !Globals::m_pCmd)
		return false;

	const float flServerTime = TICKS_TO_TIME(this->m_nTickBase());
	const short nDefinitionIndex = pBaseWeapon->m_AttributeManager()->m_Item()->m_iItemDefinitionIndex();
	CCSWeaponBaseVData* pWeaponBaseVData = pBaseWeapon->GetWeaponBaseVData();
	if (!pWeaponBaseVData)
		return false;

	if (C_BaseCSGrenade* pGrenade = static_cast<C_BaseCSGrenade*>(pBaseWeapon); pGrenade != nullptr && pWeaponBaseVData->m_WeaponType( ) == WEAPONTYPE_GRENADE)
		return !pGrenade->m_bPinPulled() && pGrenade->m_fThrowTime() > 0.f && pGrenade->m_fThrowTime() < flServerTime;
	else if (pWeaponBaseVData->m_WeaponType( ) == WEAPONTYPE_KNIFE)
		return (Globals::m_pCmd->m_nButtons.m_nValue & (IN_ATTACK) || Globals::m_pCmd->m_nButtons.m_nValue & (IN_ATTACK2)) && CanShoot(static_cast<C_CSWeaponBaseGun*>(pBaseWeapon));
	else
		return Globals::m_pCmd->m_nButtons.m_nValue & (IN_ATTACK) && CanShoot(static_cast<C_CSWeaponBaseGun*>(pBaseWeapon));
}

bool CCSPlayerController::IsThrowingGrenade(C_CSWeaponBase* pBaseWeapon)
{
	if (!pBaseWeapon)
		return false;

	if (!Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return false;

	const float flServerTime = TICKS_TO_TIME(this->m_nTickBase());
	const short nDefinitionIndex = pBaseWeapon->m_AttributeManager()->m_Item()->m_iItemDefinitionIndex();
	CCSWeaponBaseVData* pWeaponBaseVData = pBaseWeapon->GetWeaponBaseVData();
	if (!pWeaponBaseVData)
		return false;

	if (pWeaponBaseVData->m_WeaponType() == WEAPONTYPE_GRENADE)
	{
		C_BaseCSGrenade* pGrenade = reinterpret_cast<C_BaseCSGrenade*>(pBaseWeapon);
		if (pGrenade != nullptr)
		{
			return !pGrenade->m_bPinPulled() && pGrenade->m_fThrowTime() > 0.f && pGrenade->m_fThrowTime() < flServerTime;
		}
	}

	return false;
}

bool C_CSWeaponBaseGun::CanPrimaryAttack(const int nWeaponType, const float flServerTime)
{

	if (this->m_bBurstMode())
	{

		if (this->m_iBurstShotsRemaining() > 0 )
			return true;
	}

	if (this->m_nNextPrimaryAttackTick() > TIME_TO_TICKS(flServerTime))
		return false;

	if (nWeaponType == WEAPONTYPE_KNIFE)
		return true;

	if (this->m_iClip1() <= 0)
		return false;

	const std::uint16_t nDefinitionIndex = this->m_AttributeManager()->m_Item()->m_iItemDefinitionIndex();

	if (nDefinitionIndex == WEAPON_R8_REVOLVER && this->m_flPostponeFireReadyFrac() > flServerTime)
		return false;

	return true;
}

int C_CSPlayerPawn::GetAssociatedTeam( )
{
	const int nTeam = this->m_iTeamNum( );

	return nTeam;
}

bool C_CSPlayerPawn::IsEnemy( C_CSPlayerPawn* pOther )
{

	if (pOther == nullptr || this == pOther)
		return false;

	if (Convar::game_type->GetInt() == GAMETYPE_FREEFORALL && Convar::game_mode->GetInt() == GAMEMODE_FREEFORALL_SURVIVAL)

		return ( this->m_nSurvivalTeam( ) != pOther->m_nSurvivalTeam( ) );

	if (Convar::mp_teammates_are_enemies->GetBool( ))
		return true;

	return this->GetAssociatedTeam( ) != pOther->GetAssociatedTeam( );
}

bool C_CSPlayerPawn::IsVisible(C_CSPlayerPawn* pPawn, Vector vecPoint, bool bSmokeCheck, float flMaxDensity)
{
	if (!this || !pPawn)
		return false;

	TraceFilter_t filter(TRACE_MASK, this, nullptr, 4);
	Ray_t ray = Ray_t();
	GameTrace_t trace = GameTrace_t();

	Vector vecSource = this->GetEyePosition( );
	Interfaces::m_pVPhys2World->TraceShape(&ray, vecSource, vecPoint, &filter, &trace);

	if ((trace.IsVisible() || (trace.m_pHitEntity && trace.m_pHitEntity->GetRefEHandle().GetEntryIndex() == pPawn->GetRefEHandle().GetEntryIndex())) && !(bSmokeCheck && !Utilities::LineGoesThroughSmoke(vecSource, vecPoint, flMaxDensity)))
		return true;

	return false;
}

std::uint16_t C_CSPlayerPawn::GetCollisionMask()
{
	if (this && m_pCollision())
		return m_pCollision()->CollisionMask();

	return 0;
}

std::uint32_t C_CSPlayerPawn::GetOwnerHandle()
{
	std::uint32_t Result = -1;
	if (this && m_pCollision() && !(m_pCollision()->m_usSolidFlags() & 4))
		Result = this->m_hOwnerEntity().GetEntryIndex( );

	return Result;
}

std::uint32_t C_CSPlayerPawn::GetEntityHandle()
{
	#ifdef CS_PARANOID
	CS_ASSERT(Functions::fnGetEntityHandle != nullptr);
	#endif

	return ReturnAddressSpoofer::SpoofCall(ReturnAddressSpoofGadgets::m_pClientGadet, Functions::fnGetEntityHandle, this);
}

void C_CSPlayerPawn::CreateShockEffect()
{
	CEffectData effectData = CEffectData( );
	effectData.m_hEntity = this->GetEntityHandle();
	effectData.m_hOtherEntity = static_cast<std::uint32_t>(-1);
	effectData.m_nColor = IM_COL32(255, 0, 0, 255);
	effectData.CreateEffect(X("csshocked"));
}

bool C_CSPlayerPawn::IsArmored(int nHitGroup)
{
	if (nHitGroup == HITGROUP_HEAD)
	{
		if (!this->m_pItemServices())
			return false;

		return this->m_pItemServices()->m_bHasHelmet();
	}
	else
		return this->m_ArmorValue() > 0;
}

void CPlayer_MovementServices::ParachuteAccelerate(Vector& vecOutVelo, float& flStamina, float& flFriction, Vector vecMoveImpulses, float flFrametime)
{
	static auto sv_staminarecoveryrate = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_staminarecoveryrate"));
	static auto sv_air_max_wishspeed = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_air_max_wishspeed"));
	static auto sv_airaccelerate = Interfaces::m_pCvar->Find(FNV1A::HashConst("sv_airaccelerate"));

	float flMaxSpeed = vecOutVelo.Length2D();
	if (flStamina > 0)
	{
		float flSpeedScale = std::clamp(1.0f - (flStamina / 100.f), 0.f, 1.f);
		flMaxSpeed *= flSpeedScale * flSpeedScale;
		flStamina = fmaxf(flStamina - (flFrametime * sv_staminarecoveryrate->GetFloat()), 0.f);
	}

	float flYaw = Math::FloatNormalize(Globals::m_pBaseCmd->viewangles().y());

	QAngle angViewAngles = { 0, flYaw, 0 };

	Vector vecForward, vecLeft, vecUp;
	angViewAngles.ToDirections(&vecForward, &vecLeft, &vecUp);
	vecForward.NormalizeInPlace();
	vecLeft.NormalizeInPlace();

	Vector vecWishDir(
		((vecForward.x * vecMoveImpulses.x) * flMaxSpeed) - ((vecLeft.x * vecMoveImpulses.y) * flMaxSpeed),
		((vecForward.y * vecMoveImpulses.x) * flMaxSpeed) - ((vecLeft.y * vecMoveImpulses.y) * flMaxSpeed),
		0.f
	);

	float flWishSpeed = vecWishDir.NormalizeInPlace();
	flWishSpeed = fminf(flWishSpeed, flMaxSpeed);

	const float flSpeedAdd = fminf(flWishSpeed, sv_air_max_wishspeed->GetFloat() - vecOutVelo.DotProduct(vecWishDir));

	if (flSpeedAdd < 0.f)
		return;

	const float flAccelSpeed = fminf(flSpeedAdd, ((flWishSpeed * flFrametime) * sv_airaccelerate->GetFloat()) * flFriction);
	vecOutVelo += (vecWishDir * flAccelSpeed) / 2;
}
std::vector<C_CS2HudModelWeapon*> C_CSPlayerPawn::GetViewModels()
{
	auto* pArms = m_hHudModelArms().Get();
	std::vector<C_CS2HudModelWeapon*> vecViewModels;

	if (pArms)
	{
		auto pNode = pArms->m_pGameSceneNode();
		if (pNode)
		{
			for (auto* child = pNode->m_pChild(); child; child = child->m_pNextSibling())
			{
				auto* owner = reinterpret_cast<C_BaseEntity*>(child->m_pOwner());
				if (owner && owner->IsCS2HudModelWeapon())
					vecViewModels.push_back(reinterpret_cast<C_CS2HudModelWeapon*>(owner));
			}
		}
	}
	return vecViewModels;
}

C_CS2HudModelWeapon* C_CSPlayerPawn::GetViewModel()
{
	auto weapon_handle = this->m_pWeaponServices()->m_hActiveWeapon();
	auto* pWeapon = weapon_handle.Get();

	if (pWeapon)
	{
		const auto& viewModels = GetViewModels();
		for (auto pVM : viewModels)
		{
			auto* owner = pVM->m_hOwnerEntity().Get();
			if ((uintptr_t)owner == (uintptr_t)pWeapon)
				return pVM;
		}
	}
	return nullptr;
}
void CPlayer_MovementServices::RunCommand_Rebuilt(CUserCmd* pCmd)
{

}

void CCSPlayer_MovementServices::SetupMove( CUserCmd* pCmd, CMoveData* pMoveData )
{
	auto pawn = m_pPawn( );
	auto entity = pawn->m_pEntity( );

	int packed_index = entity->nIndex( );
	int player_index = 0x7FFF;

	if ( packed_index != -1 ) {
		int base_index = packed_index & 0x7FFF;
		int extra_bits = ( packed_index >> 15 ) - ( entity->m_flags( ) & 1 );
		player_index = base_index | ( extra_bits << 15 );
	}

	pMoveData->m_nSomeEntIndex = player_index;

	const auto& view_angles = pCmd->m_csgoUserCmd.base( ).viewangles( );
	pMoveData->m_vAbsOrigin = pawn->m_pGameSceneNode( )->m_vecAbsOrigin( );
	pMoveData->m_vViewAngles = Vector( view_angles.x( ), view_angles.y( ), view_angles.z( ) );
	pMoveData->m_vAbsViewAngles = pMoveData->m_vViewAngles;
	pMoveData->m_vAbsVelocity = pawn->m_vecAbsVelocity( );

	const bool is_in_no_clip = ( pawn->m_fFlags( ) & 0x40 ) != 0;
	const bool is_on_ladder = ( pawn->m_fFlags( ) & 0x20 ) != 0;

	if ( is_in_no_clip || is_on_ladder )
	{
		pMoveData->m_flForwardMove = 0.0f;
		pMoveData->m_flUpMove = 0.0f;
		pMoveData->m_vLastMovementImpulses = Vector( );

		if ( is_on_ladder )
			ForceButtonsDown( 0 );
	}
	else
	{
		const auto& base_cmd = pCmd->m_csgoUserCmd.base( );
		pMoveData->m_flForwardMove = base_cmd.forwardmove( );
		pMoveData->m_flSideMove = base_cmd.leftmove( );
		pMoveData->m_flUpMove = base_cmd.upmove( );

		pMoveData->m_vLastMovementImpulses = Vector( base_cmd.forwardmove( ), base_cmd.leftmove( ), base_cmd.upmove( ) );
	}

	pMoveData->m_SubtickMoves.m_nSize = 0;
	pMoveData->m_AttackSubtickMoves.m_nSize = 0;
	pMoveData->m_bHasSubtickInputs = false;
	pMoveData->m_flLatestSubtickFraction = 1.0f;

	const int subtick_size = pCmd->m_csgoUserCmd.base( ).subtick_moves_size( );
	for ( int i = 0; i < subtick_size; ++i ) {
		auto* subtick = pCmd->m_csgoUserCmd.mutable_base( )->mutable_subtick_moves( i );
		if ( !subtick )
			continue;

		C_SubtickMove_ move{};
		move.m_bPressed = subtick->pressed( );
		move.m_flWhen = subtick->when( );
		move.m_uButton = subtick->button( );
		move.m_AnalogMove.m_flAnalogForwardDelta = subtick->analog_forward_delta( );
		move.m_AnalogMove.m_flAnalogLeftDelta = subtick->analog_left_delta( );

		if ( pMoveData->m_AttackSubtickMoves.m_pData && pMoveData->m_SubtickMoves.m_pData )
		{
			pMoveData->m_SubtickMoves.At( i ) = move;
			pMoveData->m_AttackSubtickMoves.At( i ) = move;
		}
	}

	pMoveData->m_bHasSubtickInputs = ( pMoveData->m_SubtickMoves.m_nSize > 0 ) || ( pMoveData->m_AttackSubtickMoves.m_nSize > 0 );

	if ( pCmd->m_iFlag == 2 ) {
		float latest_fraction = 0.0f;

		if ( pMoveData->m_AttackSubtickMoves.m_nSize > 0 ) {
			int last_index = pMoveData->m_AttackSubtickMoves.m_nSize;
			latest_fraction = pMoveData->m_AttackSubtickMoves.At( last_index ).m_flWhen;
		}

		pMoveData->m_flLatestSubtickFraction = std::max( latest_fraction, 0.0f );
	}

	if ( ( pawn->m_fFlags( ) & 1 ) == 0 )
		pMoveData->m_vAbsVelocity.z = pawn->m_vecBaseVelocity( ).z;

	pMoveData->m_vAngles = pMoveData->m_vAbsViewAngles;
	pMoveData->m_bGameCodeMovedPlayer = ( pCmd->m_nCommandNumber == this->m_nGameCodeHasMovedPlayerAfterCommand( ) + 1 );
	pMoveData->m_vOldAngles = pawn->v_angle( );
	pMoveData->m_flMaxSpeed = pawn->m_flMaxPlayerMovementSpeed( );
	pMoveData->m_flFrictionDecel = this->m_flMaxspeed( );

}

float CCSPlayer_MovementServices::GetGroundPosition( CMoveData* mv )
{
	TraceFilter_t filter;
	Interfaces::m_pVPhys2World->InitPlayerMovementTraceFilter( &filter, Globals::m_pLocalPlayerPawn,
		Globals::m_pLocalPlayerPawn->m_pCollision( )->m_collisionAttribute( ).m_nInteractsWith,
		COLLISION_GROUP_PLAYER_MOVEMENT );

	Vector ground = mv->m_vAbsOrigin;
	ground.z -= 2;

	float standableZ = 0.7;

	CONVAR( sv_standable_normal );

	BBox_t bounds;
	GetBBoxBounds( &bounds );

	GameTrace_t trace;
	Interfaces::m_pVPhys2World->InitTraceInfo( &trace );

	Interfaces::m_pVPhys2World->TracePlayerBBox( &mv->m_vAbsOrigin, &ground, &bounds, &filter, &trace );

	if ( trace.m_bAllSolid || trace.m_flFraction == 1.0f )
	{
		return mv->m_vAbsOrigin.z;
	}

	return trace.m_vecEndPos.z;
}

void C_CSWeaponBase::UpdateAccuracyPenalty_Rebuilt( )
{
	CONVAR( weapon_air_spread_scale );

	if ( !Interfaces::m_pGlobalVariables->m_flCurrentTime )
		return;

	C_BaseEntity* pEntity = this->m_hOwnerEntity( ).Get( );
	if ( !pEntity )
		return;

	float flCurrentTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;

	float flInAccuracy = 0.f;

	if ( pEntity->m_nActualMoveType( ) == MOVETYPE_LADDER )
	{
		float flLadderInAccuracy1 = this->GetLadderInAccuracy( this->m_weaponMode( ) );
		float flLadderInAccuracy2 = this->GetLadderInAccuracy( 0 );

		flInAccuracy = flLadderInAccuracy1 + flLadderInAccuracy2;
	}
	else
	{
		if ( !( pEntity->m_fFlags( ) & FL_ONGROUND ) )
		{
			float flStandInAccuracy = this->GetStandInAccuracy( this->m_weaponMode( ) );
			float flJumpInAccuracy = this->GetJumpInAccuracy( this->m_weaponMode( ) );

			flInAccuracy = flStandInAccuracy + (flJumpInAccuracy * weapon_air_spread_scale->GetFloat( ) );
		}
		else
		{
			if ( pEntity->m_fFlags( ) & FL_DUCKING )
				flInAccuracy = this->GetDuckInAccuracy( this->m_weaponMode( ) );
			else
				flInAccuracy = this->GetStandInAccuracy( this->m_weaponMode( ) );
		}
	}

	if ( this->m_bInReload( ) )
		flInAccuracy += LocalPlayerData::m_pWeaponBaseVData->m_flInaccuracyReload( );

	if ( flInAccuracy <= this->m_fAccuracyPenalty( ) )
	{
		static float flLogResult = logf( 10.f );

		float flWeaponInAccuracyRecoveryTime = this->GetWeaponInAccuracyRecoveryTime( );
		float flAccuracyPenalty = this->m_fAccuracyPenalty( );
		float flNewAccuracyPenalty = expf( -( flLogResult / flWeaponInAccuracyRecoveryTime ) * INTERVAL_PER_TICK ) * ( flAccuracyPenalty - flInAccuracy ) + flInAccuracy;

		if ( this->m_fAccuracyPenalty( ) != flNewAccuracyPenalty )
			this->m_fAccuracyPenalty( ) = flNewAccuracyPenalty;
	}
	else
	{
		this->m_fAccuracyPenalty( ) = flInAccuracy;
	}

	this->UpdateTurningInAccuracy( );

	if ( this->m_flRecoilIndex( ) > 0.f )
	{
		float flCycleTime = this->GetCycleTime( this->m_weaponMode( ) );
		float flLastShotTime = this->m_fLastShotTime( );

		if ( flCurrentTime > ( flLastShotTime + flCycleTime + INTERVAL_PER_TICK ) )
		{
			const float flLogResult = logf( 10.0f ) * 2.0f;
			const float flDecay = expf( -INTERVAL_PER_TICK * flLogResult );

			float flRecoilIndex = this->m_flRecoilIndex( );
			float flNewRecoilIndex = flRecoilIndex * flDecay;

			if ( flRecoilIndex != flNewRecoilIndex )
			{
				this->m_flRecoilIndex( ) = flNewRecoilIndex;
				flRecoilIndex = flNewRecoilIndex;
			}

			if ( flRecoilIndex > 0.0f && flRecoilIndex <= 0.1f )
				this->m_flRecoilIndex( ) = 0.0f;
		}
	}

	this->m_flLastAccuracyUpdateTime( ) = flCurrentTime;
}
C_CSWeaponBase* C_CSPlayerPawn::GetLocalActiveWeapon()
{
	CPlayer_WeaponServices* pWeaponService = m_pWeaponServices();

	if (pWeaponService)
	{
		C_CSWeaponBase* pLocalWeapon = pWeaponService->m_hActiveWeapon().Get();

		if (pLocalWeapon)
			return pLocalWeapon;
	}

	return nullptr;
}