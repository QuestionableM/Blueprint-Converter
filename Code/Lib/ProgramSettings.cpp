#include "ProgramSettings.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/String/String.h"
#include "Lib/File/FileFunc.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"
#include "Object Database/Mod Data/ModData.h"

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

	void Settings::LoadJsonWstrArray(
		const nlohmann::json& file,
		const std::string& keyword,
		std::vector<std::wstring>& _array
	) {
		const auto& jArray = Json::Get(file, keyword);
		if (!jArray.is_array()) return;

		for (auto& _wstr : jArray) {
			if (!_wstr.is_string()) continue;

			std::wstring _WstrPath = String::ToWide(_wstr.get<std::string>());
			_WstrPath = PathReplacer::ReplaceKey(_WstrPath);

			_array.push_back(_WstrPath);
		}
	}

	void Settings::AddArrayPath(
		const std::wstring& element,
		std::vector<std::wstring>& _array
	) {
		for (std::wstring& _wstr : _array) {
			if (_wstr == element || File::IsEquivalent(_wstr, element))
				return;
		}

		_array.push_back(element);
	}

	void Settings::ClearSettings()
	{
		Settings::BlueprintFolders.clear();
		Settings::ModFolders.clear();
		Settings::PathToSM.clear();
		Settings::VanillaLanguagePaths.clear();
		Settings::OpenLinksInSteam = false;
		Settings::SMDirDatabase.clear();

		PathReplacer::ClearData();
		Mod::ClearMods();
	}

	void Settings::LoadUserSettings(const nlohmann::json& jSettings)
	{
		const auto& jUserSettings = Json::Get(jSettings, "UserSettings");
		if (!jUserSettings.is_object()) return;

		const auto& _SMPath		 = Json::Get(jUserSettings, "ScrapPath");
		const auto& _OpenInSteam = Json::Get(jUserSettings, "OpenLinksInSteam");

		Settings::LoadJsonWstrArray(jUserSettings, "BlueprintPaths", Settings::BlueprintFolders);
		Settings::LoadJsonWstrArray(jUserSettings, "ScrapModsPath",  Settings::ModFolders);

		if (_SMPath.is_string())
			Settings::PathToSM = String::ToWide(_SMPath.get<std::string>());

		if (_OpenInSteam.is_boolean())
			Settings::OpenLinksInSteam = _OpenInSteam.get<bool>();
	}

	void Settings::LoadResourceUpgrades(const nlohmann::json& jProgramSettings)
	{
		std::vector<std::wstring> _UpgradeFiles = {};
		Settings::LoadJsonWstrArray(jProgramSettings, "ResourceUpgradeFiles", _UpgradeFiles);

		for (std::wstring& fPath : _UpgradeFiles)
			PathReplacer::ReadResourceUpgrades(fPath);
	}

	void Settings::LoadKeywords(const nlohmann::json& jProgramSettings)
	{
		const auto& _KeyWords = Json::Get(jProgramSettings, "Keywords");
		if (!_KeyWords.is_object()) return;

		for (auto& keyword : _KeyWords.items())
		{
			if (!keyword.value().is_string()) continue;

			std::wstring _WstrKey = String::ToWide(keyword.key());
			std::wstring _WstrRepl = String::ToWide(keyword.value().get<std::string>());
			_WstrRepl = PathReplacer::ReplaceKey(_WstrRepl);

			PathReplacer::Add(_WstrKey, _WstrRepl);
		}
	}

	void Settings::LoadProgramSettings(const nlohmann::json& jSettings)
	{
		const auto& jProgramSettings = Json::Get(jSettings, "ProgramSettings");
		if (!jProgramSettings.is_object()) return;

		Settings::LoadKeywords(jProgramSettings);
		Settings::LoadResourceUpgrades(jProgramSettings);

		Settings::LoadJsonWstrArray(jProgramSettings, "ScrapObjectDatabase", Settings::SMDirDatabase);
		Settings::LoadJsonWstrArray(jProgramSettings, "LanguageDirectories", Settings::VanillaLanguagePaths);
	}

	void Settings::InitMainKeyword()
	{
		Settings::AddRegistryPathAndSave();
		if (!Settings::PathToSM.empty())
			PathReplacer::Add(L"$GAME_FOLDER", Settings::PathToSM);
	}

	void Settings::LoadSettingsFile()
	{
		nlohmann::json jConfigFile = Json::LoadParseJson(Settings::ConfigPath.data());
		if (!jConfigFile.is_object())
			return;

		Settings::ClearSettings();
		Settings::LoadUserSettings(jConfigFile);
		Settings::InitMainKeyword();
		Settings::LoadProgramSettings(jConfigFile);
	}

	void Settings::SaveSettingsFile(
		const bool sm_path,
		const bool bp_list,
		const bool mod_list,
		const bool open_in_steam
	) {
		nlohmann::json _JsonOutput = Json::LoadParseJson(Settings::ConfigPath.data());

		File::SafeCreateDir(L"./Resources");
		std::ofstream _ConfigJson(Settings::ConfigPath.data());
		if (!_ConfigJson.is_open()) return;

		nlohmann::json jUserSettings = {};

		if (_JsonOutput.find("UserSettings") != _JsonOutput.end())
		{
			const auto& user_settings = Json::Get(_JsonOutput, "UserSettings");
			if (user_settings.is_object())
				jUserSettings = user_settings;
		}

		if (sm_path)
			jUserSettings["ScrapPath"] = String::ToUtf8(Settings::PathToSM);

		if (bp_list)
		{
			nlohmann::json _BlueprintList = {};

			for (std::wstring& _bp_obj : Settings::BlueprintFolders)
				_BlueprintList.push_back(String::ToUtf8(_bp_obj));

			jUserSettings["BlueprintPaths"] = _BlueprintList;
		}

		if (mod_list)
		{
			nlohmann::json _ModList = {};

			for (std::wstring& _mod_obj : Settings::ModFolders)
				_ModList.push_back(String::ToUtf8(_mod_obj));

			jUserSettings["ScrapModsPath"] = _ModList;
		}

		if (open_in_steam)
			jUserSettings["OpenLinksInSteam"] = Settings::OpenLinksInSteam;

		_JsonOutput["UserSettings"] = jUserSettings;

		_ConfigJson << std::setfill('\t') << std::setw(1) << _JsonOutput;
		_ConfigJson.close();
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
				Settings::AddArrayPath(BPPath, Settings::BlueprintFolders);

			if (File::Exists(ModPath))
				Settings::AddArrayPath(ModPath, Settings::ModFolders);
		}
	}

	void Settings::FindSteamAndSteamWorkshop()
	{
		std::wstring _Path = Other::ReadRegistryKey(L"SOFTWARE\\Valve\\Steam", L"SteamPath");
		if (_Path.empty() || !File::Exists(_Path))
			_Path = Other::ReadRegistryKey(L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"SteamPath");

		if (_Path.empty() || !File::Exists(_Path)) return;

		std::wstring _ScrapPath = _Path + L"/steamapps/common/scrap mechanic";
		std::wstring _ScrapWorkshop = _Path + L"/steamapps/workshop/content/387990";

		if (File::Exists(_ScrapPath)) Settings::PathToSM = _ScrapPath;
		if (File::Exists(_ScrapWorkshop))
		{
			Settings::AddArrayPath(_ScrapWorkshop, Settings::ModFolders);
			Settings::AddArrayPath(_ScrapWorkshop, Settings::BlueprintFolders);
		}
	}

	void Settings::AddRegistryPathAndSave()
	{
		if (!Settings::PathToSM.empty()) return;

		Settings::FindLocalUsers();
		Settings::FindSteamAndSteamWorkshop();

		Settings::SaveSettingsFile(true, true, true);
	}
}