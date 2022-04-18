#pragma once

#include <vector>
#include <string>

namespace SMBC
{
	enum class ConvState : std::size_t
	{
		None = 0,

		DB_ReadingDatabase = 1,

		BP_ReadingJson     = 2,
		BP_ReadingObjects  = 3,
		BP_ReadingJoints   = 4,
		BP_ClearingJson    = 5,
		BP_WritingObjects  = 6,
		BP_WritingMtl      = 7,
		BP_WritingTextures = 8,

		CV_Success = 9,
		CV_Failure = 10
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
		inline static ConvState State           = ConvState::None;
		inline static std::size_t ProgressMax   = 0;
		inline static std::size_t ProgressValue = 0;

		static void SetState(const ConvState& state, const std::size_t& MaxValue);
		static void SetState(const ConvState& state);

		static std::wstring GetStateString();
		static bool StateHasNumbers();
	};
}