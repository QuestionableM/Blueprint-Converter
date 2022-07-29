#include "ProgramSettings.h"

#include "Lib\File.h"
#include "Lib\String.h"

#include "ObjectDatabase\KeywordReplacer.h"
#include "ObjectDatabase\ModData.h"

#include "DebugCon.h"

#include <filesystem>
#include <msclr/marshal_cppstd.h>
#include <ShlObj.h>

#pragma warning(push)
#pragma warning(disable : 4996)
	#include <valve_vdf\vdf_parser.hpp>
#pragma warning(pop)

namespace fs = std::filesystem;

namespace SMBC
{
	void Settings::JsonStrArrayToVector(const nlohmann::json& pJson, const std::string& pKey, std::vector<std::wstring>& pWstrVec)
	{
		const auto& pArray = Json::Get(pJson, pKey);
		if (!pArray.is_array()) return;

		for (const auto& pValue : pArray)
		{
			if (!pValue.is_string()) continue;

			std::wstring wstr_path = String::ToWide(pValue.get<std::string>());
			wstr_path = PathReplacer::ReplaceKey(wstr_path);

			pWstrVec.push_back(wstr_path);
		}
	}

	void Settings::WstrVecToJsonArray(nlohmann::json& out_json, std::vector<std::wstring>& mWstrVec, const std::string& pKey)
	{
		nlohmann::json str_array = nlohmann::json::array();

		for (const std::wstring& mWstrPath : mWstrVec)
			str_array.push_back(String::ToUtf8(mWstrPath));

		out_json[pKey] = str_array;
	}

	void Settings::AddToStrVec(std::vector<std::wstring>& mWstrVec, const std::wstring& mWstr)
	{
		for (const std::wstring& wstr_data : mWstrVec)
		{
			if (wstr_data == mWstr || File::IsEquivalent(wstr_data, mWstr))
				return;
		}

		{
			std::wstring mFinalString = mWstr;

			String::ToLowerR(mFinalString);
			String::ReplaceR(mFinalString, L'\\', L'/');

			mWstrVec.push_back(mFinalString);
		}
	}



	void Settings::ReadProgramSettings(const nlohmann::json& config_json)
	{
		const auto& pSettings = Json::Get(config_json, "ProgramSettings");
		if (!pSettings.is_object()) return;

		const auto& pKeywords = Json::Get(pSettings, "Keywords");
		if (pKeywords.is_object())
		{
			for (const auto& keyword : pKeywords.items())
			{
				if (!keyword.value().is_string()) continue;

				std::wstring wstr_key = String::ToWide(keyword.key());
				std::wstring wstr_key_repl = String::ToWide(keyword.value().get<std::string>());
				PathReplacer::ReplaceKeyR(wstr_key_repl);

				PathReplacer::SetReplacement(wstr_key, wstr_key_repl);
			}
		}

		{
			std::vector<std::wstring> pUpgradeArray = {};
			Settings::JsonStrArrayToVector(pSettings, "ResourceUpgradeFiles", pUpgradeArray);

			for (const std::wstring& pUpgradePath : pUpgradeArray)
				PathReplacer::LoadResourceUpgrades(pUpgradePath);
		}

		Settings::JsonStrArrayToVector(pSettings, "LanguageFiles", Settings::VanillaLanguagePaths);
		Settings::JsonStrArrayToVector(pSettings, "ScrapObjectDatabase", Settings::SMDirDatabase);
	}

