#pragma once

#include <string>
#include <glm.hpp>

#include "Lib/ConvData/ConvData.h"

namespace SMBC
{
	enum : int
	{
		Sep_Blocks =		1,
		Sep_Joints =		2,
		Sep_Uuid =			3,
		Sep_Color =			4,
		Sep_UuidAndColor =	5
	};

	namespace BPFunction
	{
		void ConvertBlueprintToObj(const std::wstring& bp_path, const std::wstring& bp_name, ConvertError& cError);
	};
};