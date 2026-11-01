#pragma once

class CViewSetup
{
	char _pad0[0x490];
public:
	float m_flOrthoLeft;
	float m_flOrthoTop;
	float m_flOrthoRight;
	float m_flOrthoBottom;
private: char _pad1[0x38]; public:
	float m_flFov;
	float m_flFovViewmodel;
	Vector m_vecOrigin;
private: char _pad2[0xC]; public:
	QAngle m_angView;
private: char _pad3[0x14]; public:
	float m_flAspectRatio;
private: char _pad4[0x79]; public:
	BYTE m_nSomeFlags;
};

struct CViewRender
{
	MEM_PAD(0x8);
	CViewSetup m_viewSetup;

	virtual void Function0();
};

class IClientModeShared
{
public:
};