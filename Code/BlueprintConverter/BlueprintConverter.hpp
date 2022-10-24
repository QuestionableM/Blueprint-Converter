#pragma once

#include "Lib\ConvData.hpp"

#include <glm.hpp>
#include <string>

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