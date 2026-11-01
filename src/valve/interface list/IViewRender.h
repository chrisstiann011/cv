#pragma once

class IViewRender
{
public:
	Vector m_vecOrigin;
	QAngle m_vecAngles;
	float m_flFov;
private:
	char pad_0x001C[ 0x14 ];
	ViewMatrix_t matUNK1;
	char pad_0x0070[ 0x30 ];
	ViewMatrix_t matUNK2;
	char pad_0x00E0[ 0xC8 ];
	ViewMatrix_t matUNK3;
	char pad_0x01E8[ 0x28 ];
};