	using vdf_childs_table = std::unordered_map<std::string, std::shared_ptr<tyti::vdf::object>>;
	using vdf_attrib_table = std::unordered_map<std::string, std::string>;
	bool Settings::GetSteamPaths(std::wstring& game_path, std::wstring& workshop_path)
	{
		std::wstring steam_path = String::ReadRegistryKey(L"SOFTWARE\\Valve\\Steam", L"SteamPath");
		if (steam_path.empty() || !File::Exists(steam_path))
			steam_path = String::ReadRegistryKey(L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"SteamPath");

		if (steam_path.empty() || !File::Exists(steam_path))
			return false;

		const std::wstring registry_sm_path = steam_path + L"/steamapps/common/scrap mechanic";
		const std::wstring registry_ws_path = steam_path + L"/steamapps/workshop/content/387990";

		//If SM and WS paths are not valid, then the program will read the libraryfolders.vdf file
		if (File::Exists(registry_sm_path) && File::Exists(registry_ws_path))
		{
			game_path = registry_sm_path;
			workshop_path = registry_ws_path;

			DebugOutL("Found a game path from the registry: ", 0b1101_fg, registry_sm_path);
			DebugOutL("Found a workshop path from the registry: ", 0b1101_fg, registry_ws_path);

			return true;
		}
		else
		{
			const std::wstring vdf_path = steam_path + L"/steamapps/libraryfolders.vdf";
			if (!File::Exists(vdf_path))
				return false;

			DebugOutL("VDF path found: ", vdf_path);

			std::ifstream vdf_reader(vdf_path);
			if (!vdf_reader.is_open())
				return false;

			tyti::vdf::basic_object<char> vdf_root = tyti::vdf::read(vdf_reader);
			if (vdf_root.name != "libraryfolders")
				return false;

			vdf_reader.close();

			const vdf_childs_table& vdf_lib_folders = vdf_root.childs;
			for (const auto& lib_folder : vdf_lib_folders)
			{
				const vdf_attrib_table& lib_folder_attribs = lib_folder.second->attribs;
				const vdf_attrib_table::const_iterator attrib_iter = lib_folder_attribs.find("path");
				if (attrib_iter == lib_folder_attribs.end())
					continue;

				const vdf_childs_table& lib_folder_childs = lib_folder.second->childs;
				const vdf_childs_table::const_iterator childs_iter = lib_folder_childs.find("apps");
				if (childs_iter == lib_folder_childs.end())
					continue;

				const vdf_attrib_table& apps_attribs = childs_iter->second->attribs;
				const vdf_attrib_table::const_iterator apps_attribs_iter = apps_attribs.find("387990");
				if (apps_attribs_iter == apps_attribs.end())
					continue;

				const std::wstring library_path_wstr = String::ToWide(attrib_iter->second);
				const std::wstring library_sm_path = library_path_wstr + L"/steamapps/common/scrap mechanic";
				const std::wstring library_ws_path = library_path_wstr + L"/steamapps/workshop/content/387990";

				const bool library_sm_exists = File::Exists(library_sm_path);
				const bool library_ws_exists = File::Exists(library_ws_path);

				if (library_sm_exists)
				{
					DebugOutL("Found a game path from the library: ", 0b1101_fg, library_sm_path);
					game_path = library_sm_path;
				}

				if (library_ws_exists)
				{
					DebugOutL("Found a workshop path from the library: ", 0b1101_fg, library_ws_path);
					workshop_path = library_ws_path;
				}

				if (library_sm_exists || library_ws_exists)
				{
					DebugOutL("Successfully got the steam paths!");
					return true;
				}
			}
		}

		return false;
	}

	void Settings::FindLocalUsers()
	{
		std::wstring SMLocalData;
		if (!File::GetAppDataPath(SMLocalData)) return;

		SMLocalData.append(L"\\Axolot Games\\Scrap Mechanic\\User");
		if (!File::Exists(SMLocalData)) return;

		std::error_code mError;
		fs::directory_iterator DirIter(SMLocalData, fs::directory_options::skip_permission_denied, mError);

		for (const auto& dir : DirIter)
		{
			if (mError)
			{
				DebugErrorL("Couldn't read an item in the directory: ", SMLocalData);
				continue;
			}

			if (!dir.is_directory()) continue;

			std::wstring BPPath = dir.path().wstring() + L"\\Blueprints";
			std::wstring ModPath = dir.path().wstring() + L"\\Mods";

			if (File::Exists(BPPath))
				Settings::AddToStrVec(Settings::BlueprintFolders, BPPath);

			if (File::Exists(ModPath))
				Settings::AddToStrVec(Settings::ModFolders, ModPath);
		}
	}

	void Settings::FindGamePath(const nlohmann::json& config_json, bool& should_write)
	{
		if (Settings::PathToSM.empty() || !File::Exists(Settings::PathToSM))
		{
			std::wstring game_path, ws_path;

			if (Settings::GetSteamPaths(game_path, ws_path))
			{
				Settings::PathToSM = game_path;

				Settings::AddToStrVec(Settings::BlueprintFolders, ws_path);
				Settings::AddToStrVec(Settings::ModFolders, ws_path);
				
				should_write = true;
			}

			Settings::FindLocalUsers();
		}
	}

	void Settings::ReadUserSettings(const nlohmann::json& config_json, bool& should_write)
	{
		const auto& user_settings = Json::Get(config_json, "UserSettings");
		if (user_settings.is_object())
		{
			const auto& game_path = Json::Get(user_settings, "GamePath");
			if (game_path.is_string())
			{
				const std::string sm_path = game_path.get<std::string>();
				Settings::PathToSM = String::ToWide(sm_path);

				DebugOutL("Game Path: ", Settings::PathToSM);
			}

			Settings::JsonStrArrayToVector(user_settings, "BlueprintPaths", Settings::BlueprintFolders);
			Settings::JsonStrArrayToVector(user_settings, "ScrapModsPath", Settings::ModFolders);

			const auto& open_in_steam = Json::Get(user_settings, "OpenLinksInSteam");
			Settings::OpenLinksInSteam = (open_in_steam.is_boolean() ? open_in_steam.get<bool>() : false);
		}

		Settings::FindGamePath(config_json, should_write);
	}

