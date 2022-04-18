#include "ProgramSettings.h"

#include "Lib\File.h"
#include "Lib\String.h"

#include "ObjectDatabase\KeywordReplacer.h"
#include "ObjectDatabase\ModData.h"

#include "DebugCon.h"

#include <filesystem>
#include <msclr/marshal_cppstd.h>
#include <ShlObj.h>

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

			if (Settings::GetStaemPaths(game_path, ws_path))
			{
				Settings::PathToSM = game_path;
				DebugOutL("Found a game path from the registry: ", ConCol::YELLOW_INT, Settings::PathToSM);

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
					"LanguageDirectories",
					{
						"$GAME_DATA/Gui/Language/English",
						"$SURVIVAL_DATA/Gui/Language/English",
						"$CHALLENGE_DATA/Gui/Language/English"
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

		DebugOutL(ConCol::CYAN, "Saving a new config...");
		Json::WriteToFile(Settings::ConfigPath.data(), cfgData);

		//Update the game path keyword in case the path was updated
		Settings::UpdatePathReplacement();
	}

	bool Settings::ReadConfig()
	{
		DebugOutL(ConCol::CYAN, "Reading the program config...");

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