#include "../../Precompiled.h"
#include <unordered_map>

inline bool GetEntityBoundingBox(C_BaseEntity* pEntity, Box_t* pBox)
{
	if (!pEntity)
		return false;

	CCollisionProperty* pCollision = pEntity->m_pCollision();
	if (pCollision == nullptr)
		return false;

	CGameSceneNode* pGameSceneNode = pEntity->m_pGameSceneNode();
	if (pGameSceneNode == nullptr)
		return false;

	const CTransform nodeToWorldTransform = pGameSceneNode->m_nodeToWorld();
	const Matrix3x4_t matTransform = nodeToWorldTransform.quatOrientation.ToMatrix(nodeToWorldTransform.vecPosition);

	const Vector vecMins = pCollision->m_vecMins();
	const Vector vecMaxs = pCollision->m_vecMaxs();

	float flLeft = std::numeric_limits<float>::max();
	float flTop = std::numeric_limits<float>::max();
	float flRight = std::numeric_limits<float>::lowest();
	float flBottom = std::numeric_limits<float>::lowest();

	for (int i = 0; i < 8; ++i)
	{
		const Vector vecPoint
		{
			i & 1 ? vecMaxs.x : vecMins.x,
			i & 2 ? vecMaxs.y : vecMins.y,
			i & 4 ? vecMaxs.z : vecMins.z
		};

		const Vector vecTransformed = vecPoint.Transform(matTransform);
		ImVec2 vecScreen;
		if (!Draw::WorldToScreen(vecTransformed, vecScreen))
			return false;

		flLeft = std::min(flLeft, vecScreen.x);
		flTop = std::min(flTop, vecScreen.y);
		flRight = std::max(flRight, vecScreen.x);
		flBottom = std::max(flBottom, vecScreen.y);
	}

	pBox->left = flLeft;
	pBox->top = flTop;
	pBox->right = flRight;
	pBox->bottom = flBottom;
	pBox->width = flRight - flLeft;
	pBox->height = flBottom - flTop;
	return true;
}

ImVec2 RotateVertex(const Vector& p, const ImVec2& v, float angle)
{
	float c = std::cos(M_DEG2RAD(angle));
	float s = std::sin(M_DEG2RAD(angle));

	return {
		p.x + (v.x - p.x) * c - (v.y - p.y) * s,
		p.y + (v.x - p.x) * s + (v.y - p.y) * c
	};
}

