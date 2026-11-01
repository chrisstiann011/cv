#pragma once

class CLegitbot
{
public:
	struct LegitbotPlayer_t
	{
		CCSPlayerController* m_pController;
		C_CSPlayerPawn* m_pPawn;

		QAngle m_aShootAngle;

		int m_iHealth;
		int m_iFov_CenterToAimpos;
	};

	struct LegitbotData_t
	{
		bool m_bWants2DMoveHalt = false;
		std::vector<bool> m_vbAutostop = std::vector<bool>(EAutostop::AUTOSTOP_MAX, false);
	};

	LegitbotData_t m_Data = {};

public:
	std::vector<EHitBoxes> LegitbotHB_To_HB(ELegitbotHitboxes nLegitbotHB);

	void CreateMove();
	void Triggerbot();
	void AutoRevolver();
	std::vector<LegitbotPlayer_t> GetPlayers();

	void ScreenSpaceAim(float flFovDiameter, int iSmoothness);
};
inline const auto g_Legitbot = std::make_unique<CLegitbot>();