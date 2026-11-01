#pragma once

#define SCHEMA_ADD_OFFSET(TYPE, NAME, OFFSET)                                                                 \
	[[nodiscard]] __forceinline std::add_lvalue_reference_t<TYPE> NAME()                                      \
	{                                                                                                         \
		static const std::uint32_t uOffset = OFFSET;                                                          \
		return *reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (uOffset));\
	}

#define SCHEMA_ADD_POFFSET(TYPE, NAME, OFFSET)                                                               \
	[[nodiscard]] __forceinline std::add_pointer_t<TYPE> NAME()                                              \
	{                                                                                                        \
		const static std::uint32_t uOffset = OFFSET;                                                         \
		return reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (uOffset));\
	}

#define SCHEMA_ADD_FIELD_OFFSET(TYPE, NAME, FIELD, ADDITIONAL) SCHEMA_ADD_OFFSET(TYPE, NAME, Schema::GetOffset(FNV1A::HashConst(FIELD)) + ADDITIONAL)
#define SCHEMA_ADD_FIELD(TYPE, NAME, FIELD) SCHEMA_ADD_FIELD_OFFSET(TYPE, NAME, FIELD, 0U)
#define SCHEMA_ADD_PFIELD_OFFSET(TYPE, NAME, FIELD, ADDITIONAL) SCHEMA_ADD_POFFSET(TYPE, NAME, Schema::GetOffset(FNV1A::HashConst(FIELD)) + ADDITIONAL)
#define SCHEMA_ADD_PFIELD(TYPE, NAME, FIELD) SCHEMA_ADD_PFIELD_OFFSET(TYPE, NAME, FIELD, 0U)

namespace Schema
{

	bool Setup();

	void DumpSchemas(CSchemaSystemTypeScope* pTypeScope, std::filesystem::path fsPath, bool bAddToVector);

	void DumpEnums(CSchemaSystemTypeScope* pTypeScope, std::filesystem::path fsPath);

	[[nodiscard]] std::uint32_t GetOffset( const FNV1A_t uHashedFieldName );

}