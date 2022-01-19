#include "BlockModel.h"

#include "Blueprint Converter/Convert Settings/ConvertSettings.h"

namespace SMBC
{
	void BlockModel::FillBlockData(Model& pModel, const glm::vec3& bounds)
	{
		pModel.vertices =
		{
			{-bounds.x,  bounds.y,  bounds.z},
			{-bounds.x, -bounds.y, -bounds.z},
			{-bounds.x, -bounds.y,  bounds.z},
			{-bounds.x,  bounds.y, -bounds.z},
			{ bounds.x, -bounds.y, -bounds.z},
			{ bounds.x,  bounds.y, -bounds.z},
			{ bounds.x, -bounds.y,  bounds.z},
			{ bounds.x,  bounds.y,  bounds.z}
		};

		if (ConvertSettings::ExportUvs)
		{
			pModel.uvs =
			{
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
		}

		if (ConvertSettings::ExportNormals)
		{
			pModel.normals =
			{
				{-1.0,  0.0, 0.0}, { 0.0, 0.0, -1.0 },
				{ 1.0,  0.0, 0.0}, { 0.0, 0.0,  1.0 },
				{ 0.0, -1.0, 0.0}, { 0.0, 1.0,  0.0 }
			};
		}

		SubMeshData* new_subMesh = new SubMeshData(0);
		new_subMesh->DataIdx =
		{
			{{0, 0,  0}, {1, 1,  0}, {2, 2,  0}},
			{{3, 3,  1}, {4, 4,  1}, {1, 5,  1}},
			{{5, 6,  2}, {6, 7,  2}, {4, 8,  2}},
			{{7, 9,  3}, {2, 10, 3}, {6, 11, 3}},
			{{4, 12, 4}, {2, 13, 4}, {1, 14, 4}},
			{{3, 15, 5}, {7, 16, 5}, {5, 17, 5}},
			{{0, 0,  0}, {3, 18, 0}, {1, 1,  0}},
			{{3, 3,  1}, {5, 19, 1}, {4, 4,  1}},
			{{5, 6,  2}, {7, 20, 2}, {6, 7,  2}},
			{{7, 9,  3}, {0, 21, 3}, {2, 10, 3}},
			{{4, 12, 4}, {6, 22, 4}, {2, 13, 4}},
			{{3, 15, 5}, {0, 23, 5}, {7, 16, 5}}
		};

		pModel.subMeshData.push_back(new_subMesh);
	}

	void BlockModel::FillBlockUvs(Model& pModel, const glm::vec3& bounds, const glm::vec3& pos, const int& tiling)
	{
		if (!ConvertSettings::ExportUvs) return;

		float f_tiling = (float)tiling;

		for (std::size_t a = 0; a < pModel.uvs.size(); a++)
			pModel.uvs[a] /= f_tiling;

		glm::vec3 _MulVec = bounds * 2.0f;

		glm::vec2 _PosOff(pos.x / f_tiling + _MulVec.x, pos.y / f_tiling + _MulVec.y);
		(pModel.uvs[21] *= glm::vec2(0.0f, _MulVec.y)) += _PosOff;
		(pModel.uvs[11] *= glm::vec2(_MulVec.x, 0.0f)) += _PosOff;
		(pModel.uvs[9] *= glm::vec2(_MulVec.x, _MulVec.y)) += _PosOff;
		(pModel.uvs[10] += _PosOff);

		float tile_offset2 = (pos.x + _MulVec.x) / f_tiling * (f_tiling - 1.0f);

		glm::vec2 _Off1(tile_offset2, pos.z / f_tiling + _MulVec.z);
		//2.0f, 0.0f
		(pModel.uvs[15] *= glm::vec2(_MulVec.x, 0.0f)) += _Off1;
		(pModel.uvs[16] *= glm::vec2(0.0f, _MulVec.z)) += _Off1;
		(pModel.uvs[23] *= glm::vec2(_MulVec.x, _MulVec.z)) += _Off1;
		pModel.uvs[17] += _Off1;

		glm::vec2 _Off2(pos.x / f_tiling + _MulVec.x, pos.z / f_tiling + _MulVec.z);
		//0.0f, 2.0f
		(pModel.uvs[12] *= glm::vec2(_MulVec.x, 0.0f)) += _Off2;
		(pModel.uvs[13] *= glm::vec2(0.0f, _MulVec.z)) += _Off2;
		(pModel.uvs[22] *= glm::vec2(_MulVec.x, _MulVec.z)) += _Off2;
		pModel.uvs[14] += _Off2;

		glm::vec2 _Off3(tile_offset2, pos.y / f_tiling + _MulVec.y);
		//2.0f, 2.0f
		(pModel.uvs[5] *= glm::vec2(_MulVec.x, 0.0f)) += _Off3;
		(pModel.uvs[19] *= glm::vec2(0.0f, _MulVec.y)) += _Off3;
		(pModel.uvs[3] *= glm::vec2(_MulVec.x, _MulVec.y)) += _Off3;
		pModel.uvs[4] += _Off3;

		glm::vec2 _Off4(
			(pos.y + _MulVec.y) / f_tiling * (f_tiling - 1.0f),
			pos.z / f_tiling + _MulVec.z
		);
		//4.0f, 2.0f
		(pModel.uvs[1] *= glm::vec2(_MulVec.y, 0.0f)) += _Off4;
		(pModel.uvs[0] *= glm::vec2(0.0f, _MulVec.z)) += _Off4;
		(pModel.uvs[2] *= glm::vec2(_MulVec.y, _MulVec.z)) += _Off4;
		pModel.uvs[18] += _Off4;

		glm::vec2 _Off5(pos.y / f_tiling + _MulVec.y, pos.z / f_tiling + _MulVec.z);
		(pModel.uvs[6] *= glm::vec2(_MulVec.y, 0.0f)) += _Off5;
		(pModel.uvs[7] *= glm::vec2(0.0f, _MulVec.z)) += _Off5;
		(pModel.uvs[20] *= glm::vec2(_MulVec.y, _MulVec.z)) += _Off5;
		pModel.uvs[8] += _Off5;
	}

	void BlockModel::CreateBlockModel(Model& pModel, const glm::vec3& pos, const glm::vec3& scale, const int& tiling)
	{
		BlockModel::FillBlockData(pModel, scale);
		BlockModel::FillBlockUvs (pModel, scale, pos, tiling);
	}
}