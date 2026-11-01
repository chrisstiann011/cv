#pragma once

class CUserCmd;

class CMovement
{
private:

	bool m_bWants2DMoveHalt = false;

	void DirectionalAirStrafe(Vector vVelocity, float flCurrentSpeed, float flFrameTime, bool bSubtickCalling);

	void SubtickAirStrafe();

public:

	void AirAccelerate(Vector& vVelocity, Vector vMoveImpulse, float& flStamina, float flFrameTime, float flYaw);

	QAngle m_angCameraAngles = {};
	QAngle m_angModelAngles = {};

	void MovementCorrection();

	void StopMovement();

	void BunnyHop2(CUserCmd* user_cmd);

	void BunnyHop();
	void JumpBug();

	void HandleMoveStop(bool bHaltMovement);
	void HandleDirectionalStrafe( );
	void EdgeJump(CUserCmd* pCmd);
	Vector SimulatePosAtStop(Vector vPos);

	void StoreCameraAngles() {
		this->m_angCameraAngles = Interfaces::m_pInput->GetViewAngles();
	}

	void StoreModelAngles() {
		this->m_angModelAngles = QAngle(Globals::m_pBaseCmd->viewangles().x(), Globals::m_pBaseCmd->viewangles().y(), 0.f);
	}

	void SetWants2DMoveHalt(bool bVal) { m_bWants2DMoveHalt = bVal; };
};

inline auto g_Movement = std::make_unique<CMovement>();