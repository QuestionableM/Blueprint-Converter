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

		bool has_uvs;
		bool has_normals;

		inline bool IsEmpty() { return this->DataIdx.empty(); }

		inline SubMeshData(const int& sub_mesh_idx) { this->SubMeshIndex = sub_mesh_idx; }
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

		inline Model(const std::wstring& mesh_path)
		{
			this->meshPath = mesh_path;
		}

		Model(Model&) = delete;
		Model(Model&&) = delete;
		Model(const Model&) = delete;

		inline ~Model()
		{
			for (SMBC::SubMeshData*& v_curSubMesh : this->subMeshData)
				delete v_curSubMesh;
		}

		void WriteToFile(std::ofstream& file, const glm::mat4& model_mat, OffsetData& mOffset, const class Entity* pEntity) const;

		inline glm::vec3 GetCenterPoint() const
		{
			return mCenterPoint;
		}

		inline bool IsEmpty()
		{
			return (this->subMeshData.size() <= 0 || (this->vertices.size() <= 0 && this->uvs.size() <= 0 && this->normals.size() <= 0));
		}
	};
}