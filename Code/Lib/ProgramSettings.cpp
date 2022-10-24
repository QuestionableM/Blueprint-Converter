#include "ProgramSettings.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ModData.hpp"

#include "Lib\String.hpp"
#include "Lib\File.hpp"
#include "DebugCon.hpp"

#include <msclr\marshal_cppstd.h>
#include <filesystem>
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

			std::wstring wstr_path = String::ToWide(pValue.get_ref<const std::string&>());
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
				std::wstring wstr_key_repl = String::ToWide(keyword.value().get_ref<const std::string&>());
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
		std::wstring v_steamPath;
		if (!String::ReadRegistryKey(L"SOFTWARE\\Valve\\Steam", L"SteamPath", v_steamPath))
		{
			if (!String::ReadRegistryKey(L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"SteamPath", v_steamPath))
			{
				DebugErrorL("Couldn't find the installation path for Steam!");
				return false;
			}
		}

		if (!File::Exists(v_steamPath))
			return false;

		const std::wstring v_guessedSmPath = v_steamPath + L"/steamapps/common/scrap mechanic";
		const std::wstring v_guessedWsPath = v_steamPath + L"/steamapps/workshop/content/387990";

		//If SM and WS paths are not valid, then the program will read the libraryfolders.vdf file
		if (File::Exists(v_guessedSmPath) && File::Exists(v_guessedWsPath))
		{
			game_path = v_guessedSmPath;
			workshop_path = v_guessedWsPath;

			DebugOutL("Guessed a game path from the registry: ", 0b1101_fg, v_guessedSmPath);
			DebugOutL("Guessed a workshop path from the registry: ", 0b1101_fg, v_guessedWsPath);

			return true;
		}
		else
		{
			const std::wstring vdf_path = v_steamPath + L"/steamapps/libraryfolders.vdf";
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
		std::wstring v_smLocalData;
		if (!File::GetAppDataPath(v_smLocalData)) return;

		v_smLocalData.append(L"\\Axolot Games\\Scrap Mechanic\\User");
		if (!File::Exists(v_smLocalData)) return;

		std::error_code v_errorCode;
		fs::directory_iterator v_dirIter(v_smLocalData, fs::directory_options::skip_permission_denied, v_errorCode);

		for (const auto& v_curDir : v_dirIter)
		{
			if (v_errorCode)
			{
				DebugErrorL("Couldn't read an item in the directory: ", v_smLocalData);
				continue;
			}

			if (!v_curDir.is_directory()) continue;

			const std::wstring BPPath = v_curDir.path().wstring() + L"\\Blueprints";
			const std::wstring ModPath = v_curDir.path().wstring() + L"\\Mods";

			if (File::Exists(BPPath))
				Settings::AddToStrVec(Settings::BlueprintFolders, BPPath);

			if (File::Exists(ModPath))
				Settings::AddToStrVec(Settings::LocalModFolders, ModPath);
		}
	}

	void Settings::FindGamePath(const nlohmann::json& config_json, bool& should_write)
	{
		if (Settings::PathToSM.empty() || !File::Exists(Settings::PathToSM))
		{
			std::wstring v_gamePath, v_wsPath;

			if (Settings::GetSteamPaths(v_gamePath, v_wsPath))
			{
				Settings::PathToSM = v_gamePath;

				Settings::AddToStrVec(Settings::BlueprintFolders, v_wsPath);
				Settings::AddToStrVec(Settings::ModFolders, v_wsPath);
				
				should_write = true;
			}

			Settings::FindLocalUsers();
		}
	}

	void Settings::ReadUserSettings(const nlohmann::json& config_json, bool& should_write)
	{
		const auto& v_userSettings = Json::Get(config_json, "UserSettings");
		if (v_userSettings.is_object())
		{
			const auto& v_gamePath = Json::Get(v_userSettings, "GamePath");
			if (v_gamePath.is_string())
			{
				Settings::PathToSM = String::ToWide(v_gamePath.get_ref<const std::string&>());

				DebugOutL("Game Path: ", Settings::PathToSM);
			}

			Settings::JsonStrArrayToVector(v_userSettings, "BlueprintPaths"    , Settings::BlueprintFolders);
			Settings::JsonStrArrayToVector(v_userSettings, "LocalModFolders"   , Settings::LocalModFolders);
			Settings::JsonStrArrayToVector(v_userSettings, "WorkshopModFolders", Settings::ModFolders);

			const auto& v_openInSteam = Json::Get(v_userSettings, "OpenLinksInSteam");
			Settings::OpenLinksInSteam = (v_openInSteam.is_boolean() ? v_openInSteam.get<bool>() : false);
		}

		Settings::FindGamePath(config_json, should_write);
	}

	nlohmann::json Settings::GetConfigJson(bool* should_write, const bool& read_from_file)
	{ 
		nlohmann::json cfgData;
		if (read_from_file)
		{
			nlohmann::json cfgData = Json::LoadParseJson(Settings::ConfigPath.data());
			if (!cfgData.is_object())
				cfgData = nlohmann::json::object();
		}
		else
		{
			cfgData = nlohmann::json::object();
		}

		nlohmann::json v_programSettings = Json::Get(cfgData, "ProgramSettings");
		if (v_programSettings.is_object())
		{
			const nlohmann::json::const_iterator v_iter = v_programSettings.find("LanguageDirectories");
			if (v_iter != v_programSettings.end())
			{
				DebugWarningL("Found a deprecated json key: \"LanguageDirectories\"");
				v_programSettings.erase(v_iter);

				if (should_write)
					(*should_write) = true;
			}
		}
		else
		{
			v_programSettings = nlohmann::json::object();
		}

		if (!v_programSettings.contains("Keywords"))
		{
			v_programSettings["Keywords"] =
			{
				{ "$CHALLENGE_DATA", "$GAME_FOLDER/ChallengeData" },
				{ "$GAME_DATA"     , "$GAME_FOLDER/Data"          },
				{ "$SURVIVAL_DATA" , "$GAME_FOLDER/Survival"      }
			};

			if (should_write)
				(*should_write) = true;
		}

		if (!v_programSettings.contains("LanguageFiles"))
		{
			v_programSettings["LanguageFiles"] =
			{
				"$GAME_DATA/Gui/Language/English/InventoryItemDescriptions.json",
				"$SURVIVAL_DATA/Gui/Language/English/inventoryDescriptions.json",
				"$CHALLENGE_DATA/Gui/Language/English/inventoryDescriptions.json"
			};

			if (should_write)
				(*should_write) = true;
		}

		if (!v_programSettings.contains("ResourceUpgradeFiles"))
		{
			v_programSettings["ResourceUpgradeFiles"] =
			{
				"$GAME_DATA/upgrade_resources.json"
			};

			if (should_write)
				(*should_write) = true;
		}

		if (!v_programSettings.contains("ScrapObjectDatabase"))
		{
			v_programSettings["ScrapObjectDatabase"] =
			{
				"$CHALLENGE_DATA/Objects/Database/ShapeSets",
				"$SURVIVAL_DATA/Objects/Database/ShapeSets",
				"$GAME_DATA/Objects/Database/ShapeSets"
			};

			if (should_write)
				(*should_write) = true;
		}

		cfgData["ProgramSettings"] = v_programSettings;

		return cfgData;
	}

	void Settings::ClearData()
	{
		Settings::BlueprintFolders.clear();
		Settings::ModFolders.clear();
		Settings::LocalModFolders.clear();
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
		nlohmann::json cfgData = Settings::GetConfigJson(nullptr, false);

		{
			nlohmann::json user_settings = nlohmann::json::object();

			user_settings["GamePath"] = String::ToUtf8(Settings::PathToSM);
			user_settings["OpenLinksInSteam"] = Settings::OpenLinksInSteam;

			Settings::WstrVecToJsonArray(user_settings, Settings::BlueprintFolders, "BlueprintPaths");
			Settings::WstrVecToJsonArray(user_settings, Settings::LocalModFolders, "LocalModFolders");
			Settings::WstrVecToJsonArray(user_settings, Settings::ModFolders, "WorkshopModFolders");

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
		nlohmann::json cfgData = Settings::GetConfigJson(&should_write, true);

		Settings::ReadUserSettings(cfgData, should_write);

		//Stop reading the config if the path to the game is invalid
		if (Settings::PathToSM.empty())
			return false;

		Settings::UpdatePathReplacement();
		Settings::ReadProgramSettings(cfgData);

		return should_write;
	}
}