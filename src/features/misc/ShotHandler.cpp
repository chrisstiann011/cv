#include "../../Precompiled.h"
#include "../../hooks/Hooks.h"
#include "../../gui/ChlenGui.h"
#include "../ragebot/ragebot.h"

constexpr inline const char* s_szSoundPaths[] = {
	"sounds/ui/coin_pickup_01.vsnd_c",
	"sounds/ui/panorama/mainmenu_press_shop_01.vsnd_c",
	"sounds/ui/item_drop.vsnd_c",
	"sounds/metallic.vsnd_c",
	"sounds/sound.vsnd_c",
};
void CShotHandler::RenderHitMarker(const ImVec2& pos, float size)
{
	auto clr = Config::c(g_Variables.m_Visuals.m_colHitmarker);
	auto color = clr.GetU32();

	auto draw = ImGui::GetBackgroundDrawList();
	ImU32 outline = IM_COL32(0, 0, 0, 255);

	draw->AddLine(ImVec2(pos.x - size, pos.y - size), ImVec2(pos.x - size / 2, pos.y - size / 2), outline, 1.0f);
	draw->AddLine(ImVec2(pos.x - size, pos.y - size), ImVec2(pos.x - size / 2, pos.y - size / 2), color);

	draw->AddLine(ImVec2(pos.x + size, pos.y - size), ImVec2(pos.x + size / 2, pos.y - size / 2), outline, 1.0f);
	draw->AddLine(ImVec2(pos.x + size, pos.y - size), ImVec2(pos.x + size / 2, pos.y - size / 2), color);

	draw->AddLine(ImVec2(pos.x - size, pos.y + size), ImVec2(pos.x - size / 2, pos.y + size / 2), outline, 1.0f);
	draw->AddLine(ImVec2(pos.x - size, pos.y + size), ImVec2(pos.x - size / 2, pos.y + size / 2), color);

	draw->AddLine(ImVec2(pos.x + size, pos.y + size), ImVec2(pos.x + size / 2, pos.y + size / 2), outline, 1.0f);
	draw->AddLine(ImVec2(pos.x + size, pos.y + size), ImVec2(pos.x + size / 2, pos.y + size / 2), color);
}

void CShotHandler::OnPlayerHurt(CGameEvent* pEvent)
{
	if (!Interfaces::m_pEngine->IsConnected() || !Interfaces::m_pEngine->IsInGame())
		return;

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return;

	CCSPlayerController* pAttacker = reinterpret_cast<CCSPlayerController*>(pEvent->GetPlayerController(X("attacker")));
	CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(pEvent->GetPlayerController(X("userid")));
	C_CSPlayerPawn* pPawn = reinterpret_cast<C_CSPlayerPawn*>(pController->m_hPawn().Get());

	if (!pAttacker || !pController || !pAttacker->m_bIsLocalPlayerController())
		return;

	int dmg = pEvent->GetInt(X("dmg_health"));
	if (dmg <= 0)
		return;

	for (auto& shot : m_ShotRecords)
	{
		if (shot.m_pTarget == pPawn && !shot.m_bHit)
		{
			shot.m_bHit = true;
			break;
		}
	}

	if (Config::b(g_Variables.m_Visuals.m_bShockEffect))
		pPawn->CreateShockEffect();

	if (Config::b(g_Variables.m_Misc.m_bHitSound))
		Utilities::PlayVSound(s_szSoundPaths[Config::i(g_Variables.m_Misc.m_iHitSoundIndex)], 0.9f);

	int nHitGroup = pEvent->GetInt("hitgroup");
	std::string strChannelMessage =
		"Hit " + std::string(pController->m_sSanitizedPlayerName()) + " for " +
		std::to_string(dmg) + " in " +
		Utilities::GetHitgroupName(nHitGroup) + " with " +
		pEvent->GetString(X("weapon")) + " (remaining: " +
		std::to_string(pEvent->GetInt(X("health"))) + ")";

	if (Config::b(g_Variables.m_Misc.m_bHitlogs))
	{
		AddHitLog(strChannelMessage);

		m_bConsoleLog = true;

		Color colMessage(
			static_cast<int>(g_AccentColor.x * 255.0f),
			static_cast<int>(g_AccentColor.y * 255.0f),
			static_cast<int>(g_AccentColor.z * 255.0f),
			255
		);

		std::string strFormattedMessage = "Hit " +
			std::string(pController->m_sSanitizedPlayerName()) + " for " +
			std::to_string(dmg) + " in " +
			Utilities::GetHitgroupName(nHitGroup) + " with " +
			pEvent->GetString(X("weapon")) + " (remaining: " +
			std::to_string(pEvent->GetInt(X("health"))) + ")";
		CS2ChannelManager::Msg(X("CHLENIXX"), strFormattedMessage.c_str(), colMessage);
		m_bConsoleLog = false;
	}

	const float flBacktrackTime = (NRagebot::g_Rage->m_Data.m_LastTarget.HasValidTarget() &&
		NRagebot::g_Rage->m_Data.m_LastTarget.m_pLagRecord &&
		NRagebot::g_Rage->m_Data.m_LastTarget.m_pLagRecord->m_pPawn == pPawn)
		? NRagebot::g_Rage->m_Data.m_LastTarget.m_pLagRecord->m_flSimulationTime
		: pPawn->m_flSimulationTime();

	auto& arr = g_Chams->m_mapOnShotChams[pPawn];

	std::rotate(arr.begin(), arr.begin() + 1, arr.end());
	arr.back() = flBacktrackTime;
}

