#pragma once

#include <vector>
#include <string>

namespace SMBC {
	enum class State : long {
		None =				-1,
		ReadingJson =		0,
		GettingObjects =	1,
		GettingJoints =		2,
		ReadingObjects =	3,
		ReadingDatabase =	4,
		WritingObjects =	5
	};

	enum class Error : int {
		None =			0,
		File =			1,
		Write =			2,
		NoBlocks =		3,
		NoBpData =		4,
		MainDirCreate = 5,
		BPDirCreate =	6,
		InvalidPath =	7
	};

	//Blueprint Conversion Data
	class ConvData {
	public:
		static State State;
		static unsigned long long ProgressMax;
		static unsigned long long ProgressValue;

		static void SetState(const SMBC::State& state, const unsigned long long& MaxValue = 0ull);
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
}