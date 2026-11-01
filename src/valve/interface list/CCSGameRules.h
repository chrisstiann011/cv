#pragma once

#include "../Schema.h"

class CViewVectors
{
public:
	Vector m_vecView;
	Vector m_vecHullMin;
	Vector m_vecHullMax;
	Vector m_vecDuckHullMin;
	Vector m_vecDuckHullMax;
	Vector m_vecDuckView;
	Vector m_vecOBSHullMin;
	Vector m_vecOBSHullMax;
	Vector m_vecDeadView;
};

class C_CSGameRules
{
public:
	CViewVectors* GetViewVectors()
	{
		return Memory::CallVFunc<CViewVectors*, 38U>(this);
	}
public:
	SCHEMA_ADD_FIELD(bool, m_bFreezePeriod, "C_CSGameRules->m_bFreezePeriod");
	SCHEMA_ADD_FIELD(bool, m_bWarmupPeriod, "C_CSGameRules->m_bWarmupPeriod");
	SCHEMA_ADD_FIELD(bool, m_bTeamIntroPeriod, "C_CSGameRules->m_bTeamIntroPeriod");
	SCHEMA_ADD_FIELD(bool, m_bIsValveDS, "C_CSGameRules->m_bIsValveDS");
	SCHEMA_ADD_FIELD(bool, m_bHasMatchStarted, "C_CSGameRules->m_bHasMatchStarted");
	SCHEMA_ADD_FIELD(float, m_fRoundStartTime, "C_CSGameRules->m_fRoundStartTime");
};