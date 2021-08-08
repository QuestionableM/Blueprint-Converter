#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <Windows.h>

namespace SMBC {
	enum : long {
		Stat_None =				-1,
		Stat_ReadingJson =		0,
		Stat_GettingObjects =	1,
		Stat_GettingJoints =	2,
		Stat_ReadingObjects =	3,
		Stat_ReadingDatabase =	4,
		Stat_WritingObjects =	5
	};

	enum : int {
		Err_None =			0,
		Err_File =			1,
		Err_Write =			2,
		Err_NoBlocks =		3,
		Err_NoBpData =		4,
		Err_MainDirCreate = 5,
		Err_BPDirCreate =	6,
		Err_InvalidPath =	7
	};

	//Blueprint Conversion Data
	class BPConvData {
	public:
		static long Stage;
		static long ProgressBarMax;
		static long ProgressBarValue;

		static void SetNewStage(const long& Stage, const long& MaxValue = 0);
	};

	class Blueprint {
	public:
		static std::vector<std::wstring> ImageExtensions;
		std::wstring Name;
		std::wstring Path;
		std::wstring Folder;
		std::wstring WorkshopId;
		std::wstring LowerName;

		bool IsSupportedExtension(const std::wstring& _ext);
		static std::wstring FixBlueprintName(const std::wstring& name);
		std::wstring FindBlueprintImage();
		bool BlueprintExists();

		Blueprint(
			const std::wstring& name,
			const std::wstring& path,
			const std::wstring& folder,
			const std::wstring& workshop_id
		);

		Blueprint() = default;
	};

	const static std::vector<std::wstring> ActionTable = {
		L"Reading Blueprint JSON...",
		L"Getting Blueprint Blocks and Parts... ",
		L"Getting Blueprint Joints... ",
		L"Reading Blocks and Parts... ",
		L"Reading Object Database... ",
		L"Writing Objects Into the File... "
	};

	const static std::vector<std::wstring> ConversionErrorTable = {
		L"No error. This message should never be seen!",
		L"Couldn't open the specified blueprint file. Possible reasons: Syntax error, Is not a json file",
		L"Couldn't write converted blueprint into file!",
		L"The specified blueprint doesn't have any blocks and parts to convert!",
		L"The specified blueprint file doesn't contain any blueprint information!",
		L"Couldn't create \"Converted Models\" directory!",
		L"Couldn't create a directory with the specified blueprint!",
		L"Blueprint name is invalid!"
	};
};