#include "../Precompiled.h"
#include "misc/ShotHandler.h"
#include "misc/misc.h"

static bool CompareVectors(const Vector& a, const Vector& b)
{
	return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

std::vector<Vector> ConvexHull(std::vector<Vector> points)
{
	if (points.size() < 3)
		return points;

	std::sort(points.begin(), points.end(), CompareVectors);

	std::vector<Vector> hull;

	auto crossProduct = [](const Vector& O, const Vector& A, const Vector& B)
		{
			return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
		};

	for (const auto& p : points)
	{
		while (hull.size() >= 2 && crossProduct(hull[hull.size() - 2], hull.back(), p) <= 0)
		{
			hull.pop_back();
		}
		hull.push_back(p);
	}

	size_t lowerHullSize = hull.size();
	for (int i = points.size() - 2; i >= 0; i--)
	{
		while (hull.size() > lowerHullSize && crossProduct(hull[hull.size() - 2], hull.back(), points[i]) <= 0)
		{
			hull.pop_back();
		}
		hull.push_back(points[i]);
	}

	if (!hull.empty())
		hull.pop_back();

	return hull;
}

void DrawInferno(C_Inferno* pInferno)
{
	if (!pInferno)
		return;

	std::vector<Vector> vecPoints;
	vecPoints.reserve(pInferno->m_fireCount());

	for (int i = 0; i < pInferno->m_fireCount(); ++i)
	{
		if (pInferno->m_bFireIsBurning()[i])
			vecPoints.emplace_back(pInferno->m_firePositions()[i]);
	}

	if (vecPoints.empty())
		return;

	constexpr float radius = 60.0f;
	constexpr int numSegments = 64;
	constexpr float angleStep = (2.0f * M_PI) / numSegments;

	std::vector<Vector> vecNewPoints;
	vecNewPoints.reserve(vecPoints.size() * numSegments);

	for (const auto& pos : vecPoints)
	{
		for (int j = 0; j < numSegments; ++j)
		{
			float angle = j * angleStep;
			vecNewPoints.emplace_back(pos.x + std::cos(angle) * radius,
				pos.y + std::sin(angle) * radius,
				pos.z);
		}
	}

	std::vector<Vector> vecHullPoints = ConvexHull(vecNewPoints);

	if (vecHullPoints.empty())
		return;

	std::vector<ImVec2> vec2DPoints;
	vec2DPoints.reserve(vecHullPoints.size());

	for (const auto& vecPosition : vecHullPoints)
	{
		ImVec2 vecScreenPosition;
		if (Draw::WorldToScreen(vecPosition, vecScreenPosition))
			vec2DPoints.emplace_back(vecScreenPosition);
	}

	if (!vec2DPoints.empty())
		Draw::AddPolygon(vec2DPoints, Config::c(g_Variables.m_WorldVisuals.m_colInferno));
}

static int GetInt2(CGameEvent* pEvent, const char* szEvent)
{
	using func_t = int(__fastcall*)(CGameEvent*, const char*, bool);
	static func_t fn = (func_t)Memory::FindPattern(CLIENT_DLL, X("48 89 5C 24 08 48 89 74 24 10 48 89 7C 24 18 41 56 48 83 EC 30 48 8B 01 41 8B F0 4C 8B F1 41 B0 01 48 8D 4C 24 20 48 8B DA 48 8B 78"));
	return fn(pEvent, szEvent, false);
}

void Features::OnFireEvent(CGameEvent* pEvent, bool bServerSideOnly)
{

	FNV1A_t szEventName = FNV1A::Hash(pEvent->GetName());
	switch (szEventName)
	{

		case FNV1A::HashConst("bullet_impact"):
		{
			g_ShotHandler->OnBulletImpact(pEvent);
		}

		case FNV1A::HashConst("player_hurt"):
		{

			g_ShotHandler->OnPlayerHurt(pEvent);

		}

		case FNV1A::HashConst("weapon_fire"):
		{
			g_ShotHandler->OnWeaponFire(pEvent);
		}

	}
}

constexpr float HITMARKER_TIME = 4.f;

void Features::DrawHitmarkers()
{
	if (!Interfaces::m_pGlobalVariables || m_deqHitmarkerPositions.empty())
		return;

	if (!Config::b(g_Variables.m_Visuals.m_bHitmarker))
		return;

	float flCurTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;

	m_deqHitmarkerPositions.erase(std::remove_if(
		m_deqHitmarkerPositions.begin(), m_deqHitmarkerPositions.end(),
		[flCurTime](const auto& hitmarker)
		{
			return flCurTime > hitmarker.m_flTimeAdded + HITMARKER_TIME;
		}),
		m_deqHitmarkerPositions.end());

	for (const auto& hitmarker : m_deqHitmarkerPositions)
	{
		ImVec2 vecScreenPos;
		if (!Draw::WorldToScreen(hitmarker.m_vecWorldPosition, vecScreenPos))
			continue;

		float flLength = 8.0f;
		float flGap = 4.0f;

		Color colLine = Config::c(g_Variables.m_Visuals.m_colHitmarker);

		auto DrawLineWrapper = [&](ImVec2 start, ImVec2 end)
			{
				Draw::AddLine(start, end, colLine, 1.0f);
			};

		DrawLineWrapper(
			ImVec2(vecScreenPos.x - flLength, vecScreenPos.y - flLength),
			ImVec2(vecScreenPos.x - flGap, vecScreenPos.y - flGap)
		);

		DrawLineWrapper(
			ImVec2(vecScreenPos.x + flLength, vecScreenPos.y - flLength),
			ImVec2(vecScreenPos.x + flGap, vecScreenPos.y - flGap)
		);

		DrawLineWrapper(
			ImVec2(vecScreenPos.x - flLength, vecScreenPos.y + flLength),
			ImVec2(vecScreenPos.x - flGap, vecScreenPos.y + flGap)
		);

		DrawLineWrapper(
			ImVec2(vecScreenPos.x + flLength, vecScreenPos.y + flLength),
			ImVec2(vecScreenPos.x + flGap, vecScreenPos.y + flGap)
		);
	}
}

void Features::DrawSpeedometer()
{
	if (!Config::b(g_Variables.m_Visuals.m_bSpeedometer))
		return;

	if (!Globals::m_pLocalPlayerPawn || !Globals::m_pLocalPlayerController)
		return;

	if (!Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return;

	Vector vecVelocity = LocalPlayerData::m_vecVelocity;
	float flSpeed = vecVelocity.Length2D();

	int iSize = Config::i(g_Variables.m_Visuals.m_iSpeedometerSize);
	int iMaxSpeed = Config::i(g_Variables.m_Visuals.m_iSpeedometerMaxSpeed);

	Color colSpeedometer(
		static_cast<uint8_t>(g_AccentColor.x * 255.0f),
		static_cast<uint8_t>(g_AccentColor.y * 255.0f),
		static_cast<uint8_t>(g_AccentColor.z * 255.0f),
		static_cast<uint8_t>(255)
	);
	Color colBackground(0, 0, 0, 150);

	ImVec2 vecScreenSize = ImGui::GetIO().DisplaySize;
	ImVec2 vecCenter = ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y - static_cast<float>(iSize) - 30.0f);
	float flRadius = static_cast<float>(iSize);

	Draw::AddCircle(vecCenter, flRadius, colBackground, 32, DRAW_CIRCLE_FILLED);

	Draw::AddCircle(vecCenter, flRadius, colSpeedometer, 32, DRAW_CIRCLE_OUTLINE, Color(0, 0, 0, 255), 2.0f);

	float flSpeedRatio = std::clamp(flSpeed / static_cast<float>(iMaxSpeed), 0.0f, 1.0f);
	float flAngle = (flSpeedRatio * 270.0f) - 135.0f;
	float flAngleRad = M_DEG2RAD(flAngle);

	float flStartAngle = M_DEG2RAD(-135.0f);
	int nSegments = static_cast<int>(flSpeedRatio * 64.0f);

	for (int i = 0; i <= nSegments; i++)
	{
		float flCurrentAngle = flStartAngle + (flAngleRad - flStartAngle) * (static_cast<float>(i) / static_cast<float>(nSegments));
		float flX1 = vecCenter.x + cosf(flCurrentAngle) * (flRadius - 5.0f);
		float flY1 = vecCenter.y + sinf(flCurrentAngle) * (flRadius - 5.0f);
		float flX2 = vecCenter.x + cosf(flCurrentAngle) * flRadius;
		float flY2 = vecCenter.y + sinf(flCurrentAngle) * flRadius;

		Draw::AddLine(ImVec2(flX1, flY1), ImVec2(flX2, flY2), colSpeedometer, 2.0f);
	}

	float flNeedleLength = flRadius * 0.7f;
	float flNeedleX = vecCenter.x + cosf(flAngleRad) * flNeedleLength;
	float flNeedleY = vecCenter.y + sinf(flAngleRad) * flNeedleLength;

	Draw::AddLine(vecCenter, ImVec2(flNeedleX, flNeedleY), colSpeedometer, 3.0f);

	Draw::AddCircle(vecCenter, 3.0f, colSpeedometer, 8, DRAW_CIRCLE_FILLED);

	char szSpeedText[32];
	sprintf_s(szSpeedText, sizeof(szSpeedText), "%.0f", flSpeed);
	ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, szSpeedText);
	ImVec2 vecTextPos = ImVec2(vecCenter.x - vecTextSize.x * 0.5f, vecCenter.y + flRadius * 0.3f);

	Draw::AddText(Fonts::ESP, 10.0f, vecTextPos, szSpeedText, colSpeedometer, DRAW_TEXT_OUTLINE, Color(0, 0, 0, 255));

	char szMaxSpeedText[32];
	sprintf_s(szMaxSpeedText, sizeof(szMaxSpeedText), "%d", iMaxSpeed);
	ImVec2 vecMaxTextSize = Fonts::ESP->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, szMaxSpeedText);
	ImVec2 vecMaxTextPos = ImVec2(vecCenter.x - vecMaxTextSize.x * 0.5f, vecCenter.y - flRadius - 15.0f);

	Color colMaxSpeedText(colSpeedometer.r(), colSpeedometer.g(), colSpeedometer.b(), static_cast<uint8_t>(150));
	Draw::AddText(Fonts::ESP, 10.0f, vecMaxTextPos, szMaxSpeedText, colMaxSpeedText, DRAW_TEXT_OUTLINE, Color(0, 0, 0, 255));
}