void DrawOOF(C_CSPlayerPawn* pPawn) {

	if (!pPawn->m_pGameSceneNode())
		return;

	ImColor triangleColor = Config::c(g_Variables.m_Visuals.m_colOOFArrows).GetVec4();

	float baseSize = 8.0f;
	float screenDistance = 150.0f;

	Vector vecViewOrigin, vecTargetPos, vecDelta;
	ImVec2 vecScreenPos;
	Vector vecOffScreenPos;
	float flLeeWayX, flLeeWayY, flRadius, flOffScreenRotation;
	bool bIsOnScreen;
	std::array<ImVec2, 3> arrVecVerts;

	auto GetOffScreenData = [](const Vector& vecDelta, float flRadiusX, float flRadiusY, Vector& vecOutOffScreenPos, float& flOutRot) {
		QAngle view_angles(Interfaces::m_pInput->GetViewAngles());
		Vector fwd, right, up(0.f, 0.f, 1.f);

		Math::AngleVector(view_angles, fwd);
		fwd.z = 0.f;
		fwd.Normalize();

		right = up.CrossProduct(fwd);
		float front = vecDelta.DotProduct(fwd);
		float side = vecDelta.DotProduct(right);

		vecOutOffScreenPos.x = flRadiusX * -side;
		vecOutOffScreenPos.y = flRadiusY * -front;

		flOutRot = M_RAD2DEG(std::atan2(vecOutOffScreenPos.x, vecOutOffScreenPos.y) + M_PI);

		float yaw_rad = M_DEG2RAD(-flOutRot);
		float sa = std::sin(yaw_rad);
		float ca = std::cos(yaw_rad);

		vecOutOffScreenPos.x = (ImGui::GetIO().DisplaySize.x / 2.f) + (flRadiusX * sa);
		vecOutOffScreenPos.y = (ImGui::GetIO().DisplaySize.y / 2.f) - (flRadiusY * ca);
		};

	vecTargetPos = pPawn->m_pGameSceneNode()->m_vecAbsOrigin();
	bIsOnScreen = Draw::WorldToScreen(vecTargetPos, vecScreenPos);

	flLeeWayX = ImGui::GetIO().DisplaySize.x / 18.f;
	flLeeWayY = ImGui::GetIO().DisplaySize.y / 18.f;

	if (!bIsOnScreen ||
		vecScreenPos.x < -flLeeWayX ||
		vecScreenPos.x >(ImGui::GetIO().DisplaySize.x + flLeeWayX) ||
		vecScreenPos.y < -flLeeWayY ||
		vecScreenPos.y >(ImGui::GetIO().DisplaySize.y + flLeeWayY)) {

		vecViewOrigin = Globals::m_pLocalPlayerPawn->m_pGameSceneNode()->m_vecAbsOrigin();
		vecDelta = (vecTargetPos - vecViewOrigin).Normalized();

		float dist = (vecTargetPos - vecViewOrigin).Length();
		float triangleSize = baseSize * std::clamp(600.f / dist, 0.8f, 1.2f);

		flRadius = screenDistance * (ImGui::GetIO().DisplaySize.y / 480.f);

		float flRadiusX = screenDistance * static_cast<float>(Config::i(g_Variables.m_Visuals.m_bOOFArrowsWidth)) / 100.f;
		float flRadiusY = screenDistance * static_cast<float>(Config::i(g_Variables.m_Visuals.m_bOOFArrowsHeight)) / 100.f;

		GetOffScreenData(vecDelta, flRadiusX, flRadiusY, vecOffScreenPos, flOffScreenRotation);

		flOffScreenRotation = -flOffScreenRotation;

		float height = triangleSize * 1.732f;

		arrVecVerts[0] = { vecOffScreenPos.x, vecOffScreenPos.y - triangleSize };
		arrVecVerts[1] = { vecOffScreenPos.x - triangleSize, vecOffScreenPos.y + (height / 2) };
		arrVecVerts[2] = { vecOffScreenPos.x + triangleSize, vecOffScreenPos.y + (height / 2) };

		for (auto& vert : arrVecVerts)
			vert = RotateVertex(vecOffScreenPos, vert, flOffScreenRotation);

		std::vector<ImVec2> vecPoints(arrVecVerts.begin(), arrVecVerts.end());
		Draw::AddPolygon(vecPoints, Config::c(g_Variables.m_Visuals.m_colOOFArrows));
	}
}