void CShotHandler::OnWeaponFire(CGameEvent* pEvent)
{
	if (!Interfaces::m_pEngine->IsConnected() || !Interfaces::m_pEngine->IsInGame())
		return;

	if (!Globals::m_pLocalPlayerController || !Globals::m_pLocalPlayerController->m_bPawnIsAlive())
		return;

	CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(pEvent->GetPlayerController(X("userid")));
	if (!pController || !pController->m_bIsLocalPlayerController())
		return;

	ProcessMissLogs();
}

void CShotHandler::RegisterShot(const Vector& shootPos, const Vector& targetPos, const QAngle& shootAngle,
								C_CSPlayerPawn* target, int hitbox, float damage, const std::string& weapon)
{
	if (!target)
		return;

	ShotInfo_t shot;
	shot.m_vShootPos = shootPos;
	shot.m_vTargetPos = targetPos;
	shot.m_angShootAngle = shootAngle;
	shot.m_pTarget = target;
	shot.m_nHitbox = hitbox;
	shot.m_flDamage = damage;
	shot.m_flTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;
	shot.m_bHit = false;
	shot.m_strWeapon = weapon;

	m_ShotRecords.push_back(shot);

	if (m_ShotRecords.size() > 32)
		m_ShotRecords.pop_front();
}

void CShotHandler::ProcessMissLogs()
{
	if (m_ShotRecords.empty())
		return;

	float flCurrentTime = Interfaces::m_pGlobalVariables->m_flCurrentTime;

	for (auto it = m_ShotRecords.begin(); it != m_ShotRecords.end();)
	{
		ShotInfo_t& shot = *it;

		if ((flCurrentTime - shot.m_flTime) > 0.2f && !shot.m_bHit)
		{

			std::string reason = "Unknown";

			if (!shot.m_pTarget || shot.m_pTarget->m_iHealth() <= 0)
			{
				reason = "Target died";
			}
			else
			{

				C_CSWeaponBase* pWeapon = LocalPlayerData::m_pWeapon;
				if (pWeapon)
				{

					float flInaccuracy = 0.01f;

					if (flInaccuracy > 0.01f)
					{
						reason = "Spread/Inaccuracy";
					}
					else
					{

						reason = "Registration failed";
					}
				}
			}

			LogMiss(shot, reason);
			it = m_ShotRecords.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CShotHandler::LogMiss(const ShotInfo_t& shot, const std::string& reason)
{
	if (!Config::b(g_Variables.m_Misc.m_bMisslogs))
		return;

	if (!shot.m_pTarget)
		return;

	std::string targetName = "Player";
	std::string hitboxName = Utilities::GetHitgroupName(shot.m_nHitbox);

	std::string missMessage = "Missed shot in " + targetName +
							 " due to " + hitboxName +
							 " (" + std::to_string(static_cast<int>(shot.m_flDamage)) + " dmg) - " + reason;

	Color colMiss(255, 100, 100, 255);
	CS2ChannelManager::Msg(X("CHLENIXX"), missMessage.c_str(), colMiss);
}

void CShotHandler::OnBulletImpact(CGameEvent* pEvent)
{
	CCSPlayerController* pController = reinterpret_cast<CCSPlayerController*>(pEvent->GetPlayerController(X("userid")));
	if (!pController || !pController->m_bIsLocalPlayerController())
		return;

	g_ParticleMgr->OnEvent(pEvent);
	if (Config::b(g_Variables.m_Visuals.m_SparkShotEffect)) {
		Vector vImpactPos;
		vImpactPos.x = pEvent->GetFloat(X("x"));
		vImpactPos.y = pEvent->GetFloat(X("y"));
		vImpactPos.z = pEvent->GetFloat(X("z"));

		C_CSPlayerPawn* pLocalPawn = Globals::m_pLocalPlayerPawn;
		if (pLocalPawn) {

			this->CreateImpactShockEffect(vImpactPos);
		}
	}
}
void CShotHandler::CreateImpactShockEffect(const Vector& vImpactPos)
{
	if (!Interfaces::m_pParticleManager)
		return;

	unsigned int effectIndex = 0;
	Interfaces::m_pParticleManager->CreateParticleEffect(&effectIndex, "particles/inferno_fx/explosion_incend_air_falling.vpcf");

	if (effectIndex != 0) {
		Interfaces::m_pParticleManager->SetParticleSetting(effectIndex, EParticleSetting::PARTICLE_SETTING_POSITION, (void*)&vImpactPos);

		Color sparkColor(255, 255, 255, 255);
		Interfaces::m_pParticleManager->SetParticleSetting(effectIndex, EParticleSetting::PARTICLE_SETTING_COLOR, (void*)&sparkColor);
	}
}
void CShotHandler::OnPresent()
{
	if (!Interfaces::m_pGlobalVariables)
		return;

	float flNow = Interfaces::m_pGlobalVariables->m_flCurrentTime;
	float flMaxDuration = static_cast<float>(Config::i(g_Variables.m_Visuals.m_nHitEffectsDurration));

	if (Config::b(g_Variables.m_Visuals.m_bHitmarker))
	{
		if (!m_HitRecords.empty())
		{
			auto itRemove = std::remove_if(
				m_HitRecords.begin(),
				m_HitRecords.end(),
				[flNow, flMaxDuration](const PlayerHurtRecord_t& record)
				{
					return (flNow - record.m_flTime) > flMaxDuration;
				}
			);

			if (itRemove != m_HitRecords.end())
			{
				m_HitRecords.erase(itRemove, m_HitRecords.end());
			}
		}

		for (const auto& record : m_HitRecords)
		{
			ImVec2 screenPos;
			if (Draw::WorldToScreen(record.m_vecImpactPosition, screenPos))
			{
				this->RenderHitMarker(screenPos);
			}
		}
	}
}