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
	public:
		ConvertError() = default;

		inline void operator=(const std::wstring& error_msg)
		{
			this->ErrorMsg = error_msg;
		}

		inline explicit operator bool() const noexcept
		{
			return !this->ErrorMsg.empty();
		}

		inline std::wstring GetString() const noexcept
		{
			return this->ErrorMsg;
		}

	private:
		std::wstring ErrorMsg;
	};

	//Blueprint Conversion Data
	class ConvData
	{
	public:
		inline static ConvState State           = ConvState::None;
		inline static std::size_t ProgressMax   = 0;
		inline static std::size_t ProgressValue = 0;

		inline static void SetState(const ConvState& state, const std::size_t& MaxValue)
		{
			ConvData::State = state;

			ConvData::ProgressValue = 0;
			ConvData::ProgressMax = MaxValue;
		}

		inline static void SetState(const ConvState& state)
		{
			ConvData::State = state;
		}

		inline static std::wstring GetStateString()
		{
			const std::size_t v_stateIdx = static_cast<std::size_t>(ConvData::State);
			if (v_stateIdx > 0)
				return g_actionData[v_stateIdx - 1].v_desc;

			return L"NO_STATE";
		}

		inline static bool StateHasNumbers()
		{
			const std::size_t v_stateIdx = static_cast<std::size_t>(ConvData::State);
			if (v_stateIdx > 0)
				return g_actionData[v_stateIdx - 1].v_displayNumbers;

			return false;
		}

	private:
		struct StateData
		{
			std::wstring v_desc;
			bool v_displayNumbers;
		};

		inline const static StateData g_actionData[] =
		{
			{ L"Reading Object Database... ",            true  },
			{ L"Reading Blueprint JSON...",              false },
			{ L"Reading Blueprint Blocks and Parts... ", true  },
			{ L"Reading Blueprint Joints...",            true  },
			{ L"Clearing Blueprint Data...",             false },
			{ L"Writing Objects to the File... ",        true  },
			{ L"Writing Mtl File... ",                   false },
			{ L"Writing Texture File... ",               false },

			{ L"Success!",         false },
			{ L"Conversion Error", false }
		};
	};
}