void PlayerESP::Run(CCSPlayerController* pEntity, C_CSPlayerPawn* pPawn, CRecordTrack* pRecordTrack)
{

	if (!Config::b(g_Variables.m_Visuals.m_bEnableESP))
		return;
	if (!pEntity || !pPawn)
		return;
	if (pEntity == Globals::m_pLocalPlayerController)
		return;
	if (!Globals::m_pLocalPlayerPawn->IsEnemy(pPawn))
		return;
	if (!pEntity->m_bPawnIsAlive())
		return;

	if (Config::b(g_Variables.m_Visuals.m_bEnableOOFArrows) && Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		DrawOOF(pPawn);

	ctx = { };

	if (!GetEntityBoundingBox(pPawn, &ctx.m_Box))
		return;

	const float boxMidX = ctx.m_Box.left + (ctx.m_Box.width * 0.5f);
	const float boxBottom = ctx.m_Box.bottom;
	const float bottomPadding = ctx.m_arrPadding.at(DIR_BOTTOM);
	const bool isImmune = pPawn->m_bGunGameImmunity();

	const Color colBoxESP = Config::c(g_Variables.m_Visuals.m_colBoxESP);
	const Color colESPOutline = Config::c(g_Variables.m_Visuals.m_colESPOutline);
	const Color colSkeletonESP = Config::c(g_Variables.m_Visuals.m_colSkeletonEsp);
	const Color colBacktrackSkeletonESP = Config::c(g_Variables.m_Visuals.m_colBacktrackSkeletonEsp);
	const Color colNameESP = Config::c(g_Variables.m_Visuals.m_colNameEsp);
	const Color colWeaponESP = Config::c(g_Variables.m_Visuals.m_colWeaponEsp);
	const Color colWeaponIconESP = Config::c(g_Variables.m_Visuals.m_colWeaponIcon);
	const Color colArmorBar = Config::c(g_Variables.m_Visuals.m_colArmorBar);
	const Color white(255, 255, 255, 255);

	const ImVec2 boxTopLeft(ctx.m_Box.left, ctx.m_Box.top);
	const ImVec2 boxBottomRight(ctx.m_Box.right, ctx.m_Box.bottom);

	BoneData_t* pBoneArray = nullptr;
	if (Config::b(g_Variables.m_Visuals.m_bEnableSkeletonESP)) {
		pBoneArray = pPawn->m_pGameSceneNode()->GetSkeletonInstance()->m_modelState().m_pBoneArray;
	}

	CLagRecord* pFirstRecord = nullptr;
	CLagRecord* pLastRecord = nullptr;
	if (Config::b(g_Variables.m_Visuals.m_bEnableBacktrackSkeletonESP)) {
		if (!pRecordTrack->m_deqRecords.empty()) {
			pFirstRecord = pRecordTrack->GetFirstRecord();
			pLastRecord = pRecordTrack->GetLastRecord();
		}
	}

	if (Config::b(g_Variables.m_Visuals.m_bEnableBoxESP))
		DrawBox(pEntity, boxTopLeft, boxBottomRight, colBoxESP, colESPOutline);

	if (Config::b(g_Variables.m_Visuals.m_bEnableSkeletonESP) && pBoneArray)
		DrawSkeleton(pEntity, pPawn, boxTopLeft, boxBottomRight, colSkeletonESP, colESPOutline, pBoneArray);

	if (Config::b(g_Variables.m_Visuals.m_bEnableBacktrackSkeletonESP) && pFirstRecord && pLastRecord)
	{
		float flDelta = (pFirstRecord->m_Bones->vecPosition - pLastRecord->m_Bones->vecPosition).Length2D();
		if (flDelta > 0.18f)
		{
			DrawSkeleton(pEntity, pPawn, boxTopLeft, boxBottomRight, colBacktrackSkeletonESP, colESPOutline, pLastRecord->m_Bones);
		}
	}

	if (Config::b(g_Variables.m_Visuals.m_bEnableHealthESP)) {
		const ImVec2 healthBarMin(ctx.m_Box.left - 6.0f, ctx.m_Box.top - 1.0f);
		const ImVec2 healthBarMax(ctx.m_Box.left - 2.0f, ctx.m_Box.bottom + 1.0f);
		DrawHealthBar(pPawn, healthBarMin, healthBarMax, white, colESPOutline);
	}

	if (Config::b(g_Variables.m_Visuals.m_bEnableArmorESP)) {
		const float rightPadding = ctx.m_arrPadding.at(DIR_RIGHT);
		const ImVec2 armorBarMin(ctx.m_Box.right + rightPadding + 2.0f, ctx.m_Box.top - 1.0f);
		const ImVec2 armorBarMax(ctx.m_Box.right + rightPadding + 6.0f, ctx.m_Box.bottom + 1.0f);
		DrawArmorBar(pPawn, armorBarMin, armorBarMax, colArmorBar, colESPOutline);
	}

	if (Config::b(g_Variables.m_Visuals.m_bEnableAmmoESP)) {
		const float bottomPaddingAmmo = ctx.m_arrPadding.at(DIR_BOTTOM);
		const ImVec2 ammoBarMin(ctx.m_Box.left - 1.0f, ctx.m_Box.bottom + bottomPaddingAmmo + 2.0f);
		const ImVec2 ammoBarMax(ctx.m_Box.right + 1.0f, ctx.m_Box.bottom + bottomPaddingAmmo + 6.0f);
		DrawAmmoBar(pPawn, ammoBarMin, ammoBarMax, Config::c(g_Variables.m_Visuals.m_colAmmoBar), colESPOutline);
	}

	if (Config::b(g_Variables.m_Visuals.m_bEnableNameESP)) {
		const ImVec2 namePos(boxMidX, ctx.m_Box.top - 2.0f);
		DrawName(pEntity, namePos, colNameESP, colESPOutline);
	}

	const ImVec2 weaponPos(boxMidX, boxBottom + bottomPadding);
	if (Config::b(g_Variables.m_Visuals.m_bEnableWeaponESP))
		DrawWeapon(pEntity, pPawn, weaponPos, colWeaponESP, colESPOutline);

	if (Config::b(g_Variables.m_Visuals.m_bEnableWeaponIconESP))
		DrawWeaponIcon(pEntity, pPawn, weaponPos, colWeaponIconESP, colESPOutline);

	if (Config::b(g_Variables.m_Visuals.m_bEnableHeadDotESP)) {

		CSkeletonInstance* pSkeletonCheck = pPawn->m_pGameSceneNode()->GetSkeletonInstance();
		if (pSkeletonCheck && pSkeletonCheck->m_pBoneCache)
		{
			Vector vecHeadBonePos = pPawn->m_pGameSceneNode()->GetBonePosition(6);
			if (vecHeadBonePos.IsValid() && !vecHeadBonePos.IsZero())
			{
				ImVec2 vecHeadScreenPos;
				if (Draw::WorldToScreen(vecHeadBonePos, vecHeadScreenPos)) {
					Draw::AddCircle(vecHeadScreenPos, 2, Color(255, 0, 0), 24);
				}
			}
		}
	}

	DrawFlags(pEntity, pPawn, ImVec2(ctx.m_Box.right + 2.0f, ctx.m_Box.top), white, colESPOutline);
}

static std::unordered_map<void*, float> healthAnimationMap;

void PlayerESP::DrawHealthBar(C_CSPlayerPawn* pPawn, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline)
{
	void* entityKey = static_cast<void*>(pPawn);

	float& currentHealth = healthAnimationMap[entityKey];
	if (currentHealth == 0.0f)
		currentHealth = static_cast<float>(pPawn->m_iHealth());

	const int targetHealth = pPawn->m_iHealth();
	const float targetHealthFactor = static_cast<float>(targetHealth) * 0.01f;

	const float healthTransitionSpeed = 8.0f;
	currentHealth += (static_cast<float>(targetHealth) - currentHealth) * std::min(ImGui::GetIO().DeltaTime * healthTransitionSpeed, 1.0f);

	const float currentHealthFactor = std::max(currentHealth * 0.01f, 0.0f);
	const float flHue = (currentHealthFactor * 120.f) / 360.f;
	const float flHeight = vecMax.y - vecMin.y;

	const ImVec2 healthBarMin = ImVec2(vecMin.x + 1.0f, (vecMax.y + 1.0f) - flHeight * currentHealthFactor);
	const ImVec2 healthBarMax = ImVec2(vecMax.x - 1.0f, vecMax.y - 1.0f);

	Draw::AddRect(vecMin, vecMax, Color(40, 40, 40, 100), DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline);

	Draw::AddRect(healthBarMin, healthBarMax,
		Config::c(g_Variables.m_Visuals.m_colHealthBarEsp),
		DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline);

	const std::string strHealth = std::to_string(targetHealth);
	const ImVec2 textSize = Fonts::ESP->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, strHealth.c_str());
	const ImVec2 textPos = ImVec2(
		healthBarMin.x + ((healthBarMax.x - healthBarMin.x) / 2.0f) - (textSize.x / 2.0f),
		healthBarMin.y - textSize.y + 2.0f
	);

	if (targetHealth < 100)
		Draw::AddText(Fonts::ESP, 10.0f, textPos, strHealth.c_str(), colColor, DRAW_TEXT_OUTLINE, colOutline);

	ctx.m_arrPadding.at(DIR_LEFT) += 7.0f;

	if (targetHealth <= 0)
		healthAnimationMap.erase(entityKey);
}

