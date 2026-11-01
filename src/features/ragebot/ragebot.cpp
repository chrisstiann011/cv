#include "Ragebot.h"
#include "../threading/Threader.h"

constexpr Vector vWorldUpVector = Vector(0, 0, 1);

using namespace NRagebot;

float CalculateDynamicPointScale(const Vector& vPoint, const HitboxData_t& hitbox) {
	float flSpread = LocalPlayerData::m_pWeapon->GetSpread() + LocalPlayerData::m_pWeapon->GetInaccuracy().flTotalInaccuracy;
	float flDistance = vPoint.DistTo(m_vExtrapShootPos);
	float flEffectiveSpread = flSpread * flDistance;

	const float flAdjustedRadius = std::max(hitbox.m_flRadius - flEffectiveSpread * 0.3f, hitbox.m_flRadius * 0.1f);
	return std::clamp(flAdjustedRadius / hitbox.m_flRadius, 0.0f, 1.0f) * 100.0f;
}

std::vector<Vector> CMultipointManager::GetMultiPoints(HitboxData_t hitbox) {
	std::vector<Vector> vecPoints;

	Vector center = hitbox.m_vCenter;
	Vector mins = hitbox.m_vMins;
	Vector maxs = hitbox.m_vMaxs;

	float flPointScale = g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale;
	if (flPointScale <= 0) {
		flPointScale = CalculateDynamicPointScale(center, hitbox);
	}

	float flScale = flPointScale / 100.f;

	switch (hitbox.m_nHitboxIndex) {
	case HITBOX_HEAD:
		vecPoints.push_back(center);
		vecPoints.push_back(Vector(center.x, center.y, center.z + (hitbox.m_flRadius - 0.1f) * flScale));
		vecPoints.push_back(Vector(center.x, center.y + (hitbox.m_flRadius - 0.2f) * flScale, center.z));
		vecPoints.push_back(Vector(center.x, center.y - (hitbox.m_flRadius - 0.2f) * flScale, center.z));
		vecPoints.push_back(Vector(center.x - (hitbox.m_flRadius - 0.2f) * flScale, center.y, center.z));
		break;
	case HITBOX_UPPER_CHEST:
	case HITBOX_CHEST:
		vecPoints.push_back(center);
		vecPoints.push_back(Vector(center.x, mins.y - hitbox.m_flRadius * flScale, center.z));
		vecPoints.push_back(Vector(center.x, center.y, maxs.z + (hitbox.m_flRadius - 0.2f) * flScale));
		vecPoints.push_back(Vector(center.x, center.y, mins.z - (hitbox.m_flRadius - 0.2f) * flScale));
		break;

	case HITBOX_STOMACH:
	case HITBOX_PELVIS:
		vecPoints.push_back(center);
		vecPoints.push_back(Vector(center.x, mins.y - hitbox.m_flRadius * flScale, center.z));
		vecPoints.push_back(Vector(center.x, center.y, maxs.z + (hitbox.m_flRadius - 0.2f) * flScale));
		vecPoints.push_back(Vector(center.x, center.y, mins.z - (hitbox.m_flRadius - 0.2f) * flScale));
		break;

	case HITBOX_RIGHT_HAND:
	case HITBOX_LEFT_HAND:
	case HITBOX_RIGHT_FOREARM:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_RIGHT_UPPER_ARM:
	case HITBOX_LEFT_UPPER_ARM:
		vecPoints.push_back(center);
		vecPoints.push_back(Vector(center.x, center.y, maxs.z + (hitbox.m_flRadius - 0.2f) * flScale));
		vecPoints.push_back(Vector(center.x, center.y, mins.z - (hitbox.m_flRadius - 0.2f) * flScale));
		break;

	case HITBOX_RIGHT_THIGH:
	case HITBOX_LEFT_THIGH:
	case HITBOX_RIGHT_CALF:
	case HITBOX_LEFT_CALF:
	case HITBOX_RIGHT_FOOT:
	case HITBOX_LEFT_FOOT:
		vecPoints.push_back(center);
		vecPoints.push_back(Vector(center.x, center.y, center.z - (hitbox.m_flRadius - 1.2f) * flScale));
		break;

	default:
		vecPoints.push_back(center);
		break;
	}

	return vecPoints;
}

bool CMultipointManager::ShouldUseMultipoint(int nHitboxIndex) {
	const auto& hitboxes = g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes;

	auto GetMultipointState = [&](EConfigHitBoxes hitboxType) -> bool {
		for (const auto& hitbox : hitboxes) {
			if (hitbox.m_eDefinition == hitboxType) {
				return hitbox.m_bMultipoint;
			}
		}
		return false;
		};

	switch (nHitboxIndex) {
	case HITBOX_HEAD:
		return GetMultipointState(HB_HEAD);
	case HITBOX_UPPER_CHEST:
	case HITBOX_CHEST:
		return GetMultipointState(HB_CHEST);
	case HITBOX_STOMACH:
	case HITBOX_PELVIS:
		return GetMultipointState(HB_STOMACH);
	case HITBOX_RIGHT_HAND:
	case HITBOX_LEFT_HAND:
	case HITBOX_RIGHT_FOREARM:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_RIGHT_UPPER_ARM:
	case HITBOX_LEFT_UPPER_ARM:
		return GetMultipointState(HB_ARMS);
	case HITBOX_RIGHT_THIGH:
	case HITBOX_LEFT_THIGH:
	case HITBOX_RIGHT_CALF:
	case HITBOX_LEFT_CALF:
	case HITBOX_RIGHT_FOOT:
	case HITBOX_LEFT_FOOT:
		return GetMultipointState(HB_LEGS);
	default:
		return false;
	}
}

std::vector<Vector> CMultipointManager::GenerateMultipoints(HitboxData_t hitbox) {
	if (!ShouldUseMultipoint(hitbox.m_nHitboxIndex)) {
		return { hitbox.m_vCenter };
	}

	return GetMultiPoints(hitbox);
}

float SpeedRatioCalc(float flCurrentSpeed2D, float flAccurateSpeed, float flAdjustedFullSpeed)
{
	float v6 = flCurrentSpeed2D - flAccurateSpeed;
	float v7 = flAdjustedFullSpeed - flAccurateSpeed;

	float v9 = v6 / v7;
	return std::clamp(v9, 0.0f, 1.0f);
}

