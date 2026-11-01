#pragma once

struct MaterialParamater_t
{
	Vector4D m_vecValue;
	void* m_pTextureValue;
	MEM_PAD(0x10);
	const char* m_szParamName;
	const char* m_szValue;
	std::int64_t m_nValue;
};

class CMaterial2
{
public:
	const char* GetName()
	{
		return Memory::CallVFunc<const char*, 0U>(this);
	}

	const char* GetShaderName()
	{
		return Memory::CallVFunc<const char*, 1U>(this);
	}

	MaterialParamater_t* FindParamater(const char* szName)
	{
		using fnFindParamater = MaterialParamater_t*(__fastcall*)(CMaterial2*, const char*);
		static const auto oFindParamater = reinterpret_cast<fnFindParamater>(Memory::FindPattern(MATERIAL_SYSTEM2_DLL, X("48 89 5C 24 ?? 48 89 74 24 ?? 57 48 83 EC 20 48 8B 59 18")));

		return oFindParamater(this, szName);

	}

	void UpdateParamater()
	{
		using fnUpdateParamater = MaterialParamater_t*(__fastcall*)(CMaterial2*);

		static const auto oUpdateParamater = reinterpret_cast<fnUpdateParamater>(Memory::FindPattern(MATERIAL_SYSTEM2_DLL, X("48 89 7C 24 ? 41 56 48 83 EC ? 8B 81")));

		oUpdateParamater(this);

	}

	void SetVecParamater(const char* szName, Vector4D vecValue)
	{
		const auto pParamater = FindParamater(szName);
		if (!pParamater)
			return;

		pParamater->m_vecValue = vecValue;
	}

	void SetTextureParameter(const char* szName, void* szParam)
	{
		const auto pParameter = FindParamater(szName);
		if (!pParameter)
			return;

		if (!pParameter->m_pTextureValue)
			return;

		pParameter->m_pTextureValue = szParam;
	}
};

struct MaterialArray_t
{
	uint64_t m_nigCount;
	CMaterial2*** m_pResource;
	uint64_t pad_0010[3];
};

class CObjectInfo
{
	MEM_PAD(0xB0);
	int m_nId;
};

class CSceneObject {
public:
	void* vftable;
	void* heapPtr;
	void* m_pMeshInstanceData;
	void* m_pDesc;
	void* m_pRefData;
private:
	MEM_PAD(0x8);

public:
	Matrix3x4_t m_transform;
private:
	MEM_PAD(0x26);

public:
	uint16_t m_nObjectTypeFlags;
	uint16_t m_nGameRenderCounter;
	uint8_t m_clr[3];
private:
	MEM_PAD(0x3);

public:
	void* m_pPVS;
	void* m_pExtraData;
private:
	MEM_PAD(0x10);

public:
	void* m_pWorld;
	CHandle<C_BaseEntity> m_hExternalOwner;
private:
	MEM_PAD(0x14);
};

class CAnimatableSceneObjectDesc;
class CSceneAnimatableObject
{
public:

	OFFSET( int, GetBoneCount, 0xD0 );
	OFFSET( Matrix3x4_t*, GetRenderBones, 0xD8 );

	MEM_PAD( 16 );
	void* m_pMeshInstance{ };
	void* m_pDesc{ };
	MEM_PAD( 16 );
	Matrix3x4_t m_matTranslationMatrix{ };
	MEM_PAD( 88 );
	CHandle<C_BaseEntity> m_hOwnerHandle{ };
	MEM_PAD( 80 );
	CSkeletonInstance* m_pSkeleton{ };
	MEM_PAD( 24 );
};

class CSceneData
{
public:
	char pad_0000[ 0x18 ];
	CSceneAnimatableObject* m_pSceneObject;
	CMaterial2* m_pMaterial;
	CMaterial2* m_pMaterial2;
	char pad_0030[ 32 ];
	Color m_cColor;
	char pad_0054[ 16 ];
};

class CMeshPrimitiveOutputBuffer
{
public:
	CSceneData* m_pMeshPrimitive;
	char __1[ 0x4 ];
	int m_nCount;

	CSceneData* GetPrimitive( int nIndex ) noexcept
	{
		return &m_pMeshPrimitive[ nIndex ];
	}
};

class CAnimatableSceneObjectDesc
{
public:
	CSceneAnimatableObject* CreateSceneAnimatableObject()
	{
		return Memory::CallVFunc<CSceneAnimatableObject*, 21>(this);
	}

	void* CreateBaseSceneObjectDesc(bool bAllocate)
	{
		return Memory::CallVFunc<void*, 22>(this, bAllocate);
	}
};

class CAggregateSceneObjectDataWorld {
private:
	char pad_0000[0x38];
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
private:
	char pad_0038[0x9];
};

class CAggregateSceneObjectWorld {
private:
	char pad_0000[0x120];
public:
	int count;
private:
	char pad_0120[0x4];
public:
	CAggregateSceneObjectDataWorld* array;
};

class CAggregateSceneObject
{
public:
	unsigned char pad_0[0xE4];
	float red;
	float green;
	float blue;
};
class c_aggregate_object_data
{
public:
	char pad_0000[4];
	int count;
	char pad_0008[40];
	int index;
};

class c_aggregate_object_array
{
public:
	void* object;
	c_aggregate_object_data* data;
};

class C_AggregateSceneObjectData
{
private:
	char pad_0000[0x38];
public:
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;
private:
	char pad_0038[0x9];
};

class C_AggregateSceneObject
{
private:
	char pad_0000[0x120];
public:
	int m_nCount;
private:
	char pad_0120[0x4];
public:
	C_AggregateSceneObjectData* m_pData;

};

class CDrawMesh
{
public:

	MEM_PAD(0x18);
	CSceneAnimatableObject* m_pSceneAnimatableObject;
	CMaterial2* m_pMaterial;
	MEM_PAD(0x28);
	Color m_colValue;
	MEM_PAD(0x4);
	CObjectInfo* m_pObjectInfo;
	MEM_PAD(0x4);
	std::int32_t m_nDrawFlag;

};

class IMaterialSystem2
{
public:
	CMaterial2*** FindOrCreateFromResource(CMaterial2*** pOutMaterial, const char* szMaterialName)
	{
		return Memory::CallVFunc<CMaterial2***, 14U>(this, pOutMaterial, szMaterialName);
	}

	CMaterial2** CreateMaterial(CMaterial2*** pOutMaterial, const char* szMaterialName, CDrawMesh* pData)
	{
		return Memory::CallVFunc<CMaterial2**, 29U>(this, pOutMaterial, szMaterialName, pData, 0, 0, 0, 0, 0, 1);
	}

	void SetCreateDataByMaterial(const void* pData, CMaterial2*** const pInMaterial)
	{
		return Memory::CallVFunc<void, 37U>(this, pInMaterial, pData);
	}
};