void PlayerESP::DrawArmorBar(C_CSPlayerPawn* pPawn, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline)
{
	const int iArmor = pPawn->m_ArmorValue();
	const float flFactor = static_cast<float>(iArmor) * 0.01f;
	const float flHeight = vecMax.y - vecMin.y;

	Draw::AddRect(vecMin, vecMax, Color(40, 40, 40, 100), DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline);
	Draw::AddRect(ImVec2(vecMin.x + 1.0f, (vecMax.y + 1.0f) - flHeight * flFactor), ImVec2(vecMax.x - 1.0f, vecMax.y - 1.0f), colColor, DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline);

	const std::string strArmor = std::to_string(iArmor);
	const ImVec2 textSize = Fonts::ESP->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, strArmor.c_str());
	const ImVec2 textPos = ImVec2(
		vecMin.x + ((vecMax.x - vecMin.x) / 2.0f) - (textSize.x / 2.0f),
		(vecMax.y + 1.0f) - flHeight * flFactor - textSize.y + 2.0f
	);

	if (iArmor < 100)
		Draw::AddText(Fonts::ESP, 10.0f, textPos, strArmor.c_str(), Color(255, 255, 255, 255), DRAW_TEXT_OUTLINE, colOutline);

	ctx.m_arrPadding.at(DIR_RIGHT) += 7.0f;
}

