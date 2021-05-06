#pragma once

#include <glm.hpp>
#include "Object Database/Texture Database/TextureDatabase.h"

namespace SMBC {
	struct SM_Part {
		SMBC::Texture::Texture tex;
		std::wstring object_path;
		std::wstring name;
		std::wstring uuid;
		std::wstring color;
		glm::vec3 position;
		glm::vec3 bounds;
		int xAxis;
		int zAxis;

		SM_Part(
			const std::wstring& path,
			const std::wstring& name,
			SMBC::Texture::Texture& tex,
			const glm::vec3& pos,
			const glm::vec3& bounds,
			const int& _XAxis,
			const int& _ZAxis,
			const std::wstring& uuid,
			const std::wstring& color
		);
	};

	struct SM_Block {
		glm::vec3 position;
		glm::vec3 bounds;
		int xAxis;
		int zAxis;
		std::wstring name;
		SMBC::Texture::TextureList tex_list;
		std::wstring uuid;
		int tiling;
		std::wstring color;

		SM_Block(
			const glm::vec3& pos,
			const glm::vec3& bounds,
			const int& _XAxis,
			const int& _ZAxis,
			const std::wstring& name,
			const SMBC::Texture::TextureList& tex_list,
			const std::wstring& uuid,
			const int& tiling,
			const std::wstring& color
		);
	};

	struct CubeMesh {
		std::vector<glm::vec3> Vertices;
		std::vector<glm::vec2> TexturePoints = {
			{0.0f, 1.0f}, {1.0f, 0.0f},
			{1.0f, 1.0f}, {1.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f},
			{1.0f, 0.0f}, {0.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f},
			{1.0f, 0.0f}, {0.0f, 1.0f},
			{0.0f, 0.0f}, {1.0f, 0.0f},
			{0.0f, 1.0f}, {0.0f, 0.0f},
			{0.0f, 0.0f}, {0.0f, 1.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f},
			{1.0f, 1.0f}, {1.0f, 1.0f}
		};

		/*
			reference
			this->TexturePoints = {
				{4.0f, 3.0f}, {5.0f, 2.0f},
				{5.0f, 3.0f}, {3.0f, 3.0f},
				{2.0f, 2.0f}, {3.0f, 2.0f},
				{5.0f, 0.0f}, {4.0f, 1.0f},
				{4.0f, 0.0f}, {1.0f, 1.0f},
				{0.0f, 0.0f}, {1.0f, 0.0f},
				{1.0f, 2.0f}, {0.0f, 3.0f},
				{0.0f, 2.0f}, {3.0f, 0.0f},
				{2.0f, 1.0f}, {2.0f, 0.0f},
				{4.0f, 2.0f}, {2.0f, 3.0f},
				{5.0f, 1.0f}, {0.0f, 1.0f},
				{1.0f, 3.0f}, {3.0f, 1.0f}
			}
		*/

		std::vector<glm::vec3> Normals = {
			{-1.0, 0.0, 0.0}, {0.0, 0.0, -1.0},
			{1.0, 0.0, 0.0}, {0.0, 0.0, 1.0},
			{0.0, -1.0, 0.0}, {0.0, 1.0, 0.0}
		};
		std::vector<std::vector<std::vector<uint32_t>>> DataIndexes = {
			{{0, 0, 0}, {1, 1, 0}, {2, 2, 0}},
			{{3, 3, 1}, {4, 4, 1}, {1, 5, 1}},
			{{5, 6, 2}, {6, 7, 2}, {4, 8, 2}},
			{{7, 9, 3}, {2, 10, 3}, {6, 11, 3}},
			{{4, 12, 4}, {2, 13, 4}, {1, 14, 4}},
			{{3, 15, 5}, {7, 16, 5}, {5, 17, 5}},
			{{0, 0, 0}, {3, 18, 0}, {1, 1, 0}},
			{{3, 3, 1}, {5, 19, 1}, {4, 4, 1}},
			{{5, 6, 2}, {7, 20, 2}, {6, 7, 2}},
			{{7, 9, 3}, {0, 21, 3}, {2, 10, 3}},
			{{4, 12, 4}, {6, 22, 4}, {2, 13, 4}},
			{{3, 15, 5}, {0, 23, 5}, {7, 16, 5}}
		};

	public:
		CubeMesh(const glm::vec3& bounds, const glm::vec3& pos, const int& tiling);
		~CubeMesh();
	};
}