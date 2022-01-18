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


	State ConvData::State = State::None;
	std::size_t ConvData::ProgressMax = 0ull;
	std::size_t ConvData::ProgressValue = 0ull;

	void ConvData::SetState(const SMBC::State& state, const std::size_t& MaxValue)
	{
		ConvData::State = state;
		ConvData::ProgressValue = 0;
		ConvData::ProgressMax = MaxValue;
	}
}