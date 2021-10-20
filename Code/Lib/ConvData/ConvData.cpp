#include "ConvData.h"

namespace SMBC
{
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