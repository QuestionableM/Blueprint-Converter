#include "ObjectDefinitions.h"
#include "Blueprint Converter/Blueprint Writer/BlueprintWriter.h"
#include "Blueprint Converter/Convert Settings/ConvertSettings.h"

#include "Lib/String/String.h"
#include "Lib/OtherFunc/OtherFunc.h"

#include "DebugCon.h"

namespace SMBC
{
	bool SubMeshData::is_empty()
	{
		return (this->DataIdx.size() <= 0);
	}

	bool Model::is_empty()
	{
		return (this->subMeshData.size() <= 0 || (this->vertices.size() <= 0 && this->uvs.size() <= 0 && this->normals.size() <= 0));
	}

	Model::~Model()
	{
		for (SMBC::SubMeshData*& data_ptr : this->subMeshData)
			delete data_ptr;
	}

	const glm::mat4 Object::RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	ObjectType Part::Type() const
	{
		return ObjectType::Part;
	}

	void Part::CalcCenterPoint(OffsetData& data) const
	{
		if (!modelPtr) return;

		for (const glm::vec3& vert : modelPtr->vertices)
		{
			glm::vec3 rotVert = BPFunction::GetPartRotation(vert, this->Bounds, this->xAxis, this->zAxis) + this->Position;
			rotVert = glm::vec4(rotVert, 1.0f) * this->RotationMatrix;

			data.UpdateValues(rotVert);
		}
	}

	void Part::WriteToFile(std::ofstream& out, OffsetData& data, const std::size_t& idx, const glm::vec3& offsetVec) const
	{
		if (!modelPtr) return;

		if (ConvertSettings::SeparationMethod == Sep_Blocks)
		{
			std::string obj_label = "o ";
			String::Combine(obj_label, "Part_", idx, "\n");

			out.write(obj_label.c_str(), obj_label.size());
		}

		for (const glm::vec3& vert : modelPtr->vertices)
		{
			glm::vec3 rotVert = BPFunction::GetPartRotation(vert, this->Bounds, this->xAxis, this->zAxis) + this->Position;
			rotVert = glm::vec4(rotVert, 1.0f) * this->RotationMatrix;

			std::string _vert_str = "v ";
			String::Combine(_vert_str, Other::VecToString(rotVert - offsetVec), "\n");

			out.write(_vert_str.c_str(), _vert_str.size());
		}

		if (ConvertSettings::ExportUvs)
		{
			for (const glm::vec2& uv : modelPtr->uvs)
			{
				std::string _uv_str = "vt ";
				String::Combine(_uv_str, uv.x, " ", uv.y, "\n");

				out.write(_uv_str.c_str(), _uv_str.size());
			}
		}

		if (ConvertSettings::ExportNormals)
		{
			for (const glm::vec3& norm : modelPtr->normals)
			{
				glm::vec3 rotNorm = BPFunction::GetPartRotation(norm, glm::vec3(0.0f), this->xAxis, this->zAxis);
				rotNorm = glm::vec4(rotNorm, 1.0f) * this->RotationMatrix;

				std::string _norm_str = "vn ";
				String::Combine(_norm_str, Other::VecToString(rotNorm), "\n");

				out.write(_norm_str.c_str(), _norm_str.size());
			}
		}

		for (std::size_t subDataIdx = 0; subDataIdx < modelPtr->subMeshData.size(); subDataIdx++)
		{
			SMBC::SubMeshData*& subData = modelPtr->subMeshData[subDataIdx];

			if (ConvertSettings::ApplyTextures)
			{
				std::string _mat_name = "usemtl ";
				String::Combine(_mat_name, objPtr->Uuid.ToString());

				if (ConvertSettings::MatByColor)
					String::Combine(_mat_name, " ", this->Color);

				String::Combine(_mat_name, " ", subDataIdx, "\n");

				out.write(_mat_name.c_str(), _mat_name.size());
			}

			for (std::size_t a = 0; a < subData->DataIdx.size(); a++)
			{
				std::string _f_str = "f";

				for (std::vector<long long>& d_idx : subData->DataIdx[a])
				{
					bool has_uv = (d_idx[1] > -1) && ConvertSettings::ExportUvs;
					bool has_normal = (d_idx[2] > -1) && ConvertSettings::ExportNormals;

					String::Combine(_f_str, " ", d_idx[0] + data.Vertex + 1);

					if (!has_uv && !has_normal) continue;

					_f_str.append("/");

					if (has_uv)
						_f_str.append(std::to_string(d_idx[1] + data.Texture + 1));

					if (has_normal)
						String::Combine(_f_str, "/", d_idx[2] + data.Normal + 1);
				}

				_f_str.append("\n");
				out.write(_f_str.c_str(), _f_str.size());
			}
		}

		data.Vertex  += modelPtr->vertices.size();
		data.Texture += modelPtr->uvs.size();
		data.Normal  += modelPtr->normals.size();
	}

	ObjectType Block::Type() const
	{
		return ObjectType::Block;
	}

	void Block::CalcCenterPoint(OffsetData& data) const
	{
		const int tiling_val = (blkPtr != nullptr) ? blkPtr->Tiling : 4;

		CubeMesh cMesh(this->Bounds / 2.0f, this->Position, tiling_val);

		for (const glm::vec3& vert : cMesh.Vertices)
		{
			glm::vec3 rotVert = BPFunction::GetPartRotation(vert, this->Bounds, this->xAxis, this->zAxis) + this->Position;
			rotVert = glm::vec4(rotVert, 1.0f) * this->RotationMatrix;

			data.UpdateValues(rotVert);
		}
	}

	void Block::WriteToFile(std::ofstream& out, OffsetData& data, const std::size_t& idx, const glm::vec3& offsetVec) const
	{
		if (ConvertSettings::SeparationMethod == Sep_Blocks)
		{
			std::string obj_label = "o ";
			String::Combine(obj_label, "Block_", idx, "\n");

			out.write(obj_label.c_str(), obj_label.size());
		}

		const int cBounds = (blkPtr != nullptr) ? blkPtr->Tiling : 4;
		SMBC::CubeMesh cCube(this->Bounds / 2.0f, this->Position, cBounds);

		for (const glm::vec3& vert : cCube.Vertices)
		{
			glm::vec3 rotVert = BPFunction::GetPartRotation(vert, this->Bounds, this->xAxis, this->zAxis) + this->Position;
			rotVert = glm::vec4(rotVert, 1.0f) * this->RotationMatrix;

			std::string _vert_str = "v ";
			String::Combine(_vert_str, Other::VecToString(rotVert - offsetVec), "\n");

			out.write(_vert_str.c_str(), _vert_str.size());
		}

		if (ConvertSettings::ExportUvs)
		{
			for (const glm::vec2& uv : cCube.TexturePoints)
			{
				std::string _uv_str = "vt ";
				String::Combine(_uv_str, uv.x, " ", uv.y, "\n");

				out.write(_uv_str.c_str(), _uv_str.size());
			}
		}

		if (ConvertSettings::ExportNormals)
		{
			for (const glm::vec3& norm : cCube.Normals)
			{
				glm::vec3 rotNorm = BPFunction::GetPartRotation(norm, glm::vec3(0.0f), this->xAxis, this->zAxis);
				rotNorm = glm::vec4(rotNorm, 1.0f) * this->RotationMatrix;

				std::string _norm_str = "vn ";
				String::Combine(_norm_str, Other::VecToString(rotNorm), "\n");

				out.write(_norm_str.c_str(), _norm_str.size());
			}
		}

		if (ConvertSettings::ApplyTextures)
		{
			std::string _mat_name = "usemtl ";
			String::Combine(_mat_name, this->Uuid.ToString());

			if (ConvertSettings::MatByColor)
				String::Combine(_mat_name, " ", this->Color);

			_mat_name.append("\n");

			out.write(_mat_name.c_str(), _mat_name.size());
		}

		for (std::size_t a = 0; a < cCube.DataIndexes.size(); a++)
		{
			std::string _f_str = "f";

			for (std::vector<uint32_t>& d_idx : cCube.DataIndexes[a])
			{
				String::Combine(_f_str, " ", d_idx[0] + data.Vertex + 1);

				if (!ConvertSettings::ExportNormals && !ConvertSettings::ExportUvs) continue;

				_f_str.append("/");

				if (ConvertSettings::ExportUvs)
					_f_str.append(std::to_string(d_idx[1] + data.Texture + 1));

				if (ConvertSettings::ExportNormals)
					String::Combine(_f_str, "/", d_idx[2] + data.Normal + 1);
			}

			_f_str.append("\n");
			out.write(_f_str.c_str(), _f_str.size());
		}

		data.Vertex  += cCube.Vertices.size();
		data.Texture += cCube.TexturePoints.size();
		data.Normal  += cCube.Normals.size();
	}

	CubeMesh::CubeMesh(const glm::vec3& bounds, const glm::vec3& pos, const int& tiling)
	{
		this->Vertices = {
			{-bounds.x, bounds.y, bounds.z},
			{-bounds.x, -bounds.y, -bounds.z},
			{-bounds.x, -bounds.y, bounds.z},
			{-bounds.x, bounds.y, -bounds.z},
			{bounds.x, -bounds.y, -bounds.z},
			{bounds.x, bounds.y, -bounds.z},
			{bounds.x, -bounds.y, bounds.z},
			{bounds.x, bounds.y, bounds.z}
		};

		float f_tiling = (float)tiling;

		for (std::size_t a = 0; a < this->TexturePoints.size(); a++)
			this->TexturePoints[a] /= f_tiling;

		glm::vec3 _MulVec = bounds * 2.0f;

		glm::vec2 _PosOff(pos.x / f_tiling + _MulVec.x, pos.y / f_tiling + _MulVec.y);
		(this->TexturePoints[21] *= glm::vec2(0.0f, _MulVec.y)) += _PosOff;
		(this->TexturePoints[11] *= glm::vec2(_MulVec.x, 0.0f)) += _PosOff;
		(this->TexturePoints[9] *= glm::vec2(_MulVec.x, _MulVec.y)) += _PosOff;
		(this->TexturePoints[10] += _PosOff);

		float tile_offset2 = (pos.x + _MulVec.x) / f_tiling * (f_tiling - 1.0f);

		glm::vec2 _Off1(tile_offset2, pos.z / f_tiling + _MulVec.z);
		//2.0f, 0.0f
		(this->TexturePoints[15] *= glm::vec2(_MulVec.x, 0.0f)) += _Off1;
		(this->TexturePoints[16] *= glm::vec2(0.0f, _MulVec.z)) += _Off1;
		(this->TexturePoints[23] *= glm::vec2(_MulVec.x, _MulVec.z)) += _Off1;
		this->TexturePoints[17] += _Off1;

		glm::vec2 _Off2(pos.x / f_tiling + _MulVec.x, pos.z / f_tiling + _MulVec.z);
		//0.0f, 2.0f
		(this->TexturePoints[12] *= glm::vec2(_MulVec.x, 0.0f)) += _Off2;
		(this->TexturePoints[13] *= glm::vec2(0.0f, _MulVec.z)) += _Off2;
		(this->TexturePoints[22] *= glm::vec2(_MulVec.x, _MulVec.z)) += _Off2;
		this->TexturePoints[14] += _Off2;

		glm::vec2 _Off3(tile_offset2, pos.y / f_tiling + _MulVec.y);
		//2.0f, 2.0f
		(this->TexturePoints[5] *= glm::vec2(_MulVec.x, 0.0f)) += _Off3;
		(this->TexturePoints[19] *= glm::vec2(0.0f, _MulVec.y)) += _Off3;
		(this->TexturePoints[3] *= glm::vec2(_MulVec.x, _MulVec.y)) += _Off3;
		this->TexturePoints[4] += _Off3;

		glm::vec2 _Off4(
			(pos.y + _MulVec.y) / f_tiling * (f_tiling - 1.0f),
			pos.z / f_tiling + _MulVec.z
		);
		//4.0f, 2.0f
		(this->TexturePoints[1] *= glm::vec2(_MulVec.y, 0.0f)) += _Off4;
		(this->TexturePoints[0] *= glm::vec2(0.0f, _MulVec.z)) += _Off4;
		(this->TexturePoints[2] *= glm::vec2(_MulVec.y, _MulVec.z)) += _Off4;
		this->TexturePoints[18] += _Off4;

		glm::vec2 _Off5(pos.y / f_tiling + _MulVec.y, pos.z / f_tiling + _MulVec.z);
		(this->TexturePoints[6] *= glm::vec2(_MulVec.y, 0.0f)) += _Off5;
		(this->TexturePoints[7] *= glm::vec2(0.0f, _MulVec.z)) += _Off5;
		(this->TexturePoints[20] *= glm::vec2(_MulVec.y, _MulVec.z)) += _Off5;
		this->TexturePoints[8] += _Off5;
	}
}