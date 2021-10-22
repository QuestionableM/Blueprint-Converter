#pragma once

#include <string>
#include <glm.hpp>

#include "Lib/ConvData/ConvData.h"

namespace SMBC {
	enum : int {
		Sep_Blocks =		1,
		Sep_Joints =		2,
		Sep_Uuid =			3,
		Sep_Color =			4,
		Sep_UuidAndColor =	5
	};

	namespace BPFunction {
		SMBC::Error ConvertBlueprintToObj(
			const std::wstring& blueprint_path,
			const std::wstring blueprint_name,
			const int& separation_method,
			const bool& texture_list,
			const bool& apply_textures,
			const bool& export_uvs,
			const bool& export_normals,
			const bool& mat_by_color
		);

		void GetPartPosAndBounds(
			glm::vec3& pos,
			glm::vec3& bounds,
			const int& xAxis,
			const int& zAxis,
			const bool& isJoint
		);

		glm::vec3 GetPartRotation(
			const glm::vec3& _Position,
			const glm::vec3& bounds,
			const int& xAxis,
			const int& zAxis
		);
	};
};