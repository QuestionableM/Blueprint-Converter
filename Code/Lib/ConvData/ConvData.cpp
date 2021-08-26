#include "ConvData.h"

typedef SMBC::ConvData _ConvData;

SMBC::State _ConvData::State = SMBC::State::None;
unsigned long long _ConvData::ProgressMax = 0ull;
unsigned long long _ConvData::ProgressValue = 0ull;

void _ConvData::SetState(const SMBC::State& state, const unsigned long long& MaxValue) {
	_ConvData::State = state;
	_ConvData::ProgressValue = 0;
	_ConvData::ProgressMax = MaxValue;
}