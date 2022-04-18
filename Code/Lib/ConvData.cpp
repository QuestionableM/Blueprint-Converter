#include "ConvData.h"

namespace SMBC
{
	void ConvertError::operator=(const std::wstring& error_msg)
	{
		this->ErrorMsg = error_msg;
	}

	ConvertError::operator bool() const noexcept
	{
		return !this->ErrorMsg.empty();
	}

	std::wstring ConvertError::GetString() const noexcept
	{
		return this->ErrorMsg;
	}



	void ConvData::SetState(const ConvState& state, const std::size_t& MaxValue)
	{
		ConvData::State = state;

		ConvData::ProgressValue = 0;
		ConvData::ProgressMax = MaxValue;
	}

	void ConvData::SetState(const ConvState& state)
	{
		ConvData::State = state;
	}

	struct StateData
	{
		std::wstring Description;
		bool DisplayNumbers;
	};

	const static std::vector<StateData> ActionTable =
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


	std::wstring ConvData::GetStateString()
	{
		const std::size_t state_idx = static_cast<std::size_t>(ConvData::State);
		if (state_idx > 0)
		{
			return ActionTable[state_idx - 1].Description;
		}

		return L"NO_STATE";
	}

	bool ConvData::StateHasNumbers()
	{
		const std::size_t state_idx = static_cast<std::size_t>(ConvData::State);
		if (state_idx > 0)
		{
			return ActionTable[state_idx - 1].DisplayNumbers;
		}

		return false;
	}
}