void PlayerESP::DrawAmmoBar(C_CSPlayerPawn* pPawn, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline)
{
	C_BasePlayerWeapon* pActiveWeapon = pPawn->m_pWeaponServices()->m_hActiveWeapon().Get();
	if (!pActiveWeapon)
		return;

	CCSWeaponBaseVData* pWeaponData = pActiveWeapon->GetWeaponBaseVData();
	if (!pWeaponData)
		return;

	const int iMaxClip = pWeaponData->m_iMaxClip1();
	if (iMaxClip <= 0)
		return;

	const int iClip = pActiveWeapon->m_iClip1();
	const int iReserve = pActiveWeapon->m_pReserveAmmo();

	const float flFactor = static_cast<float>(iClip) / static_cast<float>(iMaxClip);
	const float flWidth = vecMax.x - vecMin.x;

	Draw::AddRect(vecMin, vecMax, Color(40, 40, 40, 100), DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline);
	Draw::AddRect(ImVec2(vecMin.x + 1.0f, vecMin.y + 1.0f), ImVec2(vecMin.x + 1.0f + (flWidth - 2.0f) * flFactor, vecMax.y - 1.0f), colColor, DRAW_RECT_FILLED | DRAW_RECT_ALIGNED, colOutline);

	ctx.m_arrPadding.at(DIR_BOTTOM) += 7.0f;
}

void PlayerESP::DrawName(CCSPlayerController* pEntity, ImVec2 vecPosition, Color colColor, Color colOutline)
{
	const std::string strName = pEntity->m_sSanitizedPlayerName();
	const ImVec2 vecNameSize = Fonts::VerdanaEsp->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, strName.c_str());
	Draw::AddText(Fonts::VerdanaEsp, 10.0f, ImVec2(vecPosition.x - vecNameSize.x * 0.5f - 0.25f, vecPosition.y - vecNameSize.y - 1.5f), strName.c_str(), colColor, DRAW_TEXT_DROPSHADOW, colOutline);
	ctx.m_arrPadding.at(DIR_TOP) += vecNameSize.y;
}

