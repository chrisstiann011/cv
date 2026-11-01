#pragma once

#define SCHEMASYSTEM_TYPE_SCOPES_OFFSET 0x190
#define SCHEMASYSTEMTYPESCOPE_OFF1 0x24
#define SCHEMASYSTEMTYPESCOPE_OFF2 0x8

using SchemaString_t = const char*;
struct SchemaMetadataEntryData_t;

class CSchemaSystemTypeScope;
class CSchemaType;

struct CSchemaClassBinding
{
	CSchemaClassBinding* pParent;
	const char* szBinaryName;
	const char* szModuleName;
	const char* szClassName;
	void* pClassInfoOldSynthesized;
	void* pClassInfo;
	void* pThisModuleBindingPointer;
	CSchemaType* pSchemaType;
};

class CSchemaType
{
public:
	bool GetSizes(int* pOutSize, uint8_t* unkPtr)
	{
		return Memory::CallVFunc<bool, 3U>(this, pOutSize, unkPtr);
	}

	bool GetSize(int* pOutSize)
	{
		uint8_t smh = 0;
		return GetSizes(pOutSize, &smh);
	}
public:
	void* pVtable;
	const char* szName;
	CSchemaSystemTypeScope* pSystemTypeScope;
	uint8_t nTypeCategory;
	uint8_t nAatomicCategory;
};

struct SchemaClassFieldData_t
{
	SchemaString_t szName;
	CSchemaType* pSchemaType;
	std::uint32_t nSingleInheritanceOffset;
	std::int32_t nMetadataSize;
	SchemaMetadataEntryData_t* pMetaData;
};

struct SchemaClassInfoData_t;
struct SchemaStaticFieldData_t;
struct SchemaBaseClassInfoData_t
{
	unsigned int uOffset;
	SchemaClassInfoData_t* pClass;
};

struct SchemaClassInfoData_t
{
	MEM_PAD(0x8);

	const char* szName;
	char* szModuleName;

	int m_nSize;
	std::int16_t nFieldSize;

	std::int16_t nStaticSize;
	std::int16_t nMetadataSize;
	std::uint8_t nAlignOf;
	std::uint8_t nBaseClassesCount;
	MEM_PAD(0x4);
public:
	SchemaClassFieldData_t* pFields;
	SchemaStaticFieldData_t* pStaticFields;
	SchemaBaseClassInfoData_t* pSchemaParent;

	bool InheritsFrom(SchemaClassInfoData_t* pClassInfo)
	{
		if (pClassInfo == this && pClassInfo)
			return true;
		else if (!pSchemaParent || !pClassInfo)
			return false;

		for (int i = 0; i < nBaseClassesCount; i++)
		{
			SchemaBaseClassInfoData_t& baseClass = pSchemaParent[i];
			if (baseClass.pClass->InheritsFrom(pClassInfo))
				return true;
		}

		return false;
	}
};

struct SchemaEnumeratorInfoData_t
{
	SchemaString_t szName;

	union
	{
		unsigned char value_char;
		unsigned short value_short;
		unsigned int value_int;
		unsigned long long value;
	};

	MEM_PAD(0x10);
};

class CSchemaEnumInfo
{
public:
	SchemaEnumeratorInfoData_t enumInfoData;
};

class CSchemaEnumBinding
{
public:
	virtual const char* GetBindingName() = 0;
	virtual CSchemaClassBinding* AsClassBinding() = 0;
	virtual CSchemaEnumBinding* AsEnumBinding() = 0;
	virtual const char* GetBinaryName() = 0;
	virtual const char* GetProjectName() = 0;

public:
	char* szBindingName_;
	char* szDllName_;
	std::int8_t nAlign_;
	MEM_PAD(0x3);
	std::int16_t nSize_;
	std::int16_t nFlags_;
	SchemaEnumeratorInfoData_t* pEnumInfo_;
	MEM_PAD(0x8);
	CSchemaSystemTypeScope* pTypeScope_;
	MEM_PAD(0x8);
	std::int32_t unk1_;
};

template <class K, class V>
class CSchemaPtrMap
{
public:
	CUtlMap<K, V> m_Map;
	CThreadFastMutex m_Mutex;
};

class schema_field
{
public:
	const char* name;
	void* type;
	uint32_t offset;
	uint32_t metadata_size;
	void* metadata;
};

class some_class
{
public:
	void* vtable;
	const char* name;
	const char* module_name;
	uint32_t size;
	uint16_t fields_num;
	MEM_PAD(2);
	uint16_t staticSize;
	uint16_t metadataSize;
	MEM_PAD(4);
	schema_field* fields;
};

class declared_class
{
public:
	void* vtable;
	const char* name;
	const char* module_name;
	const char* unknown_str;
	some_class* schema_class;
};

class declared_class_entry
{
public:
	uint64_t hash[2];
	declared_class* declared_classes;
};

class CSchemaSystemTypeScope
{
public:
	void FindDeclaredClass(SchemaClassInfoData_t** pReturnClass, const char* szClassName)
	{
		return Memory::CallVFunc<void, 2U>(this, pReturnClass, szClassName);
	}

	void* pVtable;
	char szName[256U];
	MEM_PAD(0x368);
	uint16_t declared_classes_num;
	MEM_PAD(6);
	declared_class_entry* declared_classes;
};

class ISchemaSystem
{
public:
	CSchemaSystemTypeScope* GlobalTypeScope()
	{
		return Memory::CallVFunc<CSchemaSystemTypeScope*, 11U>(this);
	}

	CSchemaSystemTypeScope* FindTypeScopeForModule(const char* szModuleName)
	{
		return Memory::CallVFunc<CSchemaSystemTypeScope*, 13U>(this, szModuleName, nullptr);
	}

	MEM_PAD(SCHEMASYSTEM_TYPE_SCOPES_OFFSET);
	CUtlVector<CSchemaSystemTypeScope*> vecTypeScopes;
};