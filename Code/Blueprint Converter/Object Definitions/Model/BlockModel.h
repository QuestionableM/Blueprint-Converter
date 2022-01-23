#pragma once

#include "Blueprint Converter/Object Definitions/Model/Model.h"

namespace SMBC
{
	class BlockModel
	{
		BlockModel() = default;

		static void FillBlockData(Model& pModel, const glm::vec3& bounds);
		static void FillBlockUvs(Model& pModel, const glm::vec3& bounds, const glm::vec3& pos, const int& tiling);
		static void CalculateCenterPoint(Model& pModel);
	public:
		static void CreateBlockModel(Model& pModel, const glm::vec3& pos, const glm::vec3& scale, const int& tiling);
	};
}