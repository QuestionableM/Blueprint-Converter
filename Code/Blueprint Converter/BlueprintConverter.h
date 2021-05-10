#pragma once

#include <string>
#include <vector>
#include <glm.hpp>

#include "Object Database/ObjectDatabase.h"
#include "Cache Manager/CacheManager.h"

#define SMBC_SEPARATION_BLOCKS 1
#define SMBC_SEPARATION_JOINTS 2

namespace SMBC {
	namespace BPFunction {
		int ConvertBlueprintToObj(
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