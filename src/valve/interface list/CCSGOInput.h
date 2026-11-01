#pragma once

#define MULTIPLAYER_BACKUP 150

class CCSInputMessage
{
public:
	std::int32_t m_nFrameTickCount;
	float m_flFrameTickFraction;
	std::int32_t m_nPlayerTickCount;
	float m_flPlayerTickFraction;
	QAngle m_angViewAngles;
	Vector m_vecShootPosition;
	std::int32_t m_nTargetIndex;
	Vector m_vecTargetHeadPosition;
	Vector m_vecTargetAbsOrigin;
	Vector m_vecTargetAngle;
	std::int32_t m_nsvShowHitRegistration;
	std::int32_t m_nEntryIndexMax;
	std::int32_t m_nIndexUnk;
	int idekjustpadding;
};
static_assert(sizeof(CCSInputMessage) == 0x60);

class CTinyMoveStepData
{
public:
	float m_flWhen;
	MEM_PAD(0x4);
	std::uint64_t m_nButton;
	bool m_bPressed;
	MEM_PAD(0x7);
};

class CMoveStepButtons
{
public:
	std::uint64_t m_nKeyboardPressed;
	std::uint64_t m_nMouseWheelheelPressed;
	std::uint64_t m_nUnPressed;
	std::uint64_t m_nKeyboardCopy;
};

class CExtendedMoveData : public CMoveStepButtons
{
public:
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;
	std::int32_t m_nMouseDeltaX;
	std::int32_t m_nMouseDeltaY;
	std::int32_t m_nAdditionalStepMovesCount;
	CTinyMoveStepData m_TinyMoveStepData[12];
	Vector m_vecViewAngle;
	std::int32_t m_nTargetHandle;
};
class c_button_data
{
public:
	std::uint64_t flag;
	char* name;
	std::byte m_pad0[0x8];
	float value;
	std::byte m_pad1[0x4];
	float when_start;
	float when_end;
	std::byte m_pad2[0x4];
	int command_count;
	std::byte m_pad3[0x60];
};
class CCSGOInput
{
public:
	char pad_0000[16];
	c_button_data* buttons_data[63];
	char pad_0208[72];
	bool bBlockShot;
	bool m_bInThirdPerson;
	MEM_PAD(0x6);
	QAngle angThirdPersonAngles;
	MEM_PAD(0x14);
	uint64_t nKeyboardPressed;
	uint64_t nMouseWheelPressed;
	uint64_t nUnPressed;
	uint64_t nKeyboardCopy;
	float flForwardMove;
	float flSideMove;
	float flUpMove;
	Vector2D nMousePos;
	int32_t SubticksCount;
	MEM_PAD(0x120);
	Vector vecViewAngle;
	int32_t nTargetHandle;
	MEM_PAD(0x230);
	int32_t nAttackStartHistoryIndex1;
	int32_t nAttackStartHistoryIndex2;
	int32_t nAttackStartHistoryIndex3;
	char pad_061C[4];
	int32_t m_nMessageSize;
	char pad_0624[6];
	CCSInputMessage* m_pMessage;

	void SetViewAngle(const QAngle angView)
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnSetViewAngle != nullptr);
		#endif

		Functions::fnSetViewAngle(this, 0, angView);
	}

	QAngle GetViewAngles()
	{
		#ifdef CS_PARANOID
		CS_ASSERT(Functions::fnGetViewAngles != nullptr);
		#endif

		return *reinterpret_cast<QAngle*>(Functions::fnGetViewAngles(this, 0));
	}

	inline CSubtickMoveStep* CreateNewSubTickMoveStep(google::protobuf::RepeatedPtrField<CSubtickMoveStep>* pMutableSubtickMoveStep)
	{
		return Functions::fnCreateNewSubtickMoveStep(pMutableSubtickMoveStep->GetArena());
	}
};