bool CAccuracy::HasMaximumAccuracy()
{
	CONVAR(weapon_accuracy_nospread);

	if (weapon_accuracy_nospread->GetBool())
		return true;

	if (!Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerPawn->m_pCameraServices())
		return false;

	else if (LocalPlayerData::m_nWeaponDefinitionIndex == WEAPON_SSG_08 || LocalPlayerData::m_nWeaponDefinitionIndex == WEAPON_AWP || LocalPlayerData::m_nWeaponDefinitionIndex == WEAPON_SCAR_20 || LocalPlayerData::m_nWeaponDefinitionIndex == WEAPON_G3SG1) {

		Vector vCam = Globals::m_pLocalPlayerPawn->m_pCameraServices()->m_vClientScopeInaccuracy();

		if (vCam.x <= 1e-6f)
			vCam.x = 0.f;

		switch (LocalPlayerData::m_nWeaponDefinitionIndex) {
		case WEAPON_SSG_08:
			return vCam.x <= 0.00089f;
		case WEAPON_AWP:
			return vCam.x <= 0.0005f;
		case WEAPON_SCAR_20:
		case WEAPON_G3SG1:
			return vCam.x <= 0.0012f;
		default:
			return vCam.x <= 0.00089f;
		}
	}
	else if (LocalPlayerData::m_nWeaponDefinitionIndex == WEAPON_AUG || LocalPlayerData::m_nWeaponDefinitionIndex == WEAPON_SG_553) {

		Vector vCam = Globals::m_pLocalPlayerPawn->m_pCameraServices()->m_vClientScopeInaccuracy();

		if (vCam.x <= 1e-6f)
			vCam.x = 0.f;

		return vCam.x <= 0.0025f;
	}
	else if (LocalPlayerData::m_nWeaponDefinitionIndex == WEAPON_R8_REVOLVER) {
		C_CSWeaponBase* pWeapon = Globals::m_pLocalPlayerPawn->GetLocalActiveWeapon();
		if (!pWeapon || !pWeapon->IsWeapon())
			return false;

		auto inaccuracyData = pWeapon->GetInaccuracy();
		float flTotalInaccuracy = inaccuracyData.flTotalInaccuracy;

		return flTotalInaccuracy <= 0.005f;
	}
	else {
		float velocity = Globals::m_pLocalPlayerPawn->m_vecAbsVelocity().Length2D();
		bool bOnGround = Globals::m_pLocalPlayerPawn->m_fFlags() & FL_ONGROUND;

		return velocity <= 35.f && bOnGround;
	}
}

int GetRandomSeedFast(float flPitch, float flYaw, int nTickCount)
{
	struct HashBuffer
	{
		float flPitch;
		float flYaw;
		int nTick;
	};

	HashBuffer buffer;
	buffer.flPitch = flPitch;
	buffer.flYaw = flYaw;
	buffer.nTick = nTickCount;

	CSHA1 sha1;
	sha1.Reset();
	sha1.Update((const unsigned char*)&buffer, sizeof(buffer));
	sha1.Final();

	return *(int*)&sha1.m_digest[0];
}

float RoundHalf(float flValue)
{
	return floor(flValue + flValue) * 0.5f;
}

int GetRandomSeed(const QAngle& angAngles, int nTickCount)
{
	const float flPitch = RoundHalf(std::remainder(angAngles.x, 360.0f));
	const float flYaw = RoundHalf(std::remainder(angAngles.y, 360.0f));
	return GetRandomSeedFast(flPitch, flYaw, nTickCount);
}

static std::vector<Vector> CalculateSpreadInternal(int seed, float accuracy, float spread, float recoil_index, int item_def_idx, int num_bullets)
{
	std::vector<Vector> spread_results(num_bullets);
	static auto fnCalcSpread = reinterpret_cast<void(__fastcall*)(int16_t, int, int, std::uint32_t, float, float, float, float*, float*)>(Memory::FindPattern(CLIENT_DLL, X("48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 4C 63 EA")));
	if (!fnCalcSpread) return { 0.f, 0.f };
	fnCalcSpread(
		item_def_idx,
		num_bullets,
		0,
		seed + 1,
		accuracy,
		spread,
		recoil_index,
		reinterpret_cast<float*>(&spread_results[0].x),
		reinterpret_cast<float*>(&spread_results[0].y)
	);

	return spread_results;
}

Vector2D CalculateSpreadManual(int nSeed, float flInaccuracy, float flSpread, float flRecoilIndex, short nWeaponID) {
	Math::fnRandomSeed(nSeed + 1);

	float flInaccX = Math::fnRandomFloat(0.0f, 1.0f);
	float flInaccY = Math::fnRandomFloat(0.0f, 1.0f);
	float flSpreadX = Math::fnRandomFloat(0.0f, 1.0f);
	float flSpreadY = Math::fnRandomFloat(0.0f, 1.0f);

	float flInaccRadius = flInaccuracy * std::sqrt(flInaccX);
	float flSpreadRadius = flSpread * std::sqrt(flSpreadX);

	float flInaccAngle = flInaccY * M_2PI;
	float flSpreadAngle = flSpreadY * M_2PI;

	if (nWeaponID == WEAPON_NEGEV && flRecoilIndex < 3.0f) {
		for (int i = 3; i > flRecoilIndex; i--) {
			flInaccX = flInaccX * flInaccX;
			flSpreadX = flSpreadX * flSpreadX;
		}
		flInaccRadius = flInaccuracy * std::sqrt(1.0f - flInaccX);
		flSpreadRadius = flSpread * std::sqrt(1.0f - flSpreadX);
	}

	Vector2D vSpread;
	vSpread.x = std::cos(flInaccAngle) * flInaccRadius + std::cos(flSpreadAngle) * flSpreadRadius;
	vSpread.y = std::sin(flInaccAngle) * flInaccRadius + std::sin(flSpreadAngle) * flSpreadRadius;

	return vSpread;
}

Vector2D GetSpread(const int nSeed, const float flInaccuracy, const float flSpread)
{
	const float flRecoilIndex = Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get()->m_flRecoilIndex();
	Vector2D vSpread = { 0.f, 0.f };
	static auto fnCalcSpread = reinterpret_cast<void(__fastcall*)(int16_t, int, int, std::uint32_t, float, float, float, float*, float*)>(Memory::FindPattern(CLIENT_DLL, X("48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 4C 63 EA")));
	fnCalcSpread(Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get()->m_AttributeManager()->m_Item()->m_iItemDefinitionIndex(), Globals::m_pLocalPlayerPawn->m_pWeaponServices()->m_hActiveWeapon().Get()->GetWeaponBaseVData()->m_nNumBullets(), false ? 1 : 0, nSeed, flInaccuracy, flSpread, flRecoilIndex, &vSpread.x, &vSpread.y);
	return vSpread;
}