void PlayerESP::DrawWeapon(CCSPlayerController* pEntity, C_CSPlayerPawn* pPawn, ImVec2 vecPosition, Color colColor, Color colOutline)
{
	C_BasePlayerWeapon* pActiveWeapon = pPawn->m_pWeaponServices()->m_hActiveWeapon().Get();
	if (!pActiveWeapon)
		return;

	C_AttributeContainer* pAttributeContainer = pActiveWeapon->m_AttributeManager();
	if (!pAttributeContainer)
		return;

	C_EconItemView* pEconItemView = pAttributeContainer->m_Item();
	if (!pEconItemView)
		return;

	CEconItemDefinition* pEconItemDefinition = pEconItemView->GetStaticData();
	if (!pEconItemDefinition)
		return;

	std::string strWeaponName = Interfaces::m_pLocalize->FindSafe(pEconItemDefinition->m_pszItemBaseName);
	const ImVec2 vecNameSize = Fonts::VerdanaEsp->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, strWeaponName.c_str());
	Draw::AddText(Fonts::VerdanaEsp, 10.0f, ImVec2(vecPosition.x - vecNameSize.x * 0.5f - 0.75f, vecPosition.y + 2.0f), strWeaponName.c_str(), colColor, DRAW_TEXT_DROPSHADOW, colOutline);
	ctx.m_arrPadding.at(DIR_BOTTOM) += vecNameSize.y + 2.0f;
}

void PlayerESP::DrawFlags(CCSPlayerController* pEntity, C_CSPlayerPawn* pPawn, ImVec2 vecPosition, Color colColor, Color colOutline)
{
	CCSPlayer_ItemServices* pItemServices = pPawn->m_pItemServices();
	CCSPlayerController_InGameMoneyServices* pInGameMoneyServices = pEntity->m_pInGameMoneyServices();
	if (!pItemServices || !pInGameMoneyServices)
		return;

	int iFlagsHeight = 0;

	bool bIsFlashed = pPawn->m_flFlashAlpha() > 0.0f;
	FlagObjects_t flagObjects{ pItemServices->m_bHasHelmet(), pItemServices->m_bHasDefuser(), pPawn->m_bIsScoped(), pPawn->m_bIsDefusing(), bIsFlashed, pInGameMoneyServices->m_iAccount() };

	if (Config::vb(g_Variables.m_Visuals.m_vecFlags).at(FLAG_MONEY))
	{
		const std::string strText = std::to_string(flagObjects.m_iMoney).append(X("$"));
		const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, strText.c_str());
		Draw::AddText(Fonts::ESP, 10.0f, ImVec2(vecPosition.x, vecPosition.y + iFlagsHeight), strText.c_str(), Color(141, 227, 66, 255), DRAW_TEXT_OUTLINE, colOutline);
		iFlagsHeight += vecTextSize.y - 2.0f;
	}

	if (Config::Get<std::vector<bool>>(g_Variables.m_Visuals.m_vecFlags).at(FLAG_ARMOR) && pPawn->IsArmored(HITGROUP_CHEST))
	{
			const std::string strText = flagObjects.m_bHasHelmet ? X("H+K") : X("K");
			const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, strText.c_str());
			Draw::AddText(Fonts::ESP, 10.0f, ImVec2(vecPosition.x, vecPosition.y + iFlagsHeight), strText.c_str(), colColor, DRAW_TEXT_OUTLINE, colOutline);
			iFlagsHeight += vecTextSize.y - 2.0f;
	}

	if (Config::Get<std::vector<bool>>(g_Variables.m_Visuals.m_vecFlags).at(FLAG_KIT) && flagObjects.m_bHasDefuser)
	{
		const std::string strText = X("KIT");
		const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, strText.c_str());
		Draw::AddText(Fonts::ESP, 10.0f, ImVec2(vecPosition.x, vecPosition.y + iFlagsHeight), strText.c_str(), Color(63, 151, 224, 255), DRAW_TEXT_OUTLINE, colOutline);
		iFlagsHeight += vecTextSize.y - 2.0f;
	}

	if (Config::Get<std::vector<bool>>(g_Variables.m_Visuals.m_vecFlags).at(FLAG_DEFUSING) && flagObjects.m_bIsDefusing)
	{
		const std::string strText = X("DEFUSING");
		const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, strText.c_str());
		Draw::AddText(Fonts::ESP, 10.0f, ImVec2(vecPosition.x, vecPosition.y + iFlagsHeight), strText.c_str(), Color(63, 151, 224, 255), DRAW_TEXT_OUTLINE, colOutline);
		iFlagsHeight += vecTextSize.y - 2.0f;
	}

	if (Config::Get<std::vector<bool>>(g_Variables.m_Visuals.m_vecFlags).at(FLAG_ZOOM) && flagObjects.m_bIsScoping)
	{
		const std::string strText = X("ZOOM");
		const ImVec2 vecTextSize = Fonts::ESP->CalcTextSizeA(10.0f, FLT_MAX, 0.0f, strText.c_str());
		Draw::AddText(Fonts::ESP, 10.0f, ImVec2(vecPosition.x, vecPosition.y + iFlagsHeight), strText.c_str(), colColor, DRAW_TEXT_OUTLINE, colOutline);
		iFlagsHeight += vecTextSize.y - 2.0f;
	}
}

