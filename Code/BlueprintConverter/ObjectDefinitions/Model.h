#pragma once

#include <glm.hpp>
#include <string>
#include <vector>

namespace SMBC
{
	struct OffsetData;

	struct VertexData
	{
		std::size_t m_Vert;
		std::size_t m_Uv;
		std::size_t m_Norm;
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
		glm::vec3 mCenterPoint;

	public:
		Model(const std::wstring& mesh_path);
		Model(Model&) = delete;
		Model(Model&&) = delete;
		Model(const Model&) = delete;
		~Model();

		void WriteToFile(std::ofstream& file, const glm::mat4& model_mat, OffsetData& mOffset, const class Entity* pEntity) const;
		glm::vec3 GetCenterPoint() const;

		bool IsEmpty();
	};
}