float ForceShoot(const Vector& vShootPos, const Vector& vTargetPoint, const HitboxData_t& hitbox, float flConfigHitChance) {
	if (flConfigHitChance <= 0.f)
		return 0.f;

	if (flConfigHitChance >= 100.f)
		return 100.f;

	float flMaxHitChance = flConfigHitChance;
	float flDistanceFactor = 1.0f;
	float flAngleFactor = 1.0f;
	float flHitboxFactor = 1.0f;
	float flVelocityFactor = 1.0f;

	float flDistance = vShootPos.DistTo(vTargetPoint);
	if (flDistance > 1000.f) {
		flDistanceFactor = std::max(0.3f, 1.0f - (flDistance - 1000.f) / 2000.f);
	}
	else if (flDistance < 200.f) {
		flDistanceFactor = 1.1f;
	}

	Vector vDirection = (vTargetPoint - vShootPos).Normalized();
	Vector vForward, vRight, vUp;
	Interfaces::m_pInput->GetViewAngles().ToDirections(&vForward, &vRight, &vUp);

	float flDotForward = vDirection.DotProduct(vForward);
	float flDotRight = vDirection.DotProduct(vRight);
	float flDotUp = vDirection.DotProduct(vUp);

	if (flDotForward > 0.95f) {
		flAngleFactor = 1.0f;
	}

	else if (std::abs(flDotRight) > 0.5f || std::abs(flDotUp) > 0.3f) {
		flAngleFactor = 0.7f;
	}

	else if (flDotForward < 0.0f) {
		flAngleFactor = 0.3f;
	}

	switch (hitbox.m_nHitboxIndex) {
	case HITBOX_HEAD:
		flHitboxFactor = 0.8f;
		break;
	case HITBOX_CHEST:
	case HITBOX_UPPER_CHEST:
		flHitboxFactor = 1.0f;
		break;
	case HITBOX_STOMACH:
		flHitboxFactor = 1.1f;
		break;
	case HITBOX_PELVIS:
		flHitboxFactor = 1.2f;
		break;
	case HITBOX_LEFT_UPPER_ARM:
	case HITBOX_RIGHT_UPPER_ARM:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_RIGHT_FOREARM:
		flHitboxFactor = 0.9f;
		break;
	case HITBOX_LEFT_THIGH:
	case HITBOX_RIGHT_THIGH:
	case HITBOX_LEFT_CALF:
	case HITBOX_RIGHT_CALF:
		flHitboxFactor = 0.8f;
		break;
	default:
		flHitboxFactor = 1.0f;
		break;
	}

	if (g_Rage->m_Data.m_LastTarget.m_pLagRecord && Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_kbForceShoot))) {
		float flTargetSpeed = g_Rage->m_Data.m_LastTarget.m_pLagRecord->m_pPawn->m_vecVelocity().Length2D();
		if (flTargetSpeed > 100.f) {
			flVelocityFactor = std::max(0.4f, 1.0f - (flTargetSpeed - 100.f) / 300.f);
		}
	}

	float flLocalSpeed = Globals::m_pLocalPlayerPawn->m_vecAbsVelocity().Length2D();
	if (flLocalSpeed > 50.f) {
		flVelocityFactor *= std::max(0.5f, 1.0f - (flLocalSpeed - 50.f) / 200.f);
	}

	float flFinalHitChance = flMaxHitChance * flDistanceFactor * flAngleFactor * flHitboxFactor * flVelocityFactor;

	return std::clamp(flFinalHitChance, 0.0f, flMaxHitChance);
}

bool CAccuracy::IsAccurate()
{
	if (!g_Rage->m_Data.m_LastTarget.HasValidTarget() || !g_Rage->m_Data.m_LastTarget.m_pLagRecord)
		return false;

	int iHitChance = m_bHoldingHitchanceOverride
		? g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride
		: g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance;

	if (g_Rage->m_Data.m_LastTarget.HasValidTarget() && Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_kbForceShoot))) {
		float flDynamicHitChance = ForceShoot(
			m_vExtrapShootPos,
			g_Rage->m_Data.m_LastTarget.m_vPoint,
			g_Rage->m_Data.m_LastTarget.m_hitboxData,
			static_cast<float>(iHitChance)
		);
		iHitChance = static_cast<int>(flDynamicHitChance);
	}

	if (iHitChance <= 0)
		return true;

	if (g_Accuracy->HasMaximumAccuracy())
		return true;

	const float flWeaponRange = LocalPlayerData::m_pWeaponBaseVData->m_flRange();
	if (flWeaponRange <= 0.f)
		return false;

	const float flWeaponInaccuracy = LocalPlayerData::m_pWeapon->GetInaccuracy().flTotalInaccuracy;
	const float flWeaponSpread = LocalPlayerData::m_pWeapon->GetSpread();
	const float flRecoilIndex = LocalPlayerData::m_pWeapon->m_flRecoilIndex();
	const int nNumBullets = LocalPlayerData::m_pWeaponBaseVData->m_nNumBullets();

	if (flWeaponInaccuracy <= 0.f && flWeaponSpread <= 0.f)
		return true;

	Vector vForward, vRight, vUp;
	g_Rage->m_Data.m_LastTarget.m_angToTarget.ToDirections(&vForward, &vRight, &vUp);

	constexpr int nWantedHitCount = 256;
	const int nNeededHits = static_cast<int>(std::ceil(nWantedHitCount * (iHitChance / 100.f)));

	if (nNeededHits <= 0)
		return true;

	if (nNeededHits > nWantedHitCount)
		return false;

	int nHits = 0;
	const Vector& vTargetPoint = g_Rage->m_Data.m_LastTarget.m_vPoint;
	HitboxData_t& targetHitbox = g_Rage->m_Data.m_LastTarget.m_hitboxData;
	C_CSPlayerPawn* pTargetPawn = g_Rage->m_Data.m_LastTarget.m_pLagRecord->m_pPawn;

	if (!pTargetPawn || vTargetPoint.IsZero())
		return false;

	GameTrace_t GameTrace = GameTrace_t();
	TraceFilter_t Filter = TraceFilter_t(TRACE_MASK_AUTOWALL, Globals::m_pLocalPlayerPawn, nullptr, 4);
	Ray_t Ray = Ray_t();

	const Vector vStartPos = m_vShootPos;
	const float flTargetDist = (vTargetPoint - vStartPos).Length();
	const float flMaxRange = std::min(flWeaponRange, flTargetDist + 50.f);

	auto fnCheckHit = [&](const Vector2D& vSpread) -> bool
		{
			Vector vDirection = vForward + (vRight * vSpread.x) + (vUp * vSpread.y);
			vDirection.NormalizeInPlace();

			const Vector vEnd = vStartPos + (vDirection * flMaxRange);

			if (!Interfaces::m_pVPhys2World->ClipRayToEntity(&Ray, vStartPos, vEnd, pTargetPawn, &Filter, &GameTrace))
				return false;

			if (GameTrace.m_pHitEntity != pTargetPawn)
				return false;

			const Vector vHitPoint = GameTrace.m_vecEndPos;
			const float flDistToTargetPoint = (vHitPoint - vTargetPoint).Length();

			if (targetHitbox.m_nHitboxIndex >= 0 && targetHitbox.m_flRadius > 0.f)
			{
				if (targetHitbox.SegmentIntersectsCapsule(vStartPos, vHitPoint))
					return true;

				const float flDistToCenter = (vHitPoint - targetHitbox.m_vCenter).Length();
				if (flDistToCenter <= targetHitbox.m_flRadius + 2.f)
					return true;
			}

			return flDistToTargetPoint < 6.f;
		};

	for (int nSeed = 0; nSeed < nWantedHitCount; nSeed++)
	{
		const std::vector<Vector> spreadResults = CalculateSpreadInternal(nSeed, flWeaponInaccuracy, flWeaponSpread, flRecoilIndex, LocalPlayerData::m_nWeaponDefinitionIndex, nNumBullets);

		if (spreadResults.empty())
			continue;

		const Vector2D vSpread = Vector2D(spreadResults[0].x, spreadResults[0].y);

		if (fnCheckHit(vSpread))
			nHits++;

		const int nRemainingSeeds = nWantedHitCount - (nSeed + 1);
		const int nRemainingNeeded = nNeededHits - nHits;

		if (nRemainingNeeded > nRemainingSeeds)
			return false;

		if (nHits >= nNeededHits)
			return true;
	}

	const float flFinalHitChance = (static_cast<float>(nHits) / static_cast<float>(nWantedHitCount)) * 100.f;
	return flFinalHitChance >= static_cast<float>(iHitChance);
}

