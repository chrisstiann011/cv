#pragma once

namespace Tabs
{
	inline int m_iCurrentTab = 4;
	inline int m_iCurrentlegitbotTab = 4;

	inline const char* HitboxNames[] =
	{
		"Head",
		"Chest",
		"Stomach",
		"Pelvis",
		"Arms",
		"Legs",
		"Feet"
	};

	inline const char* LegitbotConditions[] =
	{
		"Visible",
		"No smoke",
		"No flash"
	};
	inline const char* szRemovals[] =
	{
		"Smoke",
		"Flash",
		"Local Legs",
		"Crosshair",
		"Local Overhead Info",
		"Team Overhead Info",
		"Team Intro",
		"Fog Brightness",
	};

	inline const char* szAutostopModes[ ] =
	{
		"Early",
		"In-Air",
	};

	inline const char* szHistoryAimModes[] =
	{
		"Enable BackTracking",
		"[dev] Backtrack Only",
	};

	inline const char* WeaponNames[] =
	{
		"Light Pistols",
		"Deagle",
		"Revolver",
		"SMGs",
		"LMGs",
		"Assault Rifles",
		"Shotguns",
		"Scout",
		"Auto-Snipers",
		"AWP"
	};

	inline const char* TargetTypes[] =
	{
		"Damage",
		"HitChance",
		"Low Health"
	};

}

namespace Fonts
{
	inline ImFont* Default;
	inline ImFont* GunIcons;
	inline ImFont* ESP;
	inline ImFont* Verdana;
	inline ImFont* VerdanaBold;
	inline ImFont* VerdanaEsp;
	inline ImFont* SegoeUIBold;
	inline ImFont* SegoeUIBold18;
	inline ImFont* SegoeUIBold18_ITALIC;
	inline ImFont* imFA = nullptr;
}

namespace Gui
{
	inline bool m_bInitialized = false;
	inline bool m_bOpen = false;
	inline bool m_bInputActive = false;
	inline ImVec2 m_vecScreenSize = ImVec2(0.0f, 0.0f);

	inline int m_iAverageFPS = 0;

	inline ImColor colThemeColor = ImColor(255, 20, 147);

	inline ID3D11ShaderResourceView* m_pSecretBackgroundTexture = nullptr;
	inline int m_iSecretBackgroundWidth = 0;
	inline int m_iSecretBackgroundHeight = 0;

	void Initialize(unsigned int uFontFlags = 0x0);

	void Update(ImGuiIO& io);
	void RenderSpectatorList();
	void RenderWatermark();

	void Render();
}