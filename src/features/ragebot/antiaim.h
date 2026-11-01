#pragma once

class CAntiAim
{
private:
	float GetYaw();
	float GetPitch();
	float GetRoll();
	float GetAtTarget(float flOldYaw);
public:
	QAngle m_angAtTarget;
	void OnCreateMove();
};
inline auto g_AntiAim = std::make_unique<CAntiAim>();