#pragma once
#include "../../memory/datatypes/CUtlLinkedListsold.h"
#pragma region convar_enumerations

enum EConVarFlag : int
{

	FCVAR_NONE = 0,
	FCVAR_UNREGISTERED = (1 << 0),
	FCVAR_DEVELOPMENTONLY = (1 << 1),
	FCVAR_GAMEDLL = (1 << 2),
	FCVAR_CLIENTDLL = (1 << 3),
	FCVAR_HIDDEN = (1 << 4),

	FCVAR_PROTECTED = (1 << 5),
	FCVAR_SPONLY = (1 << 6),
	FCVAR_ARCHIVE = (1 << 7),
	FCVAR_NOTIFY = (1 << 8),
	FCVAR_USERINFO = (1 << 9),
	FCVAR_CHEAT = (1 << 14),
	FCVAR_PRINTABLEONLY = (1 << 10),
	FCVAR_UNLOGGED = (1 << 11),
	FCVAR_NEVER_AS_STRING = (1 << 12),

	FCVAR_REPLICATED = (1 << 13),

	FCVAR_DEMO = (1 << 16),
	FCVAR_DONTRECORD = (1 << 17),
	FCVAR_RELOAD_MATERIALS = (1 << 20),
	FCVAR_RELOAD_TEXTURES = (1 << 21),
	FCVAR_NOT_CONNECTED = (1 << 22),
	FCVAR_MATERIAL_SYSTEM_THREAD = (1 << 23),
	FCVAR_ARCHIVE_XBOX = (1 << 24),
	FCVAR_ACCESSIBLE_FROM_THREADS = (1 << 25),
	FCVAR_SERVER_CAN_EXECUTE = (1 << 28),
	FCVAR_SERVER_CANNOT_QUERY = (1 << 29),
	FCVAR_CLIENTCMD_CAN_EXECUTE = (1 << 30),
	FCVAR_MATERIAL_THREAD_MASK = (FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES | FCVAR_MATERIAL_SYSTEM_THREAD)
};

enum EConVarType : short
{
	EConVarType_Invalid = -1,
	EConVarType_Bool,
	EConVarType_Int16,
	EConVarType_UInt16,
	EConVarType_Int32,
	EConVarType_UInt32,
	EConVarType_Int64,
	EConVarType_UInt64,
	EConVarType_Float32,
	EConVarType_Float64,
	EConVarType_String,
	EConVarType_Color,
	EConVarType_Vector2,
	EConVarType_Vector3,
	EConVarType_Vector4,
	EConVarType_Qangle,
	EConVarType_MAX
};

#pragma endregion

union CVValue_t
{
	bool m_i1;
	short m_i16;
	uint16_t m_u16;
	int m_i32;
	uint32_t m_u32;
	int64_t m_i64;
	uint64_t m_u64;
	float m_fl;
	double m_db;
	const char* m_sz;
	Color m_clr;
	Vector2D m_vec2;
	Vector m_vec3;
	Vector4D m_vec4;
	QAngle m_ang;
};

class CConVar
{
public:
	const char* m_szName;
	CConVar* m_m_pNext;
	MEM_PAD(0x10);
	const char* m_szDescription;
	uint32_t m_nType;
	uint32_t m_nRegistered;
	uint32_t m_nFlags;
	MEM_PAD(0x28);

	CVValue_t m_value;

	inline bool GetBool() const
	{
		return m_value.m_i1;
	}

	inline int GetInt() const
	{
		return m_value.m_i32;
	}

	inline float GetFloat() const
	{
		return m_value.m_fl;
	}

	inline Color GetColor() const
	{
		return m_value.m_clr;
	}
};

class IEngineCVar
{
public:
	MEM_PAD(0x40);
	CUtlLinkedLists<CConVar*> listConvars;

	CConVar* Find(FNV1A_t uHashedName)
	{
		for (auto i = Interfaces::m_pCvar->listConvars.m_Head; i != Interfaces::m_pCvar->listConvars.kInvalidIndex;) {

			CConVar* pConVar = Interfaces::m_pCvar->listConvars.memory[i].m_Element;
			if (pConVar == nullptr)
				continue;

			i = Interfaces::m_pCvar->listConvars.memory[i].m_Next;

			if (FNV1A::Hash(pConVar->m_szName) == uHashedName)
				return pConVar;

		}

		CS_ASSERT(false);
		return nullptr;
	}

	void UnlockHiddenCVars()
	{
		for (auto i = Interfaces::m_pCvar->listConvars.m_Head; i != Interfaces::m_pCvar->listConvars.kInvalidIndex;) {

			CConVar* pConVar = Interfaces::m_pCvar->listConvars.memory[i].m_Element;
			if (pConVar == nullptr)
				continue;

			if (pConVar->m_nFlags & FCVAR_HIDDEN)
				pConVar->m_nFlags &= ~FCVAR_HIDDEN;

			if (pConVar->m_nFlags & FCVAR_DEVELOPMENTONLY)
				pConVar->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;

			i = Interfaces::m_pCvar->listConvars.memory[i].m_Next;
		}
	}
};