#pragma once

#include <glm.hpp>
#include <vector>
#include <unordered_map>

#include <gtx/hash.hpp>

namespace SMBC
{
	struct OffsetData
	{
		using Vec3StorageMap = std::unordered_map<glm::vec3, std::size_t>;
		using Vec2StorageMap = std::unordered_map<glm::vec2, std::size_t>;

		Vec3StorageMap VertexMap = {};
		Vec3StorageMap NormalMap = {};
		Vec2StorageMap UvMap = {};

		std::size_t Vertex  = 0;
		std::size_t Normal  = 0;
		std::size_t Texture = 0;

		glm::vec3 PointOffset = { 0.0f, 0.0f, 0.0f };
	};
}