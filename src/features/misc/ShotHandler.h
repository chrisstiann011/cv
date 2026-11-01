#pragma once

class CGameEvent;

struct OnShotSkeleton_t
{
	C_CSPlayerPawn* m_pPawn;
	CCSPlayerController* m_pController;
	BoneData_t m_Matrix[128];
	float m_flTimeAdded;
};

struct ShotInfo_t {
	Vector m_vShootPos;
	Vector m_vTargetPos;
	QAngle m_angShootAngle;
	C_CSPlayerPawn* m_pTarget;
	int m_nHitbox;
	float m_flDamage;
	float m_flTime;
	bool m_bHit;
	std::string m_strWeapon;
};

class CShotHandler {
public:
	struct PlayerHurtRecord_t {
		int m_iHitGroup;
		Vector m_vecBoneOrigin;
		Vector m_vecImpactPosition;
		float m_flTime;
	};
public:
	std::deque<PlayerHurtRecord_t> m_HitRecords;
	std::deque<ShotInfo_t> m_ShotRecords;
public:
	void RenderHitMarker(const ImVec2& pos, float size = 6.0f);
	void OnPlayerHurt(CGameEvent* pEvent);
	void OnBulletImpact(CGameEvent* pEvent);
	void OnWeaponFire(CGameEvent* pEvent);
	void OnPresent();
	void RegisterShot(const Vector& shootPos, const Vector& targetPos, const QAngle& shootAngle,
					 C_CSPlayerPawn* target, int hitbox, float damage, const std::string& weapon);
	void CreateImpactShockEffect(const Vector& vImpactPos);
	void ProcessMissLogs();
	void LogMiss(const ShotInfo_t& shot, const std::string& reason);
};

inline auto g_ShotHandler = std::make_unique<CShotHandler>();
