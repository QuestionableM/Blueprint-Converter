#pragma once

#include <glm.hpp>

namespace SMBC
{
	struct AxisData;

	namespace Rotations
	{
		void InitializeRotations();
		bool RotationExists(const int& xAxis, const int& zAxis);

		glm::mat4 GetRotationMatrix(const AxisData& mAxis);
		glm::vec3 GetOffsetPosition(const AxisData& mAxis);
	}
}