void Features::OnRenderStart(CViewRender* pViewRender)
{
	if (Draw::m_bInitialized)
	{

		Draw::ClearDrawData();

		if (Interfaces::m_pEngine->IsConnected() && Interfaces::m_pEngine->IsInGame())
		{
			if (Globals::m_pLocalPlayerController && Globals::m_pLocalPlayerPawn && Interfaces::m_pGlobalVariables)
			{

				if (InventoryChanger::m_vecDumpedItems.empty() && InventoryChanger::m_vecDumpedAgents.empty())
					InventoryChanger::DumpAllSkins();

				if (Config::b(g_Variables.m_Visuals.m_bHitmarker) && Globals::m_pLocalPlayerController && Globals::m_pLocalPlayerController->m_bPawnIsAlive() && Globals::m_pLocalPlayerPawn) {
					Features::DrawHitmarkers();
				}

				if (Config::b(g_Variables.m_Visuals.m_bSpeedometer) && Globals::m_pLocalPlayerController && Globals::m_pLocalPlayerController->m_bPawnIsAlive() && Globals::m_pLocalPlayerPawn) {
					Features::DrawSpeedometer();
				}

				if (Config::b(g_Variables.m_Misc.m_bDotPenetration) && Globals::m_pLocalPlayerController && Globals::m_pLocalPlayerController->m_bPawnIsAlive() && Globals::m_pLocalPlayerPawn) {
					g_Misc->DotPenetration();
				}

				if (Config::b(g_Variables.m_WorldVisuals.m_bLSDMode)) {
					g_Misc->LSDMode();
				}

				if (Config::b(g_Variables.m_Misc.m_bDebug))
				{
					for (Vector& vecHitmarkerPosition : NRagebot::debugPoints) {
						ImVec2 asf;
						if (Draw::WorldToScreen(vecHitmarkerPosition, asf)) {
							Draw::AddCircle(asf, 2.4, Color(255, 0, 0));
						}
					}
					ImVec2 predeyepos;
					bool bWorldToScreenResult = Draw::WorldToScreen(NRagebot::m_vExtrapShootPos, predeyepos);
					if (bWorldToScreenResult) {
						Draw::AddCircle(predeyepos, 5.f, Color(255, 122, 122));
					}
					ImVec2 actualeyepos;
					bool j = Draw::WorldToScreen(NRagebot::m_vShootPos, actualeyepos);
					if (j) {
						Draw::AddCircle(actualeyepos, 5.f, Color(255, 122, 122));
					}
				}

				if (Config::b(g_Variables.m_Legitbot.m_bEnabled) && Globals::m_pLocalPlayerPawn)
				{
					ImVec2 vScreenSize = ImGui::GetIO().DisplaySize;
					ImVec2 vScreenCenter = ImVec2(vScreenSize.x * 0.5f, vScreenSize.y * 0.5f);

					QAngle aAimPunch = Globals::m_pLocalPlayerPawn->GetRemovedAimPunch();
					float flGameFOV = static_cast<float>(Config::i(g_Variables.m_Visuals.m_iWorldFov));

					ImVec2 vAimPunchCenter = {
						vScreenCenter.x - (aAimPunch.y * (vScreenSize.x / flGameFOV) * 0.5f),
						vScreenCenter.y + (aAimPunch.x * (vScreenSize.y / flGameFOV) * 0.5f)
					};

					int iFovValue = 10;
					if (LocalPlayerData::m_pWeapon)
					{
						EMenuWeaponType eWeaponType = GetWeaponMenuType((EItemDefinitionIndexes)LocalPlayerData::m_nWeaponDefinitionIndex);
						if (eWeaponType != EMenuWeaponType::NONE)
						{
							switch (eWeaponType)
							{
							case EMenuWeaponType::LIGHT_PISTOL:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_LIGHT_PISTOL_iFov);
								break;
							case EMenuWeaponType::DEAGLE:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_DEAGLE_iFov);
								break;
							case EMenuWeaponType::REVOLVER:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_REVOLVER_iFov);
								break;
							case EMenuWeaponType::SMG:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_SMG_iFov);
								break;
							case EMenuWeaponType::LMG:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_LMG_iFov);
								break;
							case EMenuWeaponType::AR:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_AR_iFov);
								break;
							case EMenuWeaponType::SHOTGUN:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_SHOTGUN_iFov);
								break;
							case EMenuWeaponType::SCOUT:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_SCOUT_iFov);
								break;
							case EMenuWeaponType::AUTOSNIPER:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_AUTOSNIPER_iFov);
								break;
							case EMenuWeaponType::AWP:
								iFovValue = Config::i(g_Variables.m_Legitbot.m_AWP_iFov);
								break;
							default:
								break;
							}
						}
					}

					if (iFovValue == -1)
					{

						const float flMinFov = 10.0f;
						const float flMaxFov = 100.0f;
						const float flMaxSpeed = 250.0f;

						Vector vecVelocity = Globals::m_pLocalPlayerPawn->m_vecVelocity();
						float flSpeed = vecVelocity.Length2D();

						float flSpeedRatio = std::clamp(flSpeed / flMaxSpeed, 0.0f, 1.0f);
						float flCalculatedFov = flMinFov + (flMaxFov - flMinFov) * flSpeedRatio;
						iFovValue = static_cast<int>(flCalculatedFov);
					}

					float flLegitbotFov = static_cast<float>(iFovValue) / 4.f;
					float flCircleRadius = (flLegitbotFov / flGameFOV) * (vScreenSize.x / 2.f);

					Draw::AddCircle(vAimPunchCenter, flCircleRadius, Color(255, 255, 255, 255), 100);
				}

				g_GrenadePrediction->Run();

				if (Config::b(g_Variables.m_Visuals.m_bEnableOnShotSkeletonESP) && !s_vecOnShotSkeletons.empty())
				{
					float flDuration = static_cast<float>(Config::i(g_Variables.m_Visuals.m_nHitEffectsDurration));
					float flCurrentTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;

					for (size_t i = 0; i < s_vecOnShotSkeletons.size();)
					{
						auto& obj = s_vecOnShotSkeletons[i];

						if ((flCurrentTime - obj.m_flTimeAdded) > flDuration)
						{
							s_vecOnShotSkeletons.erase(s_vecOnShotSkeletons.begin() + i);
							continue;
						}

						PlayerESP::DrawSkeleton(
							obj.m_pController,
							obj.m_pPawn,
							{ },
							{ },
							Config::c(g_Variables.m_Visuals.m_colOnShotSkeletonBones),
							{ },
							obj.m_Matrix
						);

						++i;
					}
				}

				for (auto& player : g_Entities->m_vecPlayersOnly) {
					CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(player.m_pEntity);
					if (!pController)
						continue;

					C_CSPlayerPawn* pPawn = pController->m_hPlayerPawn().Get();
					if (!pPawn)
						continue;

					CGameSceneNode* pGameSceneNode = pPawn->m_pGameSceneNode();
					if (!pGameSceneNode || !pGameSceneNode->GetSkeletonInstance())
						continue;

					PlayerESP::Run(pController, pPawn, player.GetRecordTrack());

				}

				for ( EntityObject_t& object : g_Entities->m_vecEntities )
				{
					switch (object.m_eType)
					{

					case ENTITY_GRENADE:
					{
						GrenadeESP::Draw(object);

						C_BaseCSGrenadeProjectile* pBaseGrenadeProj = (C_BaseCSGrenadeProjectile*)object.m_pEntity;
						if ( !pBaseGrenadeProj )
							break;

						SchemaClassInfoData_t* pGrenadeSchema;
						object.m_pEntity->GetSchemaClassInfo( &pGrenadeSchema );
						if ( !pGrenadeSchema )
							break;

						FNV1A_t uHashedName = FNV1A::Hash( pGrenadeSchema->szName );

						if ( uHashedName == FNV1A::HashConst( "C_SmokeGrenadeProjectile" ) ||
							uHashedName == FNV1A::HashConst( "C_FlashbangProjectile" ) ||
							uHashedName == FNV1A::HashConst( "C_HEGrenadeProjectile" ) ||
							uHashedName == FNV1A::HashConst( "C_MolotovProjectile" ) ||
							uHashedName == FNV1A::HashConst( "C_DecoyProjectile" ) )
						{
							if ( object.m_bPredictedGrenade )
								break;

							object.m_bPredictedGrenade = true;

							GrenadePredictionObject_t objGrenadeObject = {  };
							objGrenadeObject.m_pGrenadeEntity = object.m_pEntity;
							objGrenadeObject.m_vInitialPosition = pBaseGrenadeProj->m_vInitialPosition( );
							objGrenadeObject.m_vInitialVelocity = pBaseGrenadeProj->m_vInitialVelocity( );

							std::vector<GrenadePathPoint_t> vecGrenadePathPoint = g_GrenadePrediction->GetGrenadePathFromEntity(
								objGrenadeObject.m_vInitialPosition,
								objGrenadeObject.m_vInitialVelocity,
								(C_BaseCSGrenade*)pBaseGrenadeProj,
								Interfaces::m_pGlobalVariables->m_flCurrentTime
							);

							objGrenadeObject.m_GrenadePathPoint = vecGrenadePathPoint;

							s_vecPredictedGrenades.emplace_back( objGrenadeObject );
						}

						break;
					}

					case ENTITY_INFERNO:
					{
						C_Inferno* pInferno = reinterpret_cast<C_Inferno*>(object.m_pEntity);
						if (!pInferno) continue;

						if (!Config::b(g_Variables.m_WorldVisuals.m_bDrawInferno))
							continue;

						DrawInferno(pInferno);
						break;
					}

					case ENTITY_WEAPON:
					{
						if (!Config::b(g_Variables.m_WorldVisuals.m_bWeaponNames))
							continue;

						C_CSWeaponBaseGun* pWeapon = reinterpret_cast<C_CSWeaponBaseGun*>(object.m_pEntity);
						if (!pWeapon)
							continue;

						CGameSceneNode* pGameSceneNode = pWeapon->m_pGameSceneNode();
						if (!pGameSceneNode || !pGameSceneNode->GetSkeletonInstance())
							continue;

						Vector pos = pGameSceneNode->m_vecAbsOrigin();
						ImVec2 vecScreen;
						if (!Draw::WorldToScreen(pos, vecScreen))
							continue;

						SchemaClassInfoData_t* weaponSchema = nullptr;
						pWeapon->GetSchemaClassInfo(&weaponSchema);
						if (!weaponSchema || !weaponSchema->szName)
							continue;

						const char* text = weaponSchema->szName;
						if (!text || strlen(text) == 0)
							continue;

						Draw::AddText(Fonts::ESP, 10.0f, ImVec2(vecScreen.x - (ImGui::CalcTextSize(text).x / 2), vecScreen.y), text, Color(255, 255, 255, 255), DRAW_TEXT_OUTLINE);

						break;
					}

					default:
						break;
					}
				}
			}
		}

		Draw::SwapDrawData();
	}
}
