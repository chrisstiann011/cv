#include "../Precompiled.h"

bool Config::Setup(std::string_view szDefaultFileName)
{
	if (!std::filesystem::is_directory(fsConfigPath))
	{
		std::filesystem::remove(fsConfigPath);
		if (!std::filesystem::create_directories(fsConfigPath))
			return false;
	}

	if (!std::filesystem::is_directory(fsSchemasPath))
	{
		std::filesystem::remove(fsSchemasPath);
		if (!std::filesystem::create_directories(fsSchemasPath))
			return false;
	}

	if (!std::filesystem::is_directory(fsEnumsPath))
	{
		std::filesystem::remove(fsEnumsPath);
		if (!std::filesystem::create_directories(fsEnumsPath))
			return false;
	}

	if (!std::filesystem::is_directory(fsInventoryPath))
	{
		std::filesystem::remove(fsInventoryPath);
		if (!std::filesystem::create_directories(fsInventoryPath))
			return false;
	}

	if (!Save(szDefaultFileName, false))
		return false;

	if (!Load(szDefaultFileName, false))
		return false;

	Refresh();

	InventoryChanger::OnConfigRefresh();

	return true;
}

bool Config::Save(std::string_view szFileName, bool bInventory)
{

	std::filesystem::path fsFilePath(szFileName);
	if (fsFilePath.extension() != X(".chlen"))
		fsFilePath.replace_extension(X(".chlen"));

	const std::string szFile = std::filesystem::path(fsConfigPath / fsFilePath).string();
	nlohmann::json config = { };

	try
	{
		for (auto& variable : vecVariables)
		{
			nlohmann::json entry = { };

			entry[X("name-id")] = variable.uNameHash;
			entry[X("type-id")] = variable.uTypeHash;

			switch (variable.uTypeHash)
			{
			case FNV1A::HashConst("int"):
			{
				entry[X("value")] = variable.Get<int>();
				break;
			}
			case FNV1A::HashConst("float"):
			{
				entry[X("value")] = variable.Get<float>();
				break;
			}
			case FNV1A::HashConst("bool"):
			{
				entry[X("value")] = variable.Get<bool>();
				break;
			}
			case FNV1A::HashConst("std::string"):
			{
				entry[X("value")] = variable.Get<std::string>();
				break;
			}
			case FNV1A::HashConst("Color"):
			{
				const auto& colVariable = variable.Get<Color>();

				nlohmann::json sub = { };

				sub.push_back(colVariable.Get<COLOR_R>());
				sub.push_back(colVariable.Get<COLOR_G>());
				sub.push_back(colVariable.Get<COLOR_B>());
				sub.push_back(colVariable.Get<COLOR_A>());

				entry[X("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<MenuRageHitbox_t>"):
			{
				const auto& vecHitboxes = variable.Get<std::vector<MenuRageHitbox_t>>();

				nlohmann::json sub = nlohmann::json::array();

				for (const auto& hitbox : vecHitboxes)
				{
					nlohmann::json hitboxEntry = nlohmann::json::object();
					hitboxEntry["multipoint"] = hitbox.m_bMultipoint;
					hitboxEntry["definition"] = static_cast<unsigned int>(hitbox.m_eDefinition);
					sub.push_back(hitboxEntry);
				}

				entry[X("value")] = sub.dump();
				break;
			}

			case FNV1A::HashConst("KeyBind_t"):
			{
				const auto& keybindVariable = variable.Get<KeyBind_t>();

				nlohmann::json sub = { };

				sub.push_back(keybindVariable.m_iKey);
				sub.push_back(keybindVariable.m_iMode);

				entry[X("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<bool>"):
			{
				const auto& vecBools = variable.Get<std::vector<bool>>();

				nlohmann::json sub = { };

				for (const auto&& bValue : vecBools)
					sub.push_back(static_cast<bool>(bValue));

				entry[X("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<int>"):
			{
				const auto& vecInts = variable.Get<std::vector<int>>();

				nlohmann::json sub = { };

				for (const auto& iValue : vecInts)
					sub.push_back(iValue);

				entry[X("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<float>"):
			{
				const auto& vecFloats = variable.Get<std::vector<float>>();

				nlohmann::json sub = { };

				for (const auto& flValue : vecFloats)
					sub.push_back(flValue);

				entry[X("value")] = sub.dump();
				break;
			}
			case FNV1A::HashConst("std::vector<Color>"):
			{
				const auto vecColors = variable.Get<std::vector<Color>>();

				nlohmann::json sub = { };

				for (const auto& colValue : vecColors)
					sub.push_back(colValue.GetHex());

				entry[X("value")] = sub.dump();
				break;
			}

			default:
				break;
			}

			config.push_back(entry);
		}

		if (bInventory) {
			CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
			if (pInventory) {
				for (InventoryItem_t& item : InventoryChanger::m_vecAddedItems)
				{
					if (!&item)
						continue;

					nlohmann::json entry = { };
					entry[X("name-id")] = FNV1A::Hash(item.m_strItemName.c_str());
					entry[X("type-id")] = FNV1A::HashConst("InventoryItem_t");

					item.m_bEquippedCT = false;
					item.m_bEquippedT = false;
					item.m_nEquippedSlot = LOADOUT_SLOT_INVALID;

					for (int i = 0; i <= LOADOUT_SLOT_COUNT; ++i)
					{
						for (int j = TEAM_TT; j <= TEAM_CT; j++)
						{
							C_EconItemView* pItemView = pInventory->GetItemInLoadout(j, i);
							if (!pItemView)
								continue;

							if (pItemView->m_iItemID() == item.m_uItemID)
							{
								if (!item.m_bEquippedCT)
									item.m_bEquippedCT = j == TEAM_CT;

								if (!item.m_bEquippedT)
									item.m_bEquippedT = j == TEAM_TT;

								item.m_nEquippedSlot = i;
							}
						}
					}

					nlohmann::json sub = { };

					sub.push_back(item.m_strItemName);
					sub.push_back(item.m_strSkinName);

					sub.push_back(item.m_nPaintKit);
					sub.push_back(item.m_uItemDefinitionIndex);
					sub.push_back(item.m_uItemRarity);

					sub.push_back(item.m_nStatTrak);
					sub.push_back(item.m_nSeed);
					sub.push_back(item.m_flWear);
					sub.push_back(item.m_strCustomName);

					sub.push_back(item.m_uItemID);

					sub.push_back(item.m_nEquippedSlot);
					sub.push_back(item.m_bEquippedCT);
					sub.push_back(item.m_bEquippedT);

					entry[X("value")] = sub.dump();

					config.push_back(entry);
				}
			}
		}
	}
	catch (const nlohmann::detail::exception& ex)
	{
		Logging::PushConsoleColor(FOREGROUND_RED);
		Logging::Print(X("[error] json save failed: {}"), ex.what());
		Logging::PopConsoleColor();
		return false;
	}

	std::ofstream ofsOutFile(szFile, std::ios::out | std::ios::trunc);

	if (!ofsOutFile.good())
		return false;

	try
	{

		ofsOutFile << config.dump(4);
		ofsOutFile.close();
	}
	catch (std::ofstream::failure& ex)
	{
		Logging::PushConsoleColor(FOREGROUND_RED);
		Logging::Print(X("[error] failed to save configuration: {}"), ex.what());
		Logging::PopConsoleColor();
		return false;
	}

	Logging::Print(X("saved configuration at: {}"), szFile);
	return true;
}

bool Config::Load(std::string_view szFileName, bool bInventory)
{

	const std::string szFile = std::filesystem::path(fsConfigPath / szFileName).string();
	nlohmann::json config = { };

	std::ifstream ifsInputFile(szFile, std::ios::in);

	if (!ifsInputFile.good())
		return false;

	try
	{

		config = nlohmann::json::parse(ifsInputFile, nullptr, false);

		if (config.is_discarded())
			return false;

		ifsInputFile.close();
	}
	catch (std::ifstream::failure& ex)
	{
		Logging::PushConsoleColor(FOREGROUND_RED);
		Logging::Print(X("[error] failed to load configuration: {}"), ex.what());
		Logging::PopConsoleColor();
		return false;
	}

	try
	{
		for (const auto& variable : config)
		{
			const std::size_t nIndex = GetVariableIndex(variable[X("name-id")].get<FNV1A_t>());

			if (nIndex == C_INVALID_VARIABLE)
				continue;

			auto& entry = vecVariables.at(nIndex);

			switch (variable[X("type-id")].get<FNV1A_t>())
			{
			case FNV1A::HashConst("bool"):
			{
				entry.Set<bool>(variable[X("value")].get<bool>());
				break;
			}
			case FNV1A::HashConst("float"):
			{
				entry.Set<float>(variable[X("value")].get<float>());
				break;
			}
			case FNV1A::HashConst("int"):
			{
				entry.Set<int>(variable[X("value")].get<int>());
				break;
			}
			case FNV1A::HashConst("std::string"):
			{
				entry.Set<std::string>(variable[X("value")].get<std::string>());
				break;
			}
			case FNV1A::HashConst("Color"):
			{
				const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());

				entry.Set<Color>(Color(
					vector.at(0).get<std::uint8_t>(),
					vector.at(1).get<std::uint8_t>(),
					vector.at(2).get<std::uint8_t>(),
					vector.at(3).get<std::uint8_t>()
				));

				break;
			}
			case FNV1A::HashConst("std::vector<MenuRageHitbox_t>"):
			{
				const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());
				auto& vecHitboxes = entry.Get<std::vector<MenuRageHitbox_t>>();

				vecHitboxes.clear();

				for (const auto& hitboxEntry : vector)
				{
					MenuRageHitbox_t hitbox;
					hitbox.m_bMultipoint = hitboxEntry["multipoint"].get<bool>();
					hitbox.m_eDefinition = static_cast<EConfigHitBoxes>(hitboxEntry["definition"].get<unsigned int>());
					vecHitboxes.push_back(hitbox);
				}

				break;
			}
			case FNV1A::HashConst("KeyBind_t"):
			{
				const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());

				entry.Set<KeyBind_t>(KeyBind_t(
					vector.at(0).get<unsigned int>(),
					vector.at(1).get<unsigned int>()
				));

				break;
			}
			case FNV1A::HashConst("std::vector<bool>"):
			{
				const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());
				auto& vecBools = entry.Get<std::vector<bool>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{

					if (i < vecBools.size())
						vecBools.at(i) = vector.at(i).get<bool>();
				}

				break;
			}
			case FNV1A::HashConst("std::vector<int>"):
			{
				const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());
				auto& vecInts = entry.Get<std::vector<int>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{

					if (i < vecInts.size())
						vecInts.at(i) = vector.at(i).get<int>();
				}

				break;
			}
			case FNV1A::HashConst("std::vector<float>"):
			{
				const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());
				auto& vecFloats = entry.Get<std::vector<float>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{

					if (i < vecFloats.size())
						vecFloats.at(i) = vector.at(i).get<float>();
				}

				break;
			}
			case FNV1A::HashConst("std::vector<Color>"):
			{
				const nlohmann::json vector = nlohmann::json::parse(variable[X("value")].get<std::string>());
				auto& vecColors = entry.Get<std::vector<Color>>();

				for (std::size_t i = 0U; i < vector.size(); i++)
				{

					if (i < vecColors.size())
						vecColors.at(i) = Color::FromHex(vector.at(i).get<unsigned>());
				}

				break;
			}
			default:
				break;
			}

			if (bInventory)
			{
				CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
				CCSInventoryManager* pInventoryManager = CCSInventoryManager::GetInventoryManager();
				if (pInventory && pInventoryManager)
				{

					for (InventoryItem_t& item : InventoryChanger::m_vecAddedItems)
						pInventory->RemoveEconItem(pInventory->GetSOCDataForItem(item.m_uItemID));

					InventoryChanger::m_vecAddedItems.clear();

					for (const auto& item : config)
					{
						if (item[X("type-id")].get<FNV1A_t>() != FNV1A::HashConst("InventoryItem_t"))
							continue;

						const nlohmann::json vector = nlohmann::json::parse(item[X("value")].get<std::string>());

						InventoryItem_t inventoryItem;

						inventoryItem.m_strItemName = vector.at(0).get<std::string>();
						inventoryItem.m_strSkinName = vector.at(1).get<std::string>();

						inventoryItem.m_nPaintKit = vector.at(2).get<std::int32_t>();
						inventoryItem.m_uItemDefinitionIndex = vector.at(3).get<std::uint16_t>();
						inventoryItem.m_uItemRarity = vector.at(4).get<std::uint16_t>();

						inventoryItem.m_nStatTrak = vector.at(5).get<std::int32_t>();
						inventoryItem.m_nSeed = vector.at(6).get<std::int32_t>();
						inventoryItem.m_flWear = vector.at(7).get<float>();
						inventoryItem.m_strCustomName = vector.at(8).get<std::string>();

						inventoryItem.m_uItemID = vector.at(9).get<uint64_t>();

						inventoryItem.m_nEquippedSlot = vector.at(10).get<std::int32_t>();
						inventoryItem.m_bEquippedCT = vector.at(11).get<bool>();
						inventoryItem.m_bEquippedT = vector.at(12).get<bool>();

						CEconItem* pItem = CEconItem::CreateInstance();
						if (pItem)
						{
							CCSPlayerInventory* pInventory = CCSPlayerInventory::GetPlayerInventory();
							auto HighestIDs = pInventory->GetHighestIDs();
							pItem->m_uID = HighestIDs.first + 1;
							pItem->m_uInventory = HighestIDs.second + 1;
							pItem->m_uAccountID = static_cast<std::uint32_t>(pInventory->GetOwner().m_uID);
							pItem->m_uDefinitionIndex = inventoryItem.m_uItemDefinitionIndex;

							pItem->m_nRarity = inventoryItem.m_uItemRarity;

							if (inventoryItem.m_strCustomName != X("Agent"))
							{
								pItem->SetPaintKit(static_cast<float>(inventoryItem.m_nPaintKit));
								pItem->SetPaintSeed(inventoryItem.m_nSeed);
								pItem->SetPaintWear(inventoryItem.m_flWear);

								if (inventoryItem.m_nStatTrak > 0)
								{
									pItem->SetStatTrak(inventoryItem.m_nStatTrak);
									pItem->SetStatTrakType(0);

									if (pItem->m_nQuality != IQ_UNUSUAL)
										pItem->m_nQuality = IQ_STRANGE;
								}

								if (!inventoryItem.m_strCustomName.empty())
									pItem->SetCustomName(inventoryItem.m_strCustomName.c_str());
							}

							if (pInventory->AddEconItem(pItem))
								InventoryChanger::AddEconItemToList
								(
									InventoryItem_t(
										inventoryItem.m_strItemName,
										inventoryItem.m_strSkinName,
										inventoryItem.m_nPaintKit,
										inventoryItem.m_uItemDefinitionIndex,
										inventoryItem.m_uItemRarity,
										inventoryItem.m_nStatTrak,
										inventoryItem.m_nSeed,
										inventoryItem.m_flWear,
										inventoryItem.m_strCustomName,
										pItem->m_uID,
										inventoryItem.m_nEquippedSlot,
										inventoryItem.m_bEquippedCT,
										inventoryItem.m_bEquippedT
									)

								);

							if (inventoryItem.m_bEquippedCT)
								pInventoryManager->EquipItemInLoadout(TEAM_CT, inventoryItem.m_nEquippedSlot, pItem->m_uID);

							if (inventoryItem.m_bEquippedT)
								pInventoryManager->EquipItemInLoadout(TEAM_TT, inventoryItem.m_nEquippedSlot, pItem->m_uID);
						}
					}
				}
			}
		}
	}
	catch (const nlohmann::detail::exception& ex)
	{
		Logging::PushConsoleColor(FOREGROUND_RED);
		Logging::Print(X("[error] json load failed: {}"), ex.what());
		Logging::PopConsoleColor();
		return false;
	}

	Logging::Print(X("loaded configuration at: {}"), szFile);
	return true;
}

void Config::Remove( std::string_view szFileName )
{
	const std::string szFile = std::filesystem::path(fsConfigPath / szFileName ).string( );

	if (szFileName.compare( X( "Default.chlen" ) ) == 0)
		return;

	if (std::filesystem::remove( szFile ))
		Logging::Print( X( "removed configuration at: {}" ), szFile );
}

void Config::Refresh( )
{
	vecFileNames.clear( );

	for (const auto& it : std::filesystem::directory_iterator(fsConfigPath))
	{
		if (it.path( ).filename( ).extension( ) == X( ".chlen" ))
		{
			Logging::Print( X( "found configuration file: {}" ), it.path( ).filename( ).string( ) );
			vecFileNames.push_back( it.path( ).filename( ).string( ) );
		}
	}
}

std::size_t Config::GetVariableIndex( const FNV1A_t uNameHash )
{
	for (std::size_t i = 0U; i < vecVariables.size( ); i++)
	{
		if (vecVariables.at( i ).uNameHash == uNameHash)
			return i;
	}

	return C_INVALID_VARIABLE;
}

std::filesystem::path Config::GetWorkingPath( )
{
	std::filesystem::path fsWorkingPath;

	if (PWSTR pszPathToDocuments; SUCCEEDED(LI_FN(SHGetKnownFolderPath)( FOLDERID_Documents, 0UL, nullptr, &pszPathToDocuments ) ))
	{
		fsWorkingPath.assign( pszPathToDocuments );
		fsWorkingPath.append( X( "Chlenixx" ) );
		CoTaskMemFree(pszPathToDocuments );
	}

	return fsWorkingPath;
}

bool Config::ExportToClipboard(const std::string& file)
{
	std::filesystem::path path = std::filesystem::path(fsConfigPath) /= file;

	if (!std::filesystem::exists(path))
		return false;

	std::ifstream fileIn(path);

	if (!fileIn.good())
		return false;

	std::string strBuffer = std::string(std::istreambuf_iterator<char>(fileIn), std::istreambuf_iterator<char>());

	fileIn.close();

	if (!LI_FN(OpenClipboard)(nullptr))
		return false;

	LI_FN(EmptyClipboard)();

	const HGLOBAL hg = LI_FN(GlobalAlloc)(GMEM_MOVEABLE, strBuffer.size() + 1);

	if (!hg) {
		LI_FN(CloseClipboard)();
		return false;
	}

	CRT::MemoryCopy(GlobalLock(hg), strBuffer.data(), strBuffer.size() + 1);

	LI_FN(GlobalUnlock)(hg);

	LI_FN(SetClipboardData)(CF_TEXT, hg);

	LI_FN(CloseClipboard)();

	return true;
}

bool Config::ImportFromClipboard(const std::string& file)
{
	std::filesystem::path path = std::filesystem::path(fsConfigPath) /= file;

	if (!LI_FN(OpenClipboard)(nullptr))
		return false;

	const HANDLE hClipboardData = LI_FN(GetClipboardData)(CF_TEXT);

	std::string_view strClipBoardText = static_cast<char*>(LI_FN(GlobalLock)(hClipboardData));

	LI_FN(GlobalUnlock)(hClipboardData);

	std::ofstream fileOut(path, std::ios::trunc);
	fileOut << strClipBoardText;
	fileOut.close();

	LI_FN(CloseClipboard)();

	if (!Load(file, true))
		return false;

	return true;
}