bool CScan::IsBetterCandidate(ScanResult_t candidate, ScanResult_t current) {

	if (candidate.m_flDamage <= 0)
		return false;

	if (current.m_flDamage <= 0)
		return true;

	int targetingPriority = g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType;
	switch (targetingPriority) {
	case 0:
		return candidate.m_flDamage > current.m_flDamage;
	case 1:
		return candidate.m_flWorldDist < current.m_flWorldDist;
	case 2:
		return candidate.m_pLagRecord->m_pPawn->m_iHealth() < current.m_pLagRecord->m_pPawn->m_iHealth();
	default:
		return candidate.m_flDamage > current.m_flDamage;
	}
}

bool CScan::IsWithinFov(const Vector& vStartPos, const Vector& vTargetPos, const QAngle& angView, float flFovDegrees) {
	if (flFovDegrees >= 180.0f)
		return true;

	QAngle angToTarget = Math::CalcAngle(vStartPos, vTargetPos);

	angToTarget = Math::NormalizeAnglesReturn(angToTarget);
	QAngle normalizedView = Math::NormalizeAnglesReturn(angView);

	float flDeltaYaw = std::abs(angToTarget.y - normalizedView.y);

	if (flDeltaYaw > 180.0f)
		flDeltaYaw = 360.0f - flDeltaYaw;

	return flDeltaYaw <= (flFovDegrees * 0.5f);
}

float CheckIntersectDistance(Vector vStart, Vector vEnd, Vector vMin, Vector vMax, float flShapeRadius, Matrix3x4_t mMatrix) {
	float flDistance = FLT_MAX;
	if (flShapeRadius != -1.f) {
		Math::IntersectSegmentToCapsule(vStart, vEnd, vMin, vMax, flShapeRadius, std::ref(flDistance));
	}
	else {
		const Vector vDirection = (vEnd - vStart).Normalized();
		Vector vTransformedRay = vStart.Transform(mMatrix);
		Vector vRotatedDirection = vDirection.Rotate(mMatrix);
		Math::IntersectRayWithAABBNew(vTransformedRay, vRotatedDirection, vMin, vMax, std::ref(flDistance));
	}
	return flDistance;
}