	nlohmann::json Settings::GetConfigJson(bool* should_write)
	{
		nlohmann::json cfgData = Json::LoadParseJson(Settings::ConfigPath.data());
		if (!cfgData.is_object())
		{
			cfgData = nlohmann::json::object();
		}

		if (!cfgData.contains("ProgramSettings"))
		{
			cfgData["ProgramSettings"] =
			{
				{
					"Keywords",
					{
						{ "$CHALLENGE_DATA", "$GAME_FOLDER/ChallengeData" },
						{ "$GAME_DATA"     , "$GAME_FOLDER/Data"          },
						{ "$SURVIVAL_DATA" , "$GAME_FOLDER/Survival"      }
					}
				},
				{
					"LanguageFiles",
					{
						"$GAME_DATA/Gui/Language/English/InventoryItemDescriptions.json",
						"$SURVIVAL_DATA/Gui/Language/English/inventoryDescriptions.json",
						"$CHALLENGE_DATA/Gui/Language/English/inventoryDescriptions.json"
					}
				},
				{
					"ResourceUpgradeFiles",
					{
						"$GAME_DATA/upgrade_resources.json"
					}
				},
				{
					"ScrapObjectDatabase",
					{
						"$CHALLENGE_DATA/Objects/Database/ShapeSets",
						"$SURVIVAL_DATA/Objects/Database/ShapeSets",
						"$GAME_DATA/Objects/Database/ShapeSets"
					}
				}
			};

			if (should_write != nullptr)
				*should_write = true;
		}
		else
		{
			auto& program_settings = cfgData["ProgramSettings"];
			if (program_settings.contains("LanguageDirectories"))
			{
				DebugWarningL("Found a deprecated json key: \"LanguageDirectories\"");
				program_settings.erase("LanguageDirectories");

				program_settings["LanguageFiles"] =
				{
					"$GAME_DATA/Gui/Language/English/InventoryItemDescriptions.json",
					"$SURVIVAL_DATA/Gui/Language/English/inventoryDescriptions.json",
					"$CHALLENGE_DATA/Gui/Language/English/inventoryDescriptions.json"
				};

				if (should_write != nullptr)
					*should_write = true;
			}
		}

		return cfgData;
	}

	void Settings::ClearData()
	{
		Settings::BlueprintFolders.clear();
		Settings::ModFolders.clear();
		Settings::SMDirDatabase.clear();
		Settings::VanillaLanguagePaths.clear();

		Settings::PathToSM.clear();
		Settings::OpenLinksInSteam = false;

		PathReplacer::Clear();
		Mod::ClearMods();
	}

	void Settings::UpdatePathReplacement()
	{
		DebugOutL("Set $GAME_FOLDER to ", Settings::PathToSM);
		PathReplacer::SetReplacement(L"$GAME_FOLDER", Settings::PathToSM);
	}

	void Settings::SaveConfig()
	{
		nlohmann::json cfgData = Settings::GetConfigJson();

		{
			nlohmann::json user_settings = nlohmann::json::object();

			user_settings["GamePath"] = String::ToUtf8(Settings::PathToSM);
			user_settings["OpenLinksInSteam"] = Settings::OpenLinksInSteam;
			Settings::WstrVecToJsonArray(user_settings, Settings::ModFolders, "ScrapModsPath");
			Settings::WstrVecToJsonArray(user_settings, Settings::BlueprintFolders, "BlueprintPaths");

			cfgData["UserSettings"] = user_settings;
		}

		DebugOutL(0b0110_fg, "Saving a new config...");
		Json::WriteToFile(Settings::ConfigPath.data(), cfgData);

		//Update the game path keyword in case the path was updated
		Settings::UpdatePathReplacement();
	}

	bool Settings::ReadConfig()
	{
		DebugOutL(0b0110_fg, "Reading the program config...");
		Settings::ClearData();

		bool should_write = false;
		nlohmann::json cfgData = Settings::GetConfigJson(&should_write);

		Settings::ReadUserSettings(cfgData, should_write);

		//Stop reading the config if the path to the game is invalid
		if (Settings::PathToSM.empty()) return false;

		Settings::UpdatePathReplacement();
		Settings::ReadProgramSettings(cfgData);

		return should_write;
	}
}