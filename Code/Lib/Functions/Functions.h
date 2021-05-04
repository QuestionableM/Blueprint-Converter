#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <Windows.h>

namespace SMBC {
	class BlueprintConversionData {
	public:
		static long Stage;
		static long ProgressBarMax;
		static long ProgressBarValue;

		static void SetNewStage(const long& Stage, const long& MaxValue);
	};

	class Blueprint {
	public:
		static std::vector<std::wstring> ImageExtensions;
		static std::vector<std::wstring> BlueprintPaths;
		std::wstring BlueprintName;
		std::wstring BlueprintPath;
		std::wstring BlueprintFolder;

		bool IsSupportedExtension(const std::wstring& _ext);
		static std::wstring FixBlueprintName(const std::wstring& name);
		std::wstring FindBlueprintImage();
		Blueprint(const std::wstring& name, const std::wstring& path, const std::wstring& folder);
	};

	const static std::vector<std::wstring> ActionTable = {
		L"Reading Blueprint JSON...",
		L"Getting Blueprint Blocks and Parts... ",
		L"Getting Blueprint Joints... ",
		L"Reading Blueprint Blocks... ",
		L"Reading Blueprint Parts... ",
		L"Writing Vertices into the File... ",
		L"Writing Face Indexes into the File... ",
		L"Reading Object Database... ",
		L"Writing Texture Coordinates into the File... ",
		L"Writing Normals into the file... ",
		L"Writing Objects into the file... "
	};

	const static std::vector<std::wstring> ConversionErrorTable = {
		L"No error. This message should never be seen!",
		L"Couldn't open the specified blueprint file. Possible reasons: Syntax error, Is not a json file",
		L"Couldn't write converted blueprint into file!",
		L"The specified blueprint doesn't have any blocks and parts to convert!",
		L"The specified blueprint file doesn't contain any blueprint information!"
	};

	static int LastGenerationOutput = 0;
};