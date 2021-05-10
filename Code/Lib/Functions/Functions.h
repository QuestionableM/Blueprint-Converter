#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <Windows.h>

#define SMBC_CONV_NONE -1
#define SMBC_CONV_READING_JSON 0
#define SMBC_CONV_GETTING_OBJECTS 1
#define SMBC_CONV_GETTING_JOINTS 2
#define SMBC_CONV_READING_BLOCKS 3
#define SMBC_CONV_READING_PARTS 4
#define SMBC_CONV_WRITING_VERTICES 5
#define SMBC_CONV_WRITING_FACES 6
#define SMBC_CONV_READING_DATABASE 7
#define SMBC_CONV_WRITING_UVS 8
#define SMBC_CONV_WRITING_NORMALS 9
#define SMBC_CONV_WRITING_OBJECTS 10

#define SMBC_ERROR_NONE 0
#define SMBC_ERROR_FILE 1
#define SMBC_ERROR_WRITE 2
#define SMBC_ERROR_NO_BLOCKS 3
#define SMBC_ERROR_NO_BP_DATA 4

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