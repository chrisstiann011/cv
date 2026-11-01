#include "../Precompiled.h"

struct SchemaData_t
{
	FNV1A_t uHashedFieldName = 0x0ULL;
	std::uint32_t uOffset = 0x0U;
};

static std::vector<SchemaData_t> vecSchemaData;
void Schema::DumpSchemas(CSchemaSystemTypeScope* pTypeScope, std::filesystem::path fsPath, bool bAddToVector)
{
	const int nTableSize = pTypeScope->declared_classes_num;
	if (nTableSize == 0)
	{
		Logging::PushConsoleColor(FOREGROUND_YELLOW);
		Logging::Print(X("[schema] skipping {}, no entries...."), pTypeScope->szName);
		Logging::PopConsoleColor();
		return;
	}

	Logging::PushConsoleColor(FOREGROUND_YELLOW);
	Logging::Print(X("[schema] found: {} schema in {}"), nTableSize, pTypeScope->szName);
	Logging::PopConsoleColor();

	HANDLE hOutFile = ::CreateFileW(fsPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hOutFile == INVALID_HANDLE_VALUE)
		return;

	const std::time_t time = std::time(nullptr);
	std::tm timePoint;
	localtime_s(&timePoint, &time);

	CRT::String_t<64> szTimeBuffer(X("[%d-%m-%Y %T] RyzeXTR | schema dump\n\n"), &timePoint);
	::WriteFile(hOutFile, szTimeBuffer.Data(), static_cast<DWORD>(szTimeBuffer.Length()), nullptr, nullptr);

	for (uint16_t c = 0; c < pTypeScope->declared_classes_num; ++c)
	{

		declared_class_entry& entry = pTypeScope->declared_classes[c];
		declared_class* pDeclaredClassInfo = entry.declared_classes;

		if (!pDeclaredClassInfo)
			continue;

		if (!pDeclaredClassInfo || !pDeclaredClassInfo->schema_class)
			continue;

		const char* class_name = pDeclaredClassInfo->schema_class->name;
		if (!class_name)
			continue;

		schema_field* fields = pDeclaredClassInfo->schema_class->fields;
		uint16_t fields_count = pDeclaredClassInfo->schema_class->fields_num;

		if (!fields)
			continue;

		for (uint16_t f = 0; f < fields_count; ++f)
			
		{
			const char* field_name = fields[f].name;
			if (!field_name)
				continue;

			CRT::String_t<MAX_PATH> szFieldClassBuffer(X("%s->%s"), class_name, field_name);

			if (bAddToVector)
				vecSchemaData.emplace_back(FNV1A::Hash(szFieldClassBuffer.Data()), fields[f].offset);

			CRT::String_t<MAX_PATH> szFieldBuffer(X("    %s %s = 0x%X;\n"), class_name, field_name, fields[f].offset);

			::WriteFile(hOutFile, szFieldBuffer.Data(), static_cast<DWORD>(szFieldBuffer.Length()), nullptr, nullptr);
		}

		CRT::String_t<MAX_PATH> szBracket2Buffer(X("%s; \n\n"), X("}"));

		::WriteFile(hOutFile, szBracket2Buffer.Data(), static_cast<DWORD>(szBracket2Buffer.Length()), nullptr, nullptr);

	}

	::CloseHandle(hOutFile);
}

void Schema::DumpEnums(CSchemaSystemTypeScope* pTypeScope, std::filesystem::path fsPath)
{

}

bool Schema::Setup()
{
	std::filesystem::path fsSchemaPath = Config::fsSchemasPath;
	std::filesystem::path fsEnumPath = Config::fsEnumsPath;

	DumpSchemas(Interfaces::m_pSchemaSystem->GlobalTypeScope(), fsSchemaPath.append(X("!GlobalTypes.hpp")), false);

	CSchemaSystemTypeScope* pClientTypeScope = Interfaces::m_pSchemaSystem->FindTypeScopeForModule(X("client.dll"));
	if (!pClientTypeScope)
		return false;

	std::string strScopeName = pClientTypeScope->szName;
	size_t pos = strScopeName.find(X(".dll"));

	if (pos != std::string_view::npos)
		strScopeName = strScopeName.substr(0, pos);

	strScopeName.append(X(".hpp"));
	fsSchemaPath = Config::fsSchemasPath;

	DumpSchemas(pClientTypeScope, fsSchemaPath.append(strScopeName), true);

	return true;
}

std::uint32_t Schema::GetOffset(const FNV1A_t uHashedFieldName)
{
	if (const auto it = std::ranges::find_if(vecSchemaData, [uHashedFieldName](const SchemaData_t& data) { return data.uHashedFieldName == uHashedFieldName; }); it != vecSchemaData.end())
		return it->uOffset;

	Logging::PushConsoleColor(FOREGROUND_INTENSE_RED);
	Logging::Print(X("failed to find offset for field with hash: {}"), uHashedFieldName);
	Logging::PopConsoleColor();

	return 0U;
}

