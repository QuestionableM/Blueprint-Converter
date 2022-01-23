#pragma once

#include <glm.hpp>
#include <vector>
#include <unordered_map>

#include <gtx/hash.hpp>

namespace SMBC
{
	struct OffsetData
	{
		std::unordered_map<glm::vec3, std::size_t> VertexMap = {};
		std::unordered_map<glm::vec3, std::size_t> NormalMap = {};
		std::unordered_map<glm::vec2, std::size_t> UvMap = {};

		std::size_t Vertex  = 0;
		std::size_t Normal  = 0;
		std::size_t Texture = 0;

		glm::vec3   pt_sum   = { 0.0f, 0.0f, 0.0f };
		std::size_t pt_count = 0;

		void UpdateValues(const glm::vec3& val);
	};
}