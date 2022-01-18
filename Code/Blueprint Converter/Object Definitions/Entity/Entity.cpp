#include "Entity.h"

#include "Blueprint Converter/Convert Settings/ConvertSettings.h"
#include "Blueprint Converter/Blueprint Writer/BlueprintWriter.h"
#include "Blueprint Converter/Object Definitions/Model/Model.h"
#include "Blueprint Converter/BlueprintConverter.h"

#include "Lib/String/String.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/ConvData/ConvData.h"

#include <glm.hpp>
#include <gtx/transform.hpp>

namespace SMBC
{
	glm::mat4 Entity::GetTransformMatrix() const
	{
		glm::mat4 transform(1.0f);

		transform *= glm::translate(this->mPosition);
		transform *= glm::scale(this->mScale);

		return transform;
	}

	std::size_t Entity::GetAmountOfObjects() const
	{
		return 1;
	}

	void Entity::WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const
	{
		const glm::mat4 model_matrix = this->GetTransformMatrix();

		pModel->WriteToFile(file, model_matrix, mOffset, this);

		ConvData::ProgressValue++;
	}

	/*const glm::mat4 Object::RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	void Object::WriteVertices(const std::vector<glm::vec3>& vertices, std::ofstream& out, const glm::vec3& offsetVec) const
	{
		const glm::mat4 obj_matrix = BPFunction::GetPartRotationMatrix(this->xAxis, this->zAxis);
		for (const glm::vec3& vert : vertices)
		{
			glm::vec3 rotVert = BPFunction::GetPartRotationFromMatrix(vert, this->Bounds, obj_matrix) + this->Position;
			rotVert = glm::vec4(rotVert, 1.0f) * this->RotationMatrix;

			std::string _vert_str = "v ";
			String::Combine(_vert_str, Other::VecToString(rotVert - offsetVec), "\n");

			out.write(_vert_str.c_str(), _vert_str.size());
		}
	}

	void Object::WriteUvs(const std::vector<glm::vec2>& uvs, std::ofstream& out)
	{
		if (!ConvertSettings::ExportUvs) return;

		for (const glm::vec2& uv : uvs)
		{
			std::string _uv_str = "vt ";
			String::Combine(_uv_str, uv.x, " ", uv.y, "\n");

			out.write(_uv_str.c_str(), _uv_str.size());
		}
	}

	void Object::WriteNormals(const std::vector<glm::vec3>& normals, std::ofstream& out) const
	{
		if (!ConvertSettings::ExportNormals) return;

		const glm::mat4 obj_matrix = BPFunction::GetPartRotationMatrix(this->xAxis, this->zAxis);
		for (const glm::vec3& norm : normals)
		{
			glm::vec3 rotNorm = BPFunction::GetPartRotationFromMatrix(norm, glm::vec3(0.0f), obj_matrix);
			rotNorm = glm::vec4(rotNorm, 1.0f) * this->RotationMatrix;

			std::string _norm_str = "vn ";
			String::Combine(_norm_str, Other::VecToString(rotNorm), "\n");

			out.write(_norm_str.c_str(), _norm_str.size());
		}
	}

	void Object::WriteTextures(std::ofstream& out, const std::size_t& sub_data_idx, const bool& write_sub_idx) const
	{
		if (!ConvertSettings::ApplyTextures) return;

		std::string _mat_name = "usemtl ";
		String::Combine(_mat_name, this->Uuid.ToString());

		if (ConvertSettings::MatByColor)
			String::Combine(_mat_name, " ", this->Color.StringHex());

		if (write_sub_idx)
			String::Combine(_mat_name, " ", sub_data_idx);

		_mat_name.append("\n");

		out.write(_mat_name.c_str(), _mat_name.size());
	}

	void Object::WriteIndices(const std::vector<std::vector<std::vector<long long>>>& data_idx, OffsetData& oData, std::ofstream& out, const long long& uv_idx)
	{
		for (std::size_t a = 0; a < data_idx.size(); a++)
		{
			std::string _f_str = "f";

			for (const std::vector<long long>& d_idx : data_idx[a])
			{
				bool has_uv = (d_idx[1] > -1) && ConvertSettings::ExportUvs;
				bool has_normal = (d_idx[2] > -1) && ConvertSettings::ExportNormals;

				String::Combine(_f_str, " ", d_idx[0] + oData.Vertex + 1);

				if (!has_uv && !has_normal) continue;

				_f_str.append("/");

				if (has_uv)
					_f_str.append(std::to_string(d_idx[1] + uv_idx + 1));

				if (has_normal)
					String::Combine(_f_str, "/", d_idx[2] + oData.Normal + 1);
			}

			_f_str.append("\n");
			out.write(_f_str.c_str(), _f_str.size());
		}
	}

	void Object::WriteObjectSeparator(std::ofstream& out, const std::string& name, const std::size_t& idx)
	{
		if (ConvertSettings::SeparationMethod != Sep_Blocks) return;

		std::string obj_label = "o ";
		String::Combine(obj_label, name, "_", idx, "\n");

		out.write(obj_label.c_str(), obj_label.size());
	}




	ObjectType Part::Type() const
	{
		return ObjectType::Part;
	}

	void Part::CalcCenterPoint(OffsetData& data) const
	{
		if (!modelPtr) return;

		const glm::mat4 obj_matrix = BPFunction::GetPartRotationMatrix(this->xAxis, this->zAxis);
		for (const glm::vec3& vert : modelPtr->vertices)
		{
			glm::vec3 rotVert = BPFunction::GetPartRotationFromMatrix(vert, this->Bounds, obj_matrix) + this->Position;
			rotVert = glm::vec4(rotVert, 1.0f) * this->RotationMatrix;

			data.UpdateValues(rotVert);
		}
	}

	void Part::WriteToFile(std::ofstream& out, OffsetData& data, const std::size_t& idx, const glm::vec3& offsetVec) const
	{
		if (!modelPtr) return;

		this->WriteObjectSeparator(out, "Part", idx);

		const bool can_write_uvs = (modelPtr->WrittenUvIdx == -1);

		{
			this->WriteVertices(modelPtr->vertices, out, offsetVec);

			if (can_write_uvs)
			{
				this->WriteUvs(modelPtr->uvs, out);
				modelPtr->WrittenUvIdx = (long long)data.Texture;
				DebugOutL("Written uvs for: ", modelPtr->meshPath);
			}

			this->WriteNormals(modelPtr->normals, out);
		}

		for (std::size_t subDataIdx = 0; subDataIdx < modelPtr->subMeshData.size(); subDataIdx++)
		{
			SMBC::SubMeshData*& subData = modelPtr->subMeshData[subDataIdx];

			this->WriteTextures(out, subDataIdx, true);
			this->WriteIndices(subData->DataIdx, data, out, modelPtr->WrittenUvIdx);
		}

		{
			data.Vertex += modelPtr->vertices.size();

			if (can_write_uvs)
			{
				data.Texture += modelPtr->uvs.size();
			}

			data.Normal += modelPtr->normals.size();
		}
	}

	void Part::SetModelPtr(Model* model_ptr)
	{
		this->modelPtr = model_ptr;
	}

	ObjectType Block::Type() const
	{
		return ObjectType::Block;
	}

	void Block::CalcCenterPoint(OffsetData& data) const
	{
		const int tiling_val = (blkPtr != nullptr) ? blkPtr->Tiling : 4;

		CubeMesh cMesh(this->Bounds / 2.0f, this->Position, tiling_val);

		const glm::mat4 obj_matrix = BPFunction::GetPartRotationMatrix(this->xAxis, this->zAxis);
		for (const glm::vec3& vert : cMesh.Vertices)
		{
			glm::vec3 rotVert = BPFunction::GetPartRotationFromMatrix(vert, this->Bounds, obj_matrix) + this->Position;
			rotVert = glm::vec4(rotVert, 1.0f) * this->RotationMatrix;

			data.UpdateValues(rotVert);
		}
	}

	void Block::WriteToFile(std::ofstream& out, OffsetData& data, const std::size_t& idx, const glm::vec3& offsetVec) const
	{
		this->WriteObjectSeparator(out, "Block", idx);

		const int cBounds = (blkPtr != nullptr) ? blkPtr->Tiling : 4;
		SMBC::CubeMesh cCube(this->Bounds / 2.0f, this->Position, cBounds);

		this->WriteVertices(cCube.Vertices, out, offsetVec);
		this->WriteUvs(cCube.TexturePoints, out);
		this->WriteNormals(cCube.Normals, out);

		this->WriteTextures(out, 0, false);
		this->WriteIndices(cCube.DataIndexes, data, out, data.Texture);

		data.Vertex += cCube.Vertices.size();
		data.Texture += cCube.TexturePoints.size();
		data.Normal += cCube.Normals.size();
	}

	void Block::SetModelPtr(Model* model_ptr) {}*/
}