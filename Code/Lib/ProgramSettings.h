#pragma once

#include <vector>
#include <string>

#include "Lib\JsonFunc.h"

namespace SMBC {
	class Settings {
	public:
		constexpr static const std::wstring_view ConfigPath = L"./Resources/Config.json";
		constexpr static const std::wstring_view RotSetPath = L"./Resources/RotationSettings.json";
		constexpr static const std::wstring_view MaterialMapPath = L"./Resources/MaterialIds.json";

		inline static std::vector<std::wstring> BlueprintFolders = {};
		inline static std::vector<std::wstring> ModFolders = {};
		inline static std::vector<std::wstring> SMDirDatabase = {};
		inline static std::vector<std::wstring> VanillaLanguagePaths = {};
		inline static std::wstring PathToSM = L"";
		inline static bool OpenLinksInSteam = false;

		static void JsonStrArrayToVector(const nlohmann::json& pJson, const std::string& pKey, std::vector<std::wstring>& pWstrVec);
		static void WstrVecToJsonArray(nlohmann::json& out_json, std::vector<std::wstring>& mWstrVec, const std::string& pKey);
		static void AddToStrVec(std::vector<std::wstring>& mWstrVec, const std::wstring& mWstr);

		static void ReadProgramSettings(const nlohmann::json& config_json);

		static bool GetStaemPaths(std::wstring& game_path, std::wstring& workshop_path);
		static void FindLocalUsers();
		static void FindGamePath(nlohmann::json& config_json, bool& should_write);
		static void FillUserSettings(nlohmann::json& config_json, bool& should_write);
		static void ReadUserSettings(nlohmann::json& config_json, bool& should_write);


		static nlohmann::json GetConfigJson(bool* should_write = nullptr);
		static void ClearData();
	public:
		static void SaveConfig();
		static void ReadConfig();
	};
}