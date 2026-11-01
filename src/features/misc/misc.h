#pragma once

#include "../../Precompiled.h"
#include <mutex>

class CMisc
{
public:
	void AutoPurchase();
	void AutoPistol();
	void StraightThrow(const QAngle& pViewAngles);
	float GetMaxPlayerSpeed();
	void AccurateWalk(CUserCmd* pCmd);
	void PenetrationCrosshair();
	void RunRCS(C_CSPlayerPawn* local, CUserCmd* cmd, int shots_fired, float scale = 2.0f);
	void FreeLook(CCSGOInput* pInput, CUserCmd* pUserCmd);
	void ShowHud();
	void ShowImpacts();
	void DotPenetration();
	void LSDMode();

	QAngle GetFreeLookAngle() const { std::lock_guard<std::mutex> lock(m_FreeLookMutex); return m_FreeLookAngle; }
	Vector GetFreeLookOffset() const { std::lock_guard<std::mutex> lock(m_FreeLookMutex); return m_FreeLookOffset; }
	void SetFreeLookAngle(const QAngle& angle) { std::lock_guard<std::mutex> lock(m_FreeLookMutex); m_FreeLookAngle = angle; }
	void SetFreeLookOffset(const Vector& offset) { std::lock_guard<std::mutex> lock(m_FreeLookMutex); m_FreeLookOffset = offset; }

private:
	mutable std::mutex m_FreeLookMutex;
	QAngle m_FreeLookAngle = QAngle(0.f, 0.f, 0.f);
	Vector m_FreeLookOffset = Vector(0.f, 0.f, 0.f);
};

const auto g_Misc = std::make_unique<CMisc>();