ScanResult_t CScan::ProcessRecord(CLagRecord* pRecord) {
	if (!IsWithinFov(m_vExtrapShootPos, pRecord->m_vOrigin, Interfaces::m_pInput->GetViewAngles(), Config::f(g_Variables.m_Ragebot.m_flFov)))
		return {};

	if (!pRecord || !pRecord->m_pPawn) return {};

	ScanResult_t objCandidate = {};
	objCandidate.m_pLagRecord = pRecord;
	objCandidate.m_flWorldDist = (m_vExtrapShootPos - pRecord->m_vOrigin).Length();

	int nDamageThreshold = m_bHoldingMinOverride
		? g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride
		: ((g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage > 100)
			? (g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage - 100 + pRecord->m_pPawn->m_iHealth())
			: g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage);

	objCandidate.m_flDamageThreshold = nDamageThreshold;

	struct BestHitboxPoint_t {
		Vector m_vPoint = {};
		float m_flDamage = 0.f;
		float m_flAlignment = -1.f;
		float m_flSize = 0.f;
		HitboxData_t m_Hitbox = {};
		bool m_bIsHead = false;
		bool m_bIsLethal = false;
	};

	BestHitboxPoint_t bestHeadPoint = {};
	BestHitboxPoint_t bestBodyPoint = {};
	bool bHasHeadPoint = false;
	bool bHasBodyPoint = false;

	const float flWeaponRange = LocalPlayerData::m_pWeaponBaseVData->m_flRange();

	for (const auto& hitbox : pRecord->m_arrTargetHitboxes) {
		if (g_Rage->m_bForceHeadshot && hitbox.m_nHitboxIndex != HITBOX_HEAD)
			continue;

		if (g_Rage->m_bForceBodyaim && hitbox.m_nHitboxIndex == HITBOX_HEAD)
			continue;

		if ((hitbox.m_vCenter - m_vExtrapShootPos).Length() > flWeaponRange * 1.5f)
			continue;

		const Vector vDirToCenter = (hitbox.m_vCenter - m_vExtrapShootPos).Normalized();
		Vector vAxis = hitbox.m_vMaxs - hitbox.m_vMins;
		float flSegmentLength = vAxis.Length();
		Vector vAxisDir = flSegmentLength > 0.f ? (vAxis / flSegmentLength) : Vector{};

		bool bIsHead = (hitbox.m_nHitboxIndex == HITBOX_HEAD);
		BestHitboxPoint_t bestInHitbox = {};

		penetration_data_t pen_data{};
		if (g_Autowall->m_Functions.m_FireBullet.FireBullet(m_vExtrapShootPos, hitbox.m_vCenter, &pen_data, pRecord->m_pPawn)) {

			bool bCanHitVisible = Config::b(g_Variables.m_Ragebot.m_bAutoWall) || !pen_data.m_penetrated;

			if (bCanHitVisible) {
				float flDmgCenter = pen_data.m_damage;
				bool bIsLethal = (flDmgCenter >= pRecord->m_pPawn->m_iHealth());
				bool bMeetsMinDamage = (flDmgCenter >= nDamageThreshold);

				if (bMeetsMinDamage || bIsLethal) {
					bestInHitbox = { hitbox.m_vCenter, flDmgCenter, 1.0f, hitbox.m_flRadius, hitbox, bIsHead, bIsLethal };
				}
			}
		}

		if (hitbox.m_bMultipoint && g_Points->ShouldUseMultipoint(hitbox.m_nHitboxIndex)) {
			auto vecPoints = g_Points->GenerateMultipoints(hitbox);

			for (const auto& vMP : vecPoints) {
				penetration_data_t pen_data_mp{};
				if (g_Autowall->m_Functions.m_FireBullet.FireBullet(m_vExtrapShootPos, vMP, &pen_data_mp, pRecord->m_pPawn)) {

					bool bCanHitMP = Config::b(g_Variables.m_Ragebot.m_bAutoWall) || !pen_data_mp.m_penetrated;

					if (bCanHitMP) {
						float flDmg = pen_data_mp.m_damage;
						bool bIsLethal = (flDmg >= pRecord->m_pPawn->m_iHealth());
						bool bMeetsMinDamage = (flDmg >= nDamageThreshold);

						if (bMeetsMinDamage || bIsLethal) {
							if (flDmg > bestInHitbox.m_flDamage) {
								bestInHitbox = { vMP, flDmg, 1.0f, hitbox.m_flRadius, hitbox, bIsHead, bIsLethal };
							}
						}
					}
				}
			}
		}

		if (bestInHitbox.m_flDamage > 0) {
			if (bIsHead) {
				if (!bHasHeadPoint ||
					bestInHitbox.m_flDamage > bestHeadPoint.m_flDamage ||
					(bestInHitbox.m_flDamage >= nDamageThreshold && bestHeadPoint.m_flDamage < nDamageThreshold) ||
					(bestInHitbox.m_bIsLethal && !bestHeadPoint.m_bIsLethal)) {
					bestHeadPoint = bestInHitbox;
					bHasHeadPoint = true;
				}
			}
			else {
				if (!bHasBodyPoint ||
					bestInHitbox.m_flDamage > bestBodyPoint.m_flDamage ||
					(bestInHitbox.m_flDamage >= nDamageThreshold && bestBodyPoint.m_flDamage < nDamageThreshold) ||
					(bestInHitbox.m_bIsLethal && !bestBodyPoint.m_bIsLethal)) {
					bestBodyPoint = bestInHitbox;
					bHasBodyPoint = true;
				}
			}
		}
	}

	BestHitboxPoint_t globalBest = {};

	if (bHasHeadPoint && bHasBodyPoint) {
		bool bHeadMeetsMinDamage = bestHeadPoint.m_flDamage >= nDamageThreshold;
		bool bBodyMeetsMinDamage = bestBodyPoint.m_flDamage >= nDamageThreshold;

		if (bestHeadPoint.m_bIsLethal) {
			globalBest = bestHeadPoint;
		}
		else if (bestBodyPoint.m_bIsLethal) {
			globalBest = bestBodyPoint;
		}
		else if (bHeadMeetsMinDamage && bBodyMeetsMinDamage) {
			globalBest = bestHeadPoint;
		}
		else if (bHeadMeetsMinDamage) {
			globalBest = bestHeadPoint;
		}
		else if (bBodyMeetsMinDamage) {
			globalBest = bestBodyPoint;
		}
		else {
			globalBest = (bestHeadPoint.m_flDamage > bestBodyPoint.m_flDamage) ? bestHeadPoint : bestBodyPoint;
		}
	}
	else if (bHasHeadPoint) {
		globalBest = bestHeadPoint;
	}
	else if (bHasBodyPoint) {
		globalBest = bestBodyPoint;
	}

	objCandidate.m_vPoint = globalBest.m_vPoint;
	objCandidate.m_flDamage = globalBest.m_flDamage;
	objCandidate.m_hitboxData = globalBest.m_Hitbox;

	return objCandidate;
}

ScanResult_t CScan::ProcessTarget(CRecordTrack* pRecordTrack) {
	CLagRecord* pNewestRecord = pRecordTrack->GetFirstRecord();
	CLagRecord* pOldestRecord = pRecordTrack->GetLastRecord();

	if (!pNewestRecord || !pOldestRecord)
		return {};

	const float flPositionDelta = (pNewestRecord->m_vOrigin - pOldestRecord->m_vOrigin).Length2D();
	const bool bShouldScanBacktrack = flPositionDelta > 0.18f;

	auto frontResult = pNewestRecord->WithAppliedState([&] { return this->ProcessRecord(pNewestRecord); });

	if (!frontResult.HasValidTarget()) {
		return pOldestRecord->WithAppliedState([&] { return this->ProcessRecord(pOldestRecord); });
	}

	return frontResult;
}

ScanResult_t CScan::GetBestTarget() {

	std::vector<ScanResult_t> results(g_Entities->m_vecPlayersOnly.size());

	std::atomic<size_t> resultIndex{ 0 };

	for (size_t i = 0; i < g_Entities->m_vecPlayersOnly.size(); ++i) {
		CRecordTrack* pTrack = g_Entities->m_vecPlayersOnly[i].GetRecordTrack();
		if (!pTrack || !pTrack->m_bRageMatters) continue;
		results[i] = this->ProcessTarget(pTrack);
	}

	g_Threader->WaitUntilFinished();

	ScanResult_t objBestResult{};
	for (const auto& result : results) {
		if (g_Scan->IsBetterCandidate(result, objBestResult)) {
			objBestResult = result;
		}
	}

	objBestResult.m_angToTarget = Math::CalcAngle(m_vShootPos, objBestResult.m_vPoint) - Globals::m_pLocalPlayerPawn->GetRemovedAimPunch();

	return objBestResult;
}

