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

		void GetPartPosAndBounds(
			glm::vec3& pos,
			glm::vec3& bounds,
			const int& xAxis,
			const int& zAxis,
			const bool& isJoint
		);

		glm::mat4 GetPartRotationMatrix(const int& xAxis, const int& zAxis);
		glm::vec3 GetPartRotationFromMatrix(const glm::vec3& pos, const glm::vec3& bounds, const glm::mat4& matrix);
	};
};