constexpr const char* WeaponToFontLetter(const char* weapon) {

	if (weapon == nullptr)
		return "";

	if (strcmp(weapon, "weapon_knife_ct") == 0) return "]";
	if (strcmp(weapon, "weapon_knife_t") == 0) return "[";

	if (strcmp(weapon, "weapon_deagle") == 0) return "A";
	if (strcmp(weapon, "weapon_elite") == 0) return "B";
	if (strcmp(weapon, "weapon_fiveseven") == 0) return "C";
	if (strcmp(weapon, "weapon_glock") == 0) return "D";
	if (strcmp(weapon, "weapon_revolver") == 0) return "J";
	if (strcmp(weapon, "weapon_hkp2000") == 0) return "E";
	if (strcmp(weapon, "weapon_p250") == 0) return "F";
	if (strcmp(weapon, "weapon_usp_silencer") == 0) return "G";
	if (strcmp(weapon, "weapon_tec9") == 0) return "H";
	if (strcmp(weapon, "weapon_cz75a") == 0) return "I";

	if (strcmp(weapon, "weapon_mac10") == 0) return "K";
	if (strcmp(weapon, "weapon_ump45") == 0) return "L";
	if (strcmp(weapon, "weapon_bizon") == 0) return "M";
	if (strcmp(weapon, "weapon_mp7") == 0) return "N";
	if (strcmp(weapon, "weapon_mp9") == 0) return "R";
	if (strcmp(weapon, "weapon_p90") == 0) return "O";

	if (strcmp(weapon, "weapon_galilar") == 0) return "Q";
	if (strcmp(weapon, "weapon_famas") == 0) return "R";
	if (strcmp(weapon, "weapon_m4a1_silencer") == 0) return "T";
	if (strcmp(weapon, "weapon_m4a1") == 0) return "S";
	if (strcmp(weapon, "weapon_aug") == 0) return "U";
	if (strcmp(weapon, "weapon_sg556") == 0) return "V";
	if (strcmp(weapon, "weapon_ak47") == 0) return "W";

	if (strcmp(weapon, "weapon_g3sg1") == 0) return "X";
	if (strcmp(weapon, "weapon_scar20") == 0) return "Y";
	if (strcmp(weapon, "weapon_awp") == 0) return "Z";
	if (strcmp(weapon, "weapon_ssg08") == 0) return "a";

	if (strcmp(weapon, "weapon_xm1014") == 0) return "b";
	if (strcmp(weapon, "weapon_sawedoff") == 0) return "c";
	if (strcmp(weapon, "weapon_mag7") == 0) return "d";
	if (strcmp(weapon, "weapon_nova") == 0) return "e";

	if (strcmp(weapon, "weapon_negev") == 0) return "f";
	if (strcmp(weapon, "weapon_m249") == 0) return "g";

	if (strcmp(weapon, "weapon_taser") == 0) return "h";

	if (strcmp(weapon, "weapon_flashbang") == 0) return "i";
	if (strcmp(weapon, "weapon_hegrenade") == 0) return "j";
	if (strcmp(weapon, "weapon_smokegrenade") == 0) return "k";
	if (strcmp(weapon, "weapon_molotov") == 0) return "l";
	if (strcmp(weapon, "weapon_decoy") == 0) return "m";
	if (strcmp(weapon, "weapon_incgrenade") == 0) return "n";

	if (strcmp(weapon, "weapon_c4") == 0) return "o";

	return "";
}

