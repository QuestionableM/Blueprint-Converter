#include "ProgramSettings.h"
#include "Lib/OtherFunc/OtherFunc.h"
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

		std::wstring _WstrPath = SMBC::Other::Utf8ToWide(_wstr.get<std::string>());
		_WstrPath = SMBC::PathReplacer::ReplaceKey(_WstrPath);

		_array.push_back(_WstrPath);
	}
}

void SMBC::Settings::AddArrayPath(
	const std::wstring& element,
	std::vector<std::wstring>& _array
) {
	for (std::wstring& _wstr : _array) {
		if (_wstr == element || SMBC::FILE::IsEquivalent(_wstr, element))
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
	nlohmann::json _ConfigFile;
	if (!SMBC::JSON::OpenParseJson(SMBC::Settings::ConfigPath, _ConfigFile)) return;

	SMBC::Settings::BlueprintFolders.clear();
	SMBC::Settings::ModFolders.clear();
	SMBC::Settings::PathToSM.clear();
	SMBC::Settings::VanillaLanguagePaths.clear();
	SMBC::Settings::OpenLinksInSteam = false;
	SMBC::PathReplacer::ClearAllData();
	SMBC::Settings::SMDirDatabase.clear();
	SMBC::ObjectDatabase::ModDB.clear();

	auto& _ConfigData = _ConfigFile;

	auto& _UserSettings = _ConfigData["UserSettings"];
	if (_UserSettings.is_object()) {
		auto& _SMPath = _UserSettings["ScrapPath"];
		auto& _OpenInSteam = _UserSettings["OpenLinksInSteam"];

		SMBC::Settings::LoadJsonWstrArray(_UserSettings, "BlueprintPaths", SMBC::Settings::BlueprintFolders);
		SMBC::Settings::LoadJsonWstrArray(_UserSettings, "ScrapModsPath", SMBC::Settings::ModFolders);

		if (_SMPath.is_string())
			SMBC::Settings::PathToSM = SMBC::Other::Utf8ToWide(_SMPath.get<std::string>());

		if (_OpenInSteam.is_boolean())
			SMBC::Settings::OpenLinksInSteam = _OpenInSteam.get<bool>();
	}

	SMBC::Settings::AddRegistryPathAndSave();
	if (!SMBC::Settings::PathToSM.empty())
		SMBC::PathReplacer::Add(SMBC::KeyReplacement(L"$GAME_FOLDER", SMBC::Settings::PathToSM));

	auto& _ProgramSettings = _ConfigData["ProgramSettings"];
	if (_ProgramSettings.is_object()) {
		auto& _KeyWords = _ProgramSettings["Keywords"];

		if (_KeyWords.is_object()) {
			for (auto& keyword : _KeyWords.items()) {
				if (!keyword.value().is_string()) continue;

				std::wstring _WstrKey = SMBC::Other::Utf8ToWide(keyword.key());
				std::wstring _WstrRepl = SMBC::Other::Utf8ToWide(keyword.value().get<std::string>());
				_WstrRepl = SMBC::PathReplacer::ReplaceKey(_WstrRepl);

				SMBC::PathReplacer::Add(SMBC::KeyReplacement(_WstrKey, _WstrRepl));
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
	nlohmann::json _JsonOutput;
	SMBC::JSON::OpenParseJson(SMBC::Settings::ConfigPath, _JsonOutput);

	std::filesystem::create_directory(L"./Resources");
	std::ofstream _ConfigJson(SMBC::Settings::ConfigPath);

	if (!_ConfigJson.is_open()) return;

	if (_JsonOutput.contains("UserSettings") && !_JsonOutput.at("UserSettings").is_object())
		_JsonOutput["UserSettings"] = {};

	if (sm_path)
		_JsonOutput["UserSettings"]["ScrapPath"] = SMBC::Other::WideToUtf8(SMBC::Settings::PathToSM);

	if (bp_list) {
		nlohmann::json _BlueprintList = {};

		for (std::wstring& _bp_obj : SMBC::Settings::BlueprintFolders)
			_BlueprintList.push_back(SMBC::Other::WideToUtf8(_bp_obj));

		_JsonOutput["UserSettings"]["BlueprintPaths"] = _BlueprintList;
	}

	if (mod_list) {
		nlohmann::json _ModList = {};

		for (std::wstring& _mod_obj : SMBC::Settings::ModFolders)
			_ModList.push_back(SMBC::Other::WideToUtf8(_mod_obj));

		_JsonOutput["UserSettings"]["ScrapModsPath"] = _ModList;
	}

	if (open_in_steam)
		_JsonOutput["UserSettings"]["OpenLinksInSteam"] = SMBC::Settings::OpenLinksInSteam;

	_ConfigJson << std::setw(4) << _JsonOutput;

	_ConfigJson.close();
}

void SMBC::Settings::AddRegistryPathAndSave() {
	if (!SMBC::Settings::PathToSM.empty()) return;

	System::String^ app_data_path = System::Environment::GetFolderPath(System::Environment::SpecialFolder::ApplicationData);
	
	std::wstring SMLocalData = msclr::interop::marshal_as<std::wstring>(app_data_path);
	SMLocalData += L"\\Axolot Games\\Scrap Mechanic\\User";

	if (SMBC::FILE::FileExists(SMLocalData)) {
		fs::directory_iterator DirIter(SMLocalData, fs::directory_options::skip_permission_denied);

		for (auto& dir : DirIter) {
			if (!dir.is_directory()) continue;

			std::wstring BPPath = dir.path().wstring() + L"\\Blueprints";
			std::wstring ModPath = dir.path().wstring() + L"\\Mods";

			if (SMBC::FILE::FileExists(BPPath))
				SMBC::Settings::AddArrayPath(BPPath, SMBC::Settings::BlueprintFolders);

			if (SMBC::FILE::FileExists(ModPath))
				SMBC::Settings::AddArrayPath(ModPath, SMBC::Settings::ModFolders);
		}
	}

	std::wstring _Path = SMBC::Other::ReadRegistryKey(L"SOFTWARE\\Valve\\Steam", L"SteamPath");
	if (_Path.empty() || !SMBC::FILE::FileExists(_Path))
		_Path = SMBC::Other::ReadRegistryKey(L"SOFTWARE\\WOW6432Node\\Valve\\Steam", L"SteamPath");

	if (_Path.empty() || !SMBC::FILE::FileExists(_Path)) return;

	std::wstring _ScrapPath = _Path + L"/steamapps/common/scrap mechanic";
	std::wstring _ScrapWorkshop = _Path + L"/steamapps/workshop/content/387990";

	if (SMBC::FILE::FileExists(_ScrapPath)) SMBC::Settings::PathToSM = _ScrapPath;
	if (SMBC::FILE::FileExists(_ScrapWorkshop)) {
		SMBC::Settings::AddArrayPath(_ScrapWorkshop, SMBC::Settings::ModFolders);
		SMBC::Settings::AddArrayPath(_ScrapWorkshop, SMBC::Settings::BlueprintFolders);
	}

	SMBC::Settings::SaveSettingsFile(true, true, true);
}