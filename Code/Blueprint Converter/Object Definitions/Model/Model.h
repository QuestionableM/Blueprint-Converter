#pragma once

#include <glm.hpp>
#include <string>
#include <vector>

namespace SMBC
{
	struct OffsetData;

	struct VertexData
	{
		long long pVert;
		long long pUv;
		long long pNorm;
	};

	struct SubMeshData
	{
		std::wstring MaterialName;
		unsigned int SubMeshIndex;

		std::vector<std::vector<VertexData>> DataIdx;

		bool IsEmpty();
		SubMeshData(const int& sub_mesh_idx);
		~SubMeshData() = default;
	};

	struct Model
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<SMBC::SubMeshData*> subMeshData;

		std::wstring meshPath;

		long long WrittenUvIdx = -1ll;
		long long WrittenNormalIdx = -1ll;

	public:
		Model(const long long& written_uv_idx, const long long& written_normal_idx);
		Model(const std::wstring& mesh_path);
		Model(Model&) = delete;
		Model(Model&&) = delete;
		Model(const Model&) = delete;
		~Model();

		void WriteToFile(std::ofstream& file, const glm::mat4& model_mat, OffsetData& mOffset, const class Entity* pEntity);

		bool IsEmpty();
	};
}