void PlayerESP::DrawWeaponIcon(CCSPlayerController* pEntity, C_CSPlayerPawn* pPawn, ImVec2 vecPosition, Color colColor, Color colOutline)
{
	if(!pEntity->GetPlayerWeapon(pEntity->m_hPlayerPawn().Get())) return;
	if (!pEntity->GetPlayerWeapon(pEntity->m_hPlayerPawn().Get())->GetWeaponBaseVData()) return;
	if (!pPawn) return;

	const char* weaponLetter = pEntity->GetPlayerWeapon(pPawn)->GetWeaponBaseVData()->m_szName();
	if (!weaponLetter) return;

	const char* weaponFontLetter = WeaponToFontLetter(weaponLetter);
	ImVec2 vecTextSize = Fonts::GunIcons->CalcTextSizeA(12, FLT_MAX, 0.0f, weaponFontLetter);

	float offsetY = 14.f;
	if (Config::b(g_Variables.m_Visuals.m_bEnableWeaponESP))
		offsetY = 4.f;

	Draw::AddText(Fonts::GunIcons, 12, ImVec2(vecPosition.x - vecTextSize.x * 0.5f, vecPosition.y - vecTextSize.y + offsetY), weaponFontLetter, colColor, DRAW_TEXT_DROPSHADOW, Color(0, 0, 0, 100));
	ctx.m_arrPadding.at(DIR_BOTTOM) += vecTextSize.y + offsetY;
}

void PlayerESP::DrawSkeleton(CCSPlayerController* pEntity, C_CSPlayerPawn* pPawn, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline, BoneData_t* bones)
{
	BoneData_t hPrevious, hCurrent;
	for (auto& vecBones : BoneJointList::List)
	{
		hPrevious.vecPosition = Vector(0, 0, 0);
		for (DWORD dwBoneIndex : vecBones)
		{
			hCurrent = bones[dwBoneIndex];
			if (hPrevious.vecPosition == Vector(0, 0, 0))
			{
				hPrevious = hCurrent;
				continue;
			}

			ImVec2 previousSP, currentSP;

			if (Draw::WorldToScreen(hPrevious.vecPosition, previousSP) && Draw::WorldToScreen(hCurrent.vecPosition, currentSP))
			{
				Draw::AddLine(previousSP, currentSP, colColor, 1.f);
			}

			hPrevious = hCurrent;
		}
	}
}

void PlayerESP::DrawBox(CCSPlayerController* pEntity, ImVec2 vecMin, ImVec2 vecMax, Color colColor, Color colOutline)
{
	Draw::AddRect(vecMin, vecMax, colColor, DRAW_RECT_OUTLINE | DRAW_RECT_BORDER | DRAW_RECT_ALIGNED, colOutline, 0.0f);
}