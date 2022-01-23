#include "ProgramSettings.h"

#include "Lib/String/String.h"
#include "Lib/File/FileFunc.h"

#include "Object Database/Keyword Replacer/KeywordReplacer.h"
#include "Object Database/Mod Data/ModData.h"

#include "DebugCon.h"

#include <filesystem>
#include <msclr/marshal_cppstd.h>
#include <ShlObj.h>

namespace fs = std::filesystem;

namespace SMBC
{
	std::vector<std::wstring> Settings::BlueprintFolders = {};
	std::vector<std::wstring> Settings::ModFolders = {};
	std::vector<std::wstring> Settings::SMDirDatabase = {};
	std::vector<std::wstring> Settings::VanillaLanguagePaths = {};

	std::wstring Settings::PathToSM = L"";
	bool Settings::OpenLinksInSteam = false;

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

		mWstrVec.push_back(mWstr);
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

				PathReplacer::Add(wstr_key, PathReplacer::ReplaceKey(wstr_key_repl));
			}
		}

		{
			std::vector<std::wstring> pUpgradeArray = {};
			Settings::JsonStrArrayToVector(pSettings, "ResourceUpgradeFiles", pUpgradeArray);

			for (const std::wstring& pUpgradePath : pUpgradeArray)
				PathReplacer::ReadResourceUpgrades(pUpgradePath);
		}

		Settings::JsonStrArrayToVector(pSettings, "LanguageDirectories", Settings::VanillaLanguagePaths);
		Settings::JsonStrArrayToVector(pSettings, "ScrapObjectDatabase", Settings::SMDirDatabase);
	}

	bool Settings::GetStaemPaths(std::wstring& game_path, std::wstring& workshop_path)
	{
		std::wstring steam_path = String::ReadRegistryKey(L"SOFTWARE\\Valve\\Steam", L"SteamPath");
		if (steam_path.empty() || !File::Exists(steam_path))
			steam_path = String::ReadRegistryKey(L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"SteamPath");

		if (steam_path.empty() || !File::Exists(steam_path)) return false;

		const std::wstring sm_path = steam_path + L"/steamapps/common/scrap mechanic";
		const std::wstring ws_path = steam_path + L"/steamapps/workshop/content/387990";

		if (File::Exists(sm_path))
			game_path = sm_path;

		if (File::Exists(ws_path))
			workshop_path = ws_path;

		return true;
	}

	void Settings::FindLocalUsers()
	{
		System::String^ app_data_path = System::Environment::GetFolderPath(System::Environment::SpecialFolder::ApplicationData);

		std::wstring SMLocalData = msclr::interop::marshal_as<std::wstring>(app_data_path);
		SMLocalData.append(L"\\Axolot Games\\Scrap Mechanic\\User");

		if (!File::Exists(SMLocalData)) return;

		fs::directory_iterator DirIter(SMLocalData, fs::directory_options::skip_permission_denied);
		for (auto& dir : DirIter)
		{
			if (!dir.is_directory()) continue;

			std::wstring BPPath = dir.path().wstring() + L"\\Blueprints";
			std::wstring ModPath = dir.path().wstring() + L"\\Mods";

			if (File::Exists(BPPath))
				Settings::AddToStrVec(Settings::BlueprintFolders, BPPath);

			if (File::Exists(ModPath))
				Settings::AddToStrVec(Settings::ModFolders, ModPath);
		}
	}

	void Settings::FindGamePath(nlohmann::json& config_json, bool& should_write)
	{
		if (Settings::PathToSM.empty() || !File::Exists(Settings::PathToSM))
		{
			std::wstring game_path, ws_path;

			if (Settings::GetStaemPaths(game_path, ws_path))
			{
				{
					nlohmann::json user_settings = Json::Get(config_json, "UserSettings");

					//user_settings["ScrapPath"] = String::ToUtf8(game_path);
					//Settings::PathToSM = game_path;

					DebugOutL("Found a game path from the registry: ", Settings::PathToSM);

					config_json["UserSettings"] = user_settings;
				}

				{
					Settings::FindLocalUsers();

					Settings::AddToStrVec(Settings::BlueprintFolders, ws_path);
					Settings::AddToStrVec(Settings::ModFolders, ws_path);
				}
				
				should_write = true;
			}
			else
			{
				return;
			}
		}

		PathReplacer::Add(L"$GAME_FOLDER", Settings::PathToSM);
	}

	void Settings::FillUserSettings(nlohmann::json& config_json, bool& should_write)
	{
		nlohmann::json user_settings = Json::Get(config_json, "UserSettings");

		const bool no_mod_folders = (user_settings.find("ScrapModsPath") == user_settings.end());
		const bool no_bp_folders = (user_settings.find("BlueprintPaths") == user_settings.end());

		if (no_mod_folders || no_bp_folders)
		{
			if (no_mod_folders) Settings::WstrVecToJsonArray(user_settings, Settings::ModFolders,       "ScrapModsPath" );
			if (no_bp_folders)  Settings::WstrVecToJsonArray(user_settings, Settings::BlueprintFolders, "BlueprintPaths");

			config_json["UserSettings"] = user_settings;
			should_write = true;
		}
	}

	void Settings::ReadUserSettings(nlohmann::json& config_json, bool& should_write)
	{
		const auto& user_settings = Json::Get(config_json, "UserSettings");
		if (user_settings.is_object())
		{
			const auto& game_path = Json::Get(user_settings, "ScrapPath");
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

		Settings::FindGamePath    (config_json, should_write);
		Settings::FillUserSettings(config_json, should_write);
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
			nlohmann::json pSet = nlohmann::json::object();

			nlohmann::json pKeywords = nlohmann::json::object();
			pKeywords["$CHALLENGE_DATA"] = "$GAME_FOLDER/ChallengeData";
			pKeywords["$GAME_DATA"] = "$GAME_FOLDER/Data";
			pKeywords["$SURVIVAL_DATA"] = "$GAME_FOLDER/Survival";

			nlohmann::json pLangDirs = nlohmann::json::array();
			pLangDirs.push_back("$GAME_DATA/Gui/Language/English");
			pLangDirs.push_back("$SURVIVAL_DATA/Gui/Language/English");
			pLangDirs.push_back("$CHALLENGE_DATA/Gui/Language/English");

			nlohmann::json pResourceUpgrades = nlohmann::json::array();
			pResourceUpgrades.push_back("$GAME_DATA/upgrade_resources.json");

			nlohmann::json pObjectDatabase = nlohmann::json::array();
			pObjectDatabase.push_back("$CHALLENGE_DATA/Objects/Database/ShapeSets");
			pObjectDatabase.push_back("$SURVIVAL_DATA/Objects/Database/ShapeSets");
			pObjectDatabase.push_back("$GAME_DATA/Objects/Database/ShapeSets");

			pSet["Keywords"] = pKeywords;
			pSet["LanguageDirectories"] = pLangDirs;
			pSet["ResourceUpgradeFiles"] = pResourceUpgrades;
			pSet["ScrapObjectDatabase"] = pObjectDatabase;

			cfgData["ProgramSettings"] = pSet;

			if (should_write != nullptr)
				*should_write = true;
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

		PathReplacer::ClearData();
		Mod::ClearMods();
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

		DebugOutL("Saving a new config...");
		Json::WriteToFile(Settings::ConfigPath.data(), cfgData);
	}

	void Settings::ReadConfig()
	{
		Settings::ClearData();

		bool should_write = false;
		nlohmann::json cfgData = Settings::GetConfigJson(&should_write);

		Settings::ReadUserSettings(cfgData, should_write);
		Settings::ReadProgramSettings(cfgData);

		if (should_write)
		{
			DebugOutL("Writing a new Config.json...");
			Json::WriteToFile(Settings::ConfigPath.data(), cfgData);
		}
	}
}