void CRage::HandleKeybinds() {
	m_bHoldingMinOverride = Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_iMinDamageOverrideKey));

	static bool bLastHeadshotState = false;
	static bool bLastBodyaimState = false;

	bool bCurrentHeadshotState = Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_iForceHeadshotKey));
	bool bCurrentBodyaimState = Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_iForceBodyaimKey));

	if (bCurrentHeadshotState && !bLastHeadshotState) {
		m_bForceHeadshot = !m_bForceHeadshot;
		m_bForceBodyaim = false;
	}

	if (bCurrentBodyaimState && !bLastBodyaimState) {
		m_bForceBodyaim = !m_bForceBodyaim;
		m_bForceHeadshot = false;
	}

	bLastHeadshotState = bCurrentHeadshotState;
	bLastBodyaimState = bCurrentBodyaimState;
}

bool CRage::SetConfig()
{
	if (!Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled)) {
		this->m_Data = {};
		return false;
	}

	auto nigger = GetWeaponMenuType((EItemDefinitionIndexes)LocalPlayerData::m_nWeaponDefinitionIndex);
	if (nigger == NONE) {
		this->m_Data = {};
		this->m_bBadWeapon = true;
		return false;
	}

	if (!LocalPlayerData::m_bRageNeedsConfigUpdate)
		return true;

	LocalPlayerData::m_bRageNeedsConfigUpdate = false;

	switch (nigger) {
	case LIGHT_PISTOL:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_LIGHT_PISTOL_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_LIGHT_PISTOL_iMaxPointscale);
		break;
	case DEAGLE:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_DEAGLE_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_DEAGLE_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_DEAGLE_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_DEAGLE_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_DEAGLE_iMaxPointscale);
		break;
	case REVOLVER:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_REVOLVER_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_REVOLVER_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_REVOLVER_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_REVOLVER_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_REVOLVER_iMaxPointscale);
		break;
	case SMG:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_SMG_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_SMG_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_SMG_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_SMG_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_SMG_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_SMG_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_SMG_iMaxPointscale);
		break;
	case LMG:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_LMG_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_LMG_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_LMG_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_LMG_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_LMG_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_LMG_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_LMG_iMaxPointscale);
		break;
	case AR:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_AR_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_AR_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_AR_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_AR_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_AR_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_AR_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_AR_iMaxPointscale);
		break;
	case SHOTGUN:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_SHOTGUN_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_SHOTGUN_iMaxPointscale);
		break;
	case SCOUT:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_SCOUT_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_SCOUT_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_SCOUT_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_SCOUT_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_SCOUT_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_SCOUT_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_SCOUT_iMaxPointscale);
		break;
	case AUTOSNIPER:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_AUTOSNIPER_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_AUTOSNIPER_iMaxPointscale);
		break;
	case AWP:
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchance = Config::i(g_Variables.m_Ragebot.m_AWP_iHitchance);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iHitchanceOverride = Config::i(g_Variables.m_Ragebot.m_AWP_iHitchanceOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage = Config::i(g_Variables.m_Ragebot.m_AWP_iMinDamage);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride = Config::i(g_Variables.m_Ragebot.m_AWP_iMinDamageOverride);
		g_Variables.m_Ragebot.m_ActiveConfig.m_Hitboxes = Config::vhb<MenuRageHitbox_t>(g_Variables.m_Ragebot.m_AWP_vecHitBoxes);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iTargettingType = Config::i(g_Variables.m_Ragebot.m_AWP_iTargetPriority);
		g_Variables.m_Ragebot.m_ActiveConfig.m_iMaxPointscale = Config::i(g_Variables.m_Ragebot.m_AWP_iMaxPointscale);
		break;
	}
	this->m_bBadWeapon = false;
	return true;
}

void CRage::OverrideInputHistory()
{
	if (!this->m_Data.m_LastTarget.HasValidTarget())
		return;

	for (int i = 0; i < Globals::m_pCmd->m_csgoUserCmd.input_history_size(); i++)
	{
		CSGOInputHistoryEntryPB* pInputEntry = Globals::m_pCmd->m_csgoUserCmd.mutable_input_history(i);
		if (!pInputEntry)
			continue;

		pInputEntry->set_player_tick_count(Globals::m_pLocalPlayerController->m_nTickBase());
		pInputEntry->set_render_tick_count(this->m_Data.m_LastTarget.m_pLagRecord->m_nTick + g_LagCompensation->m_InterpolationTiming.m_iTick);

		pInputEntry->set_player_tick_fraction(0.f);
		pInputEntry->set_render_tick_fraction(0.f);

		if (pInputEntry->has_cl_interp())
			pInputEntry->mutable_cl_interp()->set_frac(0.f);

		if (pInputEntry->has_sv_interp0()) {
			pInputEntry->mutable_sv_interp0()->set_frac(0.f);
			pInputEntry->mutable_sv_interp0()->set_src_tick(0);
			pInputEntry->mutable_sv_interp0()->set_dst_tick(0);
		}

		if (pInputEntry->has_sv_interp1()) {
			pInputEntry->mutable_sv_interp1()->set_frac(0.f);
			pInputEntry->mutable_sv_interp1()->set_src_tick(0);
			pInputEntry->mutable_sv_interp1()->set_dst_tick(0);
		}

		pInputEntry->mutable_view_angles()->set_x(this->m_Data.m_LastTarget.m_angToTarget.x);
		pInputEntry->mutable_view_angles()->set_y(this->m_Data.m_LastTarget.m_angToTarget.y);

	}
}

void CRage::ConsiderAttack()
{
	if (!this->m_Data.m_LastTarget.HasValidTarget())
		return;

	if (!this->m_Data.m_bIsAccurate)
		return;

	auto& pCmd = Globals::m_pCmd;

	bool bSilent = Config::b(g_Variables.m_Ragebot.m_bSilentAim);
	bool bAutoShoot = Config::b(g_Variables.m_Ragebot.m_bAutoShoot);

	if (bAutoShoot) {
		penetration_data_t pen_data{};
		if (!g_Autowall->m_Functions.m_FireBullet.FireBullet(m_vExtrapShootPos, this->m_Data.m_LastTarget.m_vPoint, &pen_data, this->m_Data.m_LastTarget.m_pLagRecord->m_pPawn)) {

			return;
		}

		if (!Config::b(g_Variables.m_Ragebot.m_bAutoWall) && pen_data.m_penetrated) {
			return;
		}

		int iMinDamage = m_bHoldingMinOverride
			? g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamageOverride
			: g_Variables.m_Ragebot.m_ActiveConfig.m_iMinDamage;
		if (pen_data.m_damage < iMinDamage) {
			return;
		}
	}

	QAngle angCurrentView = Interfaces::m_pInput->GetViewAngles();

	QAngle angTarget = this->m_Data.m_LastTarget.m_angToTarget;

	angTarget.x = std::clamp(angTarget.x, -89.0f, 89.0f);
	Math::NormalizeAngles(angTarget);

	if (!bSilent) {
		Interfaces::m_pInput->SetViewAngle(angTarget);
	}

	if (bAutoShoot)
	{
		pCmd->m_nButtons.m_nValue |= IN_ATTACK;
		Interfaces::m_pInput->nKeyboardPressed |= IN_ATTACK;

		Globals::m_pCmd->m_csgoUserCmd.mutable_base()->set_client_tick(Globals::m_pLocalPlayerController->m_nTickBase());
		int nEntryIdx = pCmd->m_csgoUserCmd.input_history_size() - 1;
		pCmd->m_csgoUserCmd.set_attack1_start_history_index(nEntryIdx);
	}

	if (bSilent) {
		pCmd->m_csgoUserCmd.mutable_base()->mutable_viewangles()->set_x(angTarget.x);
		pCmd->m_csgoUserCmd.mutable_base()->mutable_viewangles()->set_y(angTarget.y);
	}
	else {

		pCmd->m_csgoUserCmd.mutable_base()->mutable_viewangles()->set_x(angTarget.x);
		pCmd->m_csgoUserCmd.mutable_base()->mutable_viewangles()->set_y(angTarget.y);
	}
}

void CRage::AutoRevolver() {
	if (!Globals::m_pLocalPlayerPawn || !Globals::m_pCmd)
		return;

	if (!Config::b(g_Variables.m_Ragebot.m_bAutoRevolver))
		return;

	CPlayer_WeaponServices* pWeaponServices = Globals::m_pLocalPlayerPawn->m_pWeaponServices();
	if (!pWeaponServices)
		return;

	CCSPlayer_WeaponServices* pPlayerWeaponServices = static_cast<CCSPlayer_WeaponServices*>(Globals::m_pLocalPlayerPawn->m_pWeaponServices());
	if (!pPlayerWeaponServices)
		return;

	C_CSWeaponBase* m_pWeapon = pWeaponServices->m_hActiveWeapon().Get();
	if (!m_pWeapon)
		return;

	C_EconItemView* item = m_pWeapon->m_AttributeManager()->m_Item();
	if (!item)
		return;

	if (item->m_iItemDefinitionIndex() != WEAPON_R8_REVOLVER)
		return;

	auto time = TICKS_TO_TIME(Globals::m_pLocalPlayerController->m_nTickBase() + 1);

	if (time >= pPlayerWeaponServices->m_flNextAttack()) {
		auto secondTick = time >= TICKS_TO_TIME(m_pWeapon->m_nNextPrimaryAttackTick()) && time > TICKS_TO_TIME(m_pWeapon->m_nPostponeFireReadyTicks());

		if (secondTick && time < TICKS_TO_TIME(m_pWeapon->m_nNextSecondaryAttackTick()))
			Globals::m_pCmd->m_nButtons.m_nValue |= IN_ATTACK2;
		else if (!secondTick)
			Globals::m_pCmd->m_nButtons.m_nValue |= IN_ATTACK;
	}

	if (!m_pWeapon->m_iClip1() && m_pWeapon->m_pReserveAmmo() > 0) {
		Globals::m_pCmd->m_nButtons.m_nValue &= ~IN_ATTACK;
		Globals::m_pCmd->m_nButtons.m_nValue |= IN_RELOAD;
	}
}

void CRage::DrawRageFov()
{

	if (!Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled))
		return;

	if (!Config::b(g_Variables.m_Ragebot.m_bDrawFov))
		return;

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return;

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

	float flFov = Config::f(g_Variables.m_Ragebot.m_flFov);
	if (flFov <= 0.f || flFov >= 180.f)
		return;

	float flGameFov = 90.0f;
	float radius = tanf(flFov * 0.5f * M_PI / 180.0f) / tanf(flGameFov * 0.5f * M_PI / 180.0f) * (io.DisplaySize.x * 0.5f);

	auto pDrawList = ImGui::GetBackgroundDrawList();
	if (pDrawList) {

		pDrawList->AddCircle(center, radius, IM_COL32(255, 255, 255, 180), 100, 1.0f);
	}
}

void CRage::DrawAimbotTarget() {
	if (!Config::b(g_Variables.m_Ragebot.m_bDrawTarget))
		return;

	if (ImGui::GetCurrentContext() == nullptr)
		return;

	if (!Interfaces::m_pEngine->IsInGame() || !Globals::m_pLocalPlayerPawn)
		return;

	if (!Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled))
		return;

	auto& lastTarget = this->m_Data.m_LastTarget;

	if (!lastTarget.HasValidTarget() || lastTarget.m_vPoint.IsZero())
		return;

	if (!lastTarget.m_pLagRecord || !lastTarget.m_pLagRecord->m_pPawn)
		return;

	ImVec2 vecScreenPos;
	if (!Draw::WorldToScreen(lastTarget.m_vPoint, vecScreenPos))
		return;

	ImVec2 displaySize = ImGui::GetIO().DisplaySize;

	if (!std::isfinite(vecScreenPos.x) || !std::isfinite(vecScreenPos.y))
		return;

	if (vecScreenPos.x < 0 || vecScreenPos.x > displaySize.x || vecScreenPos.y < 0 || vecScreenPos.y > displaySize.y)
		return;

	ImDrawList* pBackgroundDrawList = ImGui::GetBackgroundDrawList();
	if (!pBackgroundDrawList)
		return;

	const float radius = 3.5f;
	const ImU32 colTarget = IM_COL32(255, 0, 0, 76);

	try {

		pBackgroundDrawList->AddCircleFilled(vecScreenPos, radius, colTarget, 12);
	}
	catch (...) {

		return;
	}
}

void CRage::OnCreateMove()
{
	static int s_lastTargetTick = 0;
	static bool s_hadTargetLastFrame = false;

	if (!Config::b(g_Variables.m_Ragebot.m_bRagebotEnabled) || this->m_bBadWeapon)
	{

		this->m_Data.m_LastTarget = {};
		this->m_Data.m_bIsAccurate = false;
		this->m_Data.m_bShouldInvalidateAngle = false;
		this->m_Data.m_bWants2DMoveHalt = false;
		s_hadTargetLastFrame = false;
		return;
	}

	if (!LocalPlayerData::m_pWeapon || !LocalPlayerData::m_pWeaponBaseVData)
		return;

	AutoRevolver();

	static bool s_bDuckPeekActive = false;
	static bool s_bWasEnemyVisible = false;

	if(Config::b(g_Variables.m_Ragebot.m_bDuckPeekAssist) && Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_kbDuckPeekAssist)))
	{
		s_bDuckPeekActive = true;

		bool bEnemyVisible = false;

		Vector vecStandingEyePos = LocalPlayerData::m_vecEyePosition;
		if (auto pMove = Globals::m_pLocalPlayerPawn->m_pMovementServices())
		{
			float flDuckAmount = pMove->m_flDuckAmount();
			if (flDuckAmount > 0.0f)
				vecStandingEyePos.z += 18.0f * flDuckAmount;
		}

		for (const auto& trackedPlayer : g_Entities->m_vecPlayersOnly)
		{
			CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(trackedPlayer.m_pEntity);
			if (!pController || !pController->m_bPawnIsAlive())
				continue;

			C_CSPlayerPawn* pPlayer = pController->m_hPlayerPawn().Get();
			if (!pPlayer || pPlayer == Globals::m_pLocalPlayerPawn)
				continue;

			if (!Globals::m_pLocalPlayerPawn->IsEnemy(pPlayer))
				continue;

			if (!pPlayer->m_pGameSceneNode())
				continue;

			CSkeletonInstance* pSkeleton = pPlayer->m_pGameSceneNode()->GetSkeletonInstance();
			if (!pSkeleton || !pSkeleton->m_pBoneCache)
				continue;

			penetration_data_t pen_data{};
			Vector vecTargetPos = Vector(0, 0, 0);

			const int nChestBoneIndex = static_cast<int>(EHitBoxes::HITBOX_CHEST);
			if (nChestBoneIndex >= 0 && nChestBoneIndex < 128) {
				vecTargetPos = pPlayer->m_pGameSceneNode()->GetBonePosition(nChestBoneIndex);
			}

			if (!vecTargetPos.IsValid() || vecTargetPos.IsZero())
				continue;

			if (g_Autowall->m_Functions.m_FireBullet.FireBullet(vecStandingEyePos, vecTargetPos, &pen_data, pPlayer))
			{
				if (pen_data.m_damage > 0)
				{
					bEnemyVisible = true;
					break;
				}
			}
		}

		int nCurrentTick = Interfaces::m_pGlobalVariables->m_nTickCount;
		if (nCurrentTick - CRage::s_nDuckPeekKillTick < 32)
		{
			Globals::m_pCmd->m_nButtons.m_nValue |= IN_DUCK;
			s_bWasEnemyVisible = false;
		}
		else if (bEnemyVisible)
		{
			Globals::m_pCmd->m_nButtons.m_nValue &= ~IN_DUCK;
			s_bWasEnemyVisible = true;
		}
		else
		{
			Globals::m_pCmd->m_nButtons.m_nValue |= IN_DUCK;
			s_bWasEnemyVisible = false;
		}
	}
	else
	{
		s_bDuckPeekActive = false;
		s_bWasEnemyVisible = false;
	}

	if (Globals::m_pCmd->m_nCommandNumber == g_Scan->m_nLastSequenceScan)
		return;

	g_Scan->m_nLastSequenceScan = Globals::m_pCmd->m_nCommandNumber;

	if (!Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_iForceHeadshotKey))) {
		m_bForceHeadshot = false;
	}

	if (!Input::HandleInput(Config::kb(g_Variables.m_Ragebot.m_iForceBodyaimKey))) {
		m_bForceBodyaim = false;
	}

	if (LocalPlayerData::m_pWeapon->m_nNextPrimaryAttackTick() > Interfaces::m_pGlobalVariables->m_nTickCount)
	{

		this->m_Data.m_LastTarget = {};
		this->m_Data.m_bIsAccurate = false;
		this->m_Data.m_bShouldInvalidateAngle = false;
		return;
	}

	bool bShouldExtrap = Globals::m_pLocalPlayerPawn->m_vecAbsVelocity().Length2D() > LocalPlayerData::m_pWeapon->GetMaxSpeed() * 0.34f;
	m_vExtrapShootPos = bShouldExtrap ? g_Movement->SimulatePosAtStop(m_vShootPos) : m_vShootPos;

	this->m_Data.m_LastTarget = g_Scan->GetBestTarget();

	QAngle m_vecAimAngle;
	Vector m_Delta = g_Rage->m_Data.m_LastTarget.m_vPoint - LocalPlayerData::m_vecEyePosition;
	Math::VectorAngles(m_Delta, m_vecAimAngle);
	m_vecAimAngle.z = 0;

	if (!this->m_Data.m_LastTarget.HasValidTarget())
	{

		this->m_Data.m_LastTarget = {};
		this->m_Data.m_bIsAccurate = false;
		this->m_Data.m_bShouldInvalidateAngle = false;

		if (s_hadTargetLastFrame) {
			s_lastTargetTick = Interfaces::m_pGlobalVariables->m_nTickCount;
			s_hadTargetLastFrame = false;
		}

		int currentTick = Interfaces::m_pGlobalVariables->m_nTickCount;
		int ticksElapsed = currentTick - s_lastTargetTick;
		if (ticksElapsed > 13) {
			this->m_Data.m_bWants2DMoveHalt = false;
		}

		return;
	}

	s_hadTargetLastFrame = true;

	this->m_Data.m_bIsAccurate = this->m_Data.m_LastTarget.m_pLagRecord->WithAppliedState(([&] { return g_Accuracy->IsAccurate(); }));
	this->m_Data.m_bShouldInvalidateAngle = !this->m_Data.m_LastTarget.m_pLagRecord->WantsLagCompensationOnEntity(LocalPlayerData::m_vecAbsOrigin, g_Movement->m_angModelAngles);

	bool bShouldAutostop = false;
	if (Config::b(g_Variables.m_Ragebot.m_bAutostop) && this->m_Data.m_LastTarget.HasValidTarget()) {
		float flCurrentSpeed = Globals::m_pLocalPlayerPawn->m_vecAbsVelocity().Length2D();
		float flMaxSpeed = LocalPlayerData::m_pWeapon->GetMaxSpeed();

		float flMinSpeedThreshold = flMaxSpeed * 0.20f;

		bShouldAutostop = (flCurrentSpeed > flMinSpeedThreshold);

	}
	this->m_Data.m_bWants2DMoveHalt = bShouldAutostop;
}
