#pragma once

#include <vector>
#include <string>

#include "Lib/Json/JsonFunc.h"

namespace SMBC {
	class Settings {
		static std::wstring ConfigPath;

		static void LoadJsonWstrArray(
			const nlohmann::json& file,
			const std::string& keyword,
			std::vector<std::wstring>& _array
		);
		static void AddArrayPath(const std::wstring& element, std::vector<std::wstring>& _array);

	public:
		static std::vector<std::wstring> BlueprintFolders;
		static std::vector<std::wstring> ModFolders;
		static std::vector<std::wstring> SMDirDatabase;
		static std::vector<std::wstring> VanillaLanguagePaths;
		static std::wstring PathToSM;
		static bool OpenLinksInSteam;

		static void LoadSettingsFile();
		static void SaveSettingsFile(
			const bool sm_path = false,
			const bool bp_list = false,
			const bool mod_list = false,
			const bool open_in_steam = false
		);

		static void AddRegistryPathAndSave();
	};
}