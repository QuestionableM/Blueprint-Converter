#pragma once

#include <vector>
#include <string>

namespace SMBC
{
	enum class State : long
	{
		None =				-1,
		ReadingJson =		0,
		GettingObjects =	1,
		GettingJoints =		2,
		ReadingObjects =	3,
		ReadingDatabase =	4,
		WritingObjects =	5
	};

	class ConvertError
	{
		std::wstring ErrorMsg;
	public:
		ConvertError() = default;

		void operator=(const std::wstring& error_msg);

		explicit operator bool() const noexcept;
		std::wstring GetString() const noexcept;
	};

	//Blueprint Conversion Data
	class ConvData
	{
	public:
		static State State;
		static std::size_t ProgressMax;
		static std::size_t ProgressValue;

		static void SetState(const SMBC::State& state, const std::size_t& MaxValue = 0);
	};

	const static std::vector<std::wstring> ActionTable =
	{
		L"Reading Blueprint JSON...",
		L"Getting Blueprint Blocks and Parts... ",
		L"Getting Blueprint Joints... ",
		L"Reading Blocks and Parts... ",
		L"Reading Object Database... ",
		L"Writing Objects Into the File... "
	};
}