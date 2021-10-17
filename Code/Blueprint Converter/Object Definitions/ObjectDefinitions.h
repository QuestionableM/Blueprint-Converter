#pragma once

#include <glm.hpp>
#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Mod Data/ModData.h"

namespace SMBC {
	struct SubMeshData {
		std::wstring MaterialName;
		int SubMeshIndex;

		std::vector<std::vector<std::vector<long long>>> DataIdx;

		bool is_empty();
	};

	struct Model {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<SMBC::SubMeshData*> subMeshData;

		std::wstring meshPath;

		bool is_empty();

		~Model();
	};

	struct CachedBlock {
		SMBC::BlockData* blkPtr = nullptr;
		std::wstring color;
	};

	struct CachedPart {
		SMBC::PartData* objPtr = nullptr;
		SMBC::Model* modelPtr = nullptr;

		std::wstring color;
	};

	struct SM_Part {
		SMBC::PartData* objPtr = nullptr;
		SMBC::Model* modelPtr = nullptr;

		std::wstring color;
		glm::vec3 position;
		glm::vec3 bounds;
		int xAxis;
		int zAxis;
		int _index;
	};

	struct SM_Block {
		SMBC::BlockData* blkPtr = nullptr;

		glm::vec3 position;
		glm::vec3 bounds;
		int xAxis;
		int zAxis;
		SMBC::Uuid uuid;
		std::wstring color;
		long _index;
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
	};
}