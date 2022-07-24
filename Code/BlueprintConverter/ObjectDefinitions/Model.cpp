#include "Model.h"

#include "BlueprintConverter\ObjectDefinitions\Entity.h"
#include "BlueprintConverter\ConvertSettings.h"
#include "BlueprintConverter\OffsetData.h"

#include "Lib\String.h"

#include "DebugCon.h"

#include <fstream>

namespace SMBC
{
	bool SubMeshData::IsEmpty()
	{
		return (this->DataIdx.size() <= 0);
	}

	SubMeshData::SubMeshData(const int& sub_mesh_idx)
	{
		this->SubMeshIndex = sub_mesh_idx;
	}



	Model::Model(const std::wstring& mesh_path)
	{
		this->meshPath = mesh_path;
	}

	Model::~Model()
	{
		for (SMBC::SubMeshData*& data_ptr : this->subMeshData)
			delete data_ptr;
	}

	void Model::WriteToFile(std::ofstream& file, const glm::mat4& model_mat, OffsetData& mOffset, const class Entity* pEntity) const
	{
		std::vector<glm::vec3> translated_vertices = {};
		std::vector<glm::vec3> translated_normals = {};

		translated_vertices.resize(this->vertices.size());
		translated_normals.resize(this->normals.size());

		for (std::size_t a = 0; a < this->vertices.size(); a++)
		{
			const glm::vec3& vertex = this->vertices[a];

			const glm::vec3 pVertRes = model_mat * glm::vec4(vertex, 1.0f);
			const glm::vec3 pVertPos = pVertRes - mOffset.PointOffset;

			translated_vertices[a] = pVertPos;

			if (mOffset.VertexMap.find(pVertPos) == mOffset.VertexMap.end())
			{
				mOffset.VertexMap.insert(std::make_pair(pVertPos, mOffset.Vertex));
				mOffset.Vertex++;

				const std::string output_str = "\nv " + String::FloatVecToString(&pVertPos.x, 3);
				file.write(output_str.c_str(), output_str.size());
			}
		}

		if (ConvertSettings::ExportUvs)
		{
			for (std::size_t a = 0; a < this->uvs.size(); a++)
			{
				const glm::vec2& uv = this->uvs[a];

				if (mOffset.UvMap.find(uv) == mOffset.UvMap.end())
				{
					mOffset.UvMap.insert(std::make_pair(uv, mOffset.Texture));
					mOffset.Texture++;

					const std::string output_str = "\nvt " + String::FloatVecToString(&uv.x, 2);
					file.write(output_str.c_str(), output_str.size());
				}
			}
		}

		if (ConvertSettings::ExportNormals)
		{
			const glm::mat4 rot_matrix(model_mat[0], model_mat[1], model_mat[2], glm::highp_vec4(0.0f));

			for (std::size_t a = 0; a < this->normals.size(); a++)
			{
				const glm::vec3& normal = this->normals[a];
				const glm::vec3 pNormal = rot_matrix * glm::vec4(normal, 1.0f);

				translated_normals[a] = pNormal;

				if (mOffset.NormalMap.find(pNormal) == mOffset.NormalMap.end())
				{
					mOffset.NormalMap.insert(std::make_pair(pNormal, mOffset.Normal));
					mOffset.Normal++;

					const std::string output_str = "\nvn " + String::FloatVecToString(&pNormal.x, 3);
					file.write(output_str.c_str(), output_str.size());
				}
			}
		}

		for (std::size_t mIdx = 0; mIdx < this->subMeshData.size(); mIdx++)
		{
			const SubMeshData* pSubMesh = this->subMeshData[mIdx];

			if (pEntity != nullptr && ConvertSettings::ApplyTextures)
			{
				const std::string mtl_name = "\nusemtl " + pEntity->GetMtlName(pSubMesh->MaterialName, mIdx);
				file.write(mtl_name.c_str(), mtl_name.size());
			}

			for (std::size_t a = 0; a < pSubMesh->DataIdx.size(); a++)
			{
				std::string _f_str = "\nf";

				const std::vector<VertexData>& vert_vec = pSubMesh->DataIdx[a];
				for (std::size_t b = 0; b < vert_vec.size(); b++)
				{
					const VertexData& d_idx = vert_vec[b];

					{
						const glm::vec3& pVertex = translated_vertices[d_idx.m_Vert];
						const OffsetData::Vec3StorageMap::iterator it = mOffset.VertexMap.find(pVertex);
						if (it != mOffset.VertexMap.end())
							String::Combine(_f_str, " ", (it->second + 1));
					}

					if (!pSubMesh->has_uvs && !pSubMesh->has_normals) continue;

					_f_str.append("/");

					if (pSubMesh->has_uvs)
					{
						const glm::vec2& pUv = this->uvs[d_idx.m_Uv];
						const OffsetData::Vec2StorageMap::iterator it = mOffset.UvMap.find(pUv);
						if (it != mOffset.UvMap.end())
							String::Combine(_f_str, (it->second + 1));
					}

					if (pSubMesh->has_normals)
					{
						const glm::vec3& pNormal = translated_normals[d_idx.m_Norm];
						const OffsetData::Vec3StorageMap::iterator it = mOffset.NormalMap.find(pNormal);
						if (it != mOffset.NormalMap.end())
							String::Combine(_f_str, "/", (it->second + 1));
					}
				}

				file.write(_f_str.c_str(), _f_str.size());
			}
		}
	}

	glm::vec3 Model::GetCenterPoint() const
	{
		return mCenterPoint;
	}

	bool Model::IsEmpty()
	{
		return (this->subMeshData.size() <= 0 || (this->vertices.size() <= 0 && this->uvs.size() <= 0 && this->normals.size() <= 0));
	}
}