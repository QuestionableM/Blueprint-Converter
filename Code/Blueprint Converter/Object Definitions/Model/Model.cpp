#include "Model.h"

#include "Blueprint Converter/Convert Settings/ConvertSettings.h"
#include "Blueprint Converter/OffsetData/OffsetData.h"
#include "Blueprint Converter/Object Definitions/Entity/Entity.h"

#include "Lib/String/String.h"
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



	Model::Model(const long long& written_uv_idx, const long long& written_normal_idx)
	{
		this->WrittenUvIdx = written_uv_idx;
		this->WrittenNormalIdx = written_normal_idx;
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



	void Model::WriteToFile(std::ofstream& file, const glm::mat4& model_mat, OffsetData& mOffset, const class Entity* pEntity)
	{
		for (std::size_t a = 0; a < this->vertices.size(); a++)
		{
			const glm::vec3& vertex = this->vertices[a];
			const glm::vec3 pVertPos = model_mat * glm::vec4(vertex, 1.0f);
			const std::string output_str = "\nv " + String::FloatVecToString(&pVertPos.x, 3);

			file.write(output_str.c_str(), output_str.size());
		}

		if (ConvertSettings::ExportUvs && this->WrittenUvIdx == -1)
		{
			DebugOutL("Writing uvs for (", mOffset.Texture, ") -> ", this->meshPath);
			this->WrittenUvIdx = mOffset.Texture;

			for (std::size_t a = 0; a < this->uvs.size(); a++)
			{
				const glm::vec2& uv = this->uvs[a];
				const std::string output_str = "\nvt " + String::FloatVecToString(&uv.x, 2);

				file.write(output_str.c_str(), output_str.size());
			}

			mOffset.Texture += this->uvs.size();
		}

		long long normal_offset = this->WrittenNormalIdx;
		if (ConvertSettings::ExportNormals && this->WrittenNormalIdx == -1)
		{
			DebugOutL("Writing normals for (", mOffset.Normal, ") -> ", this->meshPath);

			normal_offset = mOffset.Normal;
			const glm::mat4 rot_matrix(model_mat[0], model_mat[1], model_mat[2], glm::highp_vec4(0.0f));

			for (std::size_t a = 0; a < this->normals.size(); a++)
			{
				const glm::vec3& normal = this->normals[a];
				const glm::vec3 pNormal = rot_matrix * glm::vec4(normal, 1.0f);
				const std::string output_str = "\nvn " + String::FloatVecToString(&pNormal.x, 3);

				file.write(output_str.c_str(), output_str.size());
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

					String::Combine(_f_str, " ", (d_idx.pVert + mOffset.Vertex + 1));

					const bool has_uv     = (ConvertSettings::ExportUvs		&& d_idx.pUv   > -1);
					const bool has_normal = (ConvertSettings::ExportNormals && d_idx.pNorm > -1);

					if (!has_uv && !has_normal) continue;

					_f_str.append("/");

					if (has_uv)     String::Combine(_f_str,      (d_idx.pUv   + this->WrittenUvIdx     + 1));
					if (has_normal) String::Combine(_f_str, "/", (d_idx.pNorm + normal_offset		   + 1));
				}

				file.write(_f_str.c_str(), _f_str.size());
			}
		}

		mOffset.Vertex += this->vertices.size();

		if (ConvertSettings::ExportNormals && this->WrittenNormalIdx)
			mOffset.Normal += this->normals.size();
	}

	bool Model::IsEmpty()
	{
		return (this->subMeshData.size() <= 0 || (this->vertices.size() <= 0 && this->uvs.size() <= 0 && this->normals.size() <= 0));
	}
}