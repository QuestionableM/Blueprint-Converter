#include "ProgramSettings.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/String/String.h"
#include "Lib/File/FileFunc.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"
#include "Object Database/ObjectDatabase.h"

#include <filesystem>
#include <msclr/marshal_cppstd.h>
#include <ShlObj.h>

namespace fs = std::filesystem;

std::wstring SMBC::Settings::ConfigPath = L"./Resources/Config.json";

void SMBC::Settings::LoadJsonWstrArray(
	const nlohmann::json& file,
	const std::string& keyword,
	std::vector<std::wstring>& _array
) {
	if (!file.contains(keyword)) return;

	auto& _JsonArray = file.at(keyword);
	if (!_JsonArray.is_array()) return;

	for (auto& _wstr : _JsonArray) {
		if (!_wstr.is_string()) continue;

		std::wstring _WstrPath = SMBC::String::ToWide(_wstr.get<std::string>());
		_WstrPath = SMBC::PathReplacer::ReplaceKey(_WstrPath);

		_array.push_back(_WstrPath);
	}
}

void SMBC::Settings::AddArrayPath(
	const std::wstring& element,
	std::vector<std::wstring>& _array
) {
	for (std::wstring& _wstr : _array) {
		if (_wstr == element || SMBC::File::IsEquivalent(_wstr, element))
			return;
	}

	_array.push_back(element);
}

std::vector<std::wstring> SMBC::Settings::BlueprintFolders = {};
std::vector<std::wstring> SMBC::Settings::ModFolders = {};
std::vector<std::wstring> SMBC::Settings::SMDirDatabase = {};
std::vector<std::wstring> SMBC::Settings::VanillaLanguagePaths = {};
std::wstring SMBC::Settings::PathToSM = L"";
bool SMBC::Settings::OpenLinksInSteam = false;

void SMBC::Settings::LoadSettingsFile() {
	nlohmann::json _ConfigFile = SMBC::Json::LoadParseJson(SMBC::Settings::ConfigPath);
	if (!_ConfigFile.is_object())
		return;

	SMBC::Settings::BlueprintFolders.clear();
	SMBC::Settings::ModFolders.clear();
	SMBC::Settings::PathToSM.clear();
	SMBC::Settings::VanillaLanguagePaths.clear();
	SMBC::Settings::OpenLinksInSteam = false;
	SMBC::PathReplacer::ClearAllData();
	SMBC::Settings::SMDirDatabase.clear();
	SMBC::Mod::ClearMods();

	const auto& _UserSettings = SMBC::Json::Get(_ConfigFile, "UserSettings");
	if (_UserSettings.is_object())
	{
		const auto& _SMPath = SMBC::Json::Get(_UserSettings, "ScrapPath");
		const auto& _OpenInSteam = SMBC::Json::Get(_UserSettings, "OpenLinksInSteam");

		SMBC::Settings::LoadJsonWstrArray(_UserSettings, "BlueprintPaths", SMBC::Settings::BlueprintFolders);
		SMBC::Settings::LoadJsonWstrArray(_UserSettings, "ScrapModsPath", SMBC::Settings::ModFolders);

		if (_SMPath.is_string())
			SMBC::Settings::PathToSM = SMBC::String::ToWide(_SMPath.get<std::string>());

		if (_OpenInSteam.is_boolean())
			SMBC::Settings::OpenLinksInSteam = _OpenInSteam.get<bool>();
	}

	SMBC::Settings::AddRegistryPathAndSave();
	if (!SMBC::Settings::PathToSM.empty())
		SMBC::PathReplacer::Add(L"$GAME_FOLDER", SMBC::Settings::PathToSM);

	const auto& _ProgramSettings = SMBC::Json::Get(_ConfigFile, "ProgramSettings");
	if (_ProgramSettings.is_object()) {
		const auto& _KeyWords = SMBC::Json::Get(_ProgramSettings, "Keywords");

		if (_KeyWords.is_object()) {
			for (auto& keyword : _KeyWords.items()) {
				if (!keyword.value().is_string()) continue;

				std::wstring _WstrKey = SMBC::String::ToWide(keyword.key());
				std::wstring _WstrRepl = SMBC::String::ToWide(keyword.value().get<std::string>());
				_WstrRepl = SMBC::PathReplacer::ReplaceKey(_WstrRepl);

				SMBC::PathReplacer::Add(_WstrKey, _WstrRepl);
			}
		}

		std::vector<std::wstring> _UpgradeFiles = {};
		SMBC::Settings::LoadJsonWstrArray(_ProgramSettings, "ResourceUpgradeFiles", _UpgradeFiles);

		for (std::wstring& _str : _UpgradeFiles)
			SMBC::PathReplacer::ReadResourceUpgrades(_str);
		
		SMBC::Settings::LoadJsonWstrArray(_ProgramSettings, "ScrapObjectDatabase", SMBC::Settings::SMDirDatabase);
		SMBC::Settings::LoadJsonWstrArray(_ProgramSettings, "LanguageDirectories", SMBC::Settings::VanillaLanguagePaths);
	}
}

void SMBC::Settings::SaveSettingsFile(
	const bool sm_path,
	const bool bp_list,
	const bool mod_list,
	const bool open_in_steam
) {
	nlohmann::json _JsonOutput = SMBC::Json::LoadParseJson(SMBC::Settings::ConfigPath);

	SMBC::File::SafeCreateDir(L"./Resources");
	std::ofstream _ConfigJson(SMBC::Settings::ConfigPath);

	if (!_ConfigJson.is_open()) return;

	if (_JsonOutput.contains("UserSettings") && !_JsonOutput.at("UserSettings").is_object())
		_JsonOutput["UserSettings"] = {};

	if (sm_path)
		_JsonOutput["UserSettings"]["ScrapPath"] = SMBC::String::ToUtf8(SMBC::Settings::PathToSM);

	if (bp_list) {
		nlohmann::json _BlueprintList = {};

		for (std::wstring& _bp_obj : SMBC::Settings::BlueprintFolders)
			_BlueprintList.push_back(SMBC::String::ToUtf8(_bp_obj));

		_JsonOutput["UserSettings"]["BlueprintPaths"] = _BlueprintList;
	}

	if (mod_list) {
		nlohmann::json _ModList = {};

		for (std::wstring& _mod_obj : SMBC::Settings::ModFolders)
			_ModList.push_back(SMBC::String::ToUtf8(_mod_obj));

		_JsonOutput["UserSettings"]["ScrapModsPath"] = _ModList;
	}

	if (open_in_steam)
		_JsonOutput["UserSettings"]["OpenLinksInSteam"] = SMBC::Settings::OpenLinksInSteam;

	_ConfigJson << std::setfill('\t') << std::setw(1) << _JsonOutput;

	_ConfigJson.close();
}

void SMBC::Settings::AddRegistryPathAndSave() {
	if (!SMBC::Settings::PathToSM.empty()) return;

	System::String^ app_data_path = System::Environment::GetFolderPath(System::Environment::SpecialFolder::ApplicationData);
	
	std::wstring SMLocalData = msclr::interop::marshal_as<std::wstring>(app_data_path);
	SMLocalData.append(L"\\Axolot Games\\Scrap Mechanic\\User");

	if (SMBC::File::Exists(SMLocalData)) {
		fs::directory_iterator DirIter(SMLocalData, fs::directory_options::skip_permission_denied);

		for (auto& dir : DirIter) {
			if (!dir.is_directory()) continue;

			std::wstring BPPath = dir.path().wstring() + L"\\Blueprints";
			std::wstring ModPath = dir.path().wstring() + L"\\Mods";

			if (SMBC::File::Exists(BPPath))
				SMBC::Settings::AddArrayPath(BPPath, SMBC::Settings::BlueprintFolders);

			if (SMBC::File::Exists(ModPath))
				SMBC::Settings::AddArrayPath(ModPath, SMBC::Settings::ModFolders);
		}
	}

	std::wstring _Path = SMBC::Other::ReadRegistryKey(L"SOFTWARE\\Valve\\Steam", L"SteamPath");
	if (_Path.empty() || !SMBC::File::Exists(_Path))
		_Path = SMBC::Other::ReadRegistryKey(L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"SteamPath");

	if (_Path.empty() || !SMBC::File::Exists(_Path)) return;

	std::wstring _ScrapPath = _Path + L"/steamapps/common/scrap mechanic";
	std::wstring _ScrapWorkshop = _Path + L"/steamapps/workshop/content/387990";

	if (SMBC::File::Exists(_ScrapPath)) SMBC::Settings::PathToSM = _ScrapPath;
	if (SMBC::File::Exists(_ScrapWorkshop)) {
		SMBC::Settings::AddArrayPath(_ScrapWorkshop, SMBC::Settings::ModFolders);
		SMBC::Settings::AddArrayPath(_ScrapWorkshop, SMBC::Settings::BlueprintFolders);
	}

	SMBC::Settings::SaveSettingsFile(true, true, true);
}