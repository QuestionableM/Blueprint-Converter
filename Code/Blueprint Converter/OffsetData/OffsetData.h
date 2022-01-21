#pragma once

#include <glm.hpp>

namespace SMBC
{
	struct OffsetData
	{
		std::size_t Vertex  = 0;
		std::size_t Normal  = 0;
		std::size_t Texture = 0;

		std::size_t BlockNormalOffset = 0;
		bool BlockDataWritten = false;

		glm::vec3   pt_sum   = { 0.0f, 0.0f, 0.0f };
		std::size_t pt_count = 0;

		void UpdateValues(const glm::vec3& val);
	};
}