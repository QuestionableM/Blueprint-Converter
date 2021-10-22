#include "Model.h"

namespace SMBC
{
	bool SubMeshData::IsEmpty()
	{
		return (this->DataIdx.size() <= 0);
	}

	bool Model::IsEmpty()
	{
		return (this->subMeshData.size() <= 0 || (this->vertices.size() <= 0 && this->uvs.size() <= 0 && this->normals.size() <= 0));
	}

	Model::~Model()
	{
		for (SMBC::SubMeshData*& data_ptr : this->subMeshData)
			delete data_ptr;
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