#pragma once
#include <filesystem>

#pragma region config_definitions
#define C_ADD_VARIABLE( Type, szName, pDefault ) const std::uint32_t szName = Config::AddVariable<Type>(FNV1A::HashConst(#szName), FNV1A::HashConst(#Type), pDefault);
#define C_ADD_VARIABLE_VECTOR( Type, uSize, szName, pDefault ) const std::uint32_t szName = Config::AddVariable<std::vector<Type>>(FNV1A::HashConst(#szName), FNV1A::HashConst("std::vector<" #Type ">"), Config::GetFilledVector<Type, uSize>(pDefault));
#define C_ADD_VECTOR_HITBOX(szName, pDefault) const std::uint32_t szName = Config::AddVectorHitbox(FNV1A::HashConst(#szName), pDefault);
#define C_INVALID_VARIABLE (std::size_t)(-1)
#pragma endregion

struct KeyBind_t
{
	unsigned int m_iKey = 0;
	unsigned int m_iMode = 0;
};

struct VariableObject_t
{
	VariableObject_t() = default;

	explicit VariableObject_t(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const std::any pDefault)
		: uNameHash(uNameHash), uTypeHash(uTypeHash), pValue(pDefault) { }

	~VariableObject_t() = default;

	template<typename T>
	T& Get()
	{
		return *(T*)(std::any_cast<T>(&pValue));
	}

	template<typename T>
	void Set(T value)
	{
		pValue.emplace<T>(value);
	}

	FNV1A_t		uNameHash;
	FNV1A_t		uTypeHash;
	std::any	pValue;
};
enum EConfigHitBoxes : unsigned int
{
	HB_HEAD = 0,
	HB_CHEST,
	HB_STOMACH,
	HB_PELVIS,
	HB_ARMS,
	HB_LEGS,
	HB_FEET,
	HB_MAX
};

struct MenuRageHitbox_t {
	bool m_bMultipoint = false;
	EConfigHitBoxes m_eDefinition;
};

namespace Config
{

	bool Setup(std::string_view szDefaultFileName);
	
	bool Save(std::string_view szFileName, bool bInventory);
	
	bool Load(std::string_view szFileName, bool bInventory);
	
	void Remove(std::string_view szFileName);
	
	void Refresh();

	std::size_t GetVariableIndex(const FNV1A_t uNameHash);
	
	std::filesystem::path GetWorkingPath();

	bool ExportToClipboard(const std::string& strFile);
	bool ImportFromClipboard(const std::string& strFile);

	const std::filesystem::path fsConfigPath = GetWorkingPath() / X("Settings") / X("Internal") / X("Configs");
	const std::filesystem::path fsInventoryPath = GetWorkingPath() / X("Settings") / X("Internal") / X("Inventory");
	const std::filesystem::path fsSchemasPath = GetWorkingPath() / X("Schemas");
	const std::filesystem::path fsEnumsPath = GetWorkingPath() / X("Enums");

	inline std::deque<std::string> vecFileNames = { };

	inline std::deque<std::string> vecInventoryFileNames = { };

	inline std::deque<std::string> vecSkinFileNames = { };

	inline std::vector<VariableObject_t> vecVariables = { };

	template <typename T>
	T& Get(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<T>();
	}

	inline std::vector<bool>& vb(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<std::vector<bool>>();
	}

	inline std::vector<int>& vi(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<std::vector<int>>();
	}

	inline std::vector<float>& vf(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<std::vector<float>>();
	}

	inline std::vector<Color>& vc(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<std::vector<Color>>();
	}

	inline std::vector<std::string>& vstr(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<std::vector<std::string>>();
	}

	inline bool& b(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<bool>();
	}

	inline int& i(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<int>();
	}

	inline unsigned int& ui(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<unsigned int>();
	}

	inline float& f(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<float>();
	}

	inline Color& c(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<Color>();
	}

	inline KeyBind_t& kb(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<KeyBind_t>();
	}

	inline std::string& str(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).Get<std::string>();
	}

	inline std::uint32_t AddVectorHitbox(const FNV1A_t uNameHash, const std::vector<MenuRageHitbox_t>& pDefault)
	{
		vecVariables.emplace_back(uNameHash, FNV1A::HashConst("std::vector<MenuRageHitbox_t>"), std::make_any<std::vector<MenuRageHitbox_t>>(pDefault));
		return static_cast<std::uint32_t>(vecVariables.size() - 1U);
	}

	template<typename T>
	inline std::vector<T>& vhb(const std::uint32_t nIndex)
	{
		return vecVariables.at(nIndex).template Get<std::vector<T>>();
	}

	template <typename T>
	std::uint32_t AddVariable(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, T pDefault)
	{
		vecVariables.emplace_back(uNameHash, uTypeHash, std::make_any<T>(pDefault));
		return static_cast<std::uint32_t>(vecVariables.size() - 1U);
	}

	template <typename T, std::size_t S>
	std::vector<T> GetFilledVector(const T& fill)
	{
		std::vector<T> vecTemp(S);
		std::fill(vecTemp.begin(), vecTemp.begin() + S, fill);
		return vecTemp;
	}

}