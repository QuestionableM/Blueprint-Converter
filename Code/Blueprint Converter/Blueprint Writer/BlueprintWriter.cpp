#include <filesystem>
#include <fstream>

#include "BlueprintWriter.h"
#include "Blueprint Converter/BlueprintConverter.h"
#include "Lib/Functions/Functions.h"
#include "Lib/OtherFunc/OtherFunc.h"

namespace fs = std::filesystem;

SMBC::ConvertedModel::ConvertedModel(SMBC::ConvertedModel::ConvertedModelData& cm_data) {
	this->conv_data.apply_texture = cm_data.apply_texture;
	this->conv_data.export_normals = cm_data.export_normals;
	this->conv_data.export_uvs = cm_data.export_uvs;
	this->conv_data.separate_parts = cm_data.separate_parts;
	this->conv_data.tex_list = cm_data.tex_list;
}

void SMBC::ConvertedModel::LoadBlueprintBlocks(
	std::vector<SMBC::ModelDataChunk>& ModelDataChunks,
	SMBC::ModelDataChunk& NewDataChunk,
	SMBC::SubMeshCache& NewSubMeshCache,
	SMBC::ConvertedModel::OffsetData& o_data
) {
	long long ModelFaceIdxOffset = 0ll;
	long long ModelTextureOffset = 0ll;
	long long ModelNormalOffset = 0ll;

	SMBC::BlueprintConversionData::SetNewStage(3, (uint32_t)this->BlockList.size());
	for (uint32_t block_idx = 0u; block_idx < this->BlockList.size(); block_idx++) {
		ModelFaceIdxOffset = o_data.Vertex;
		ModelTextureOffset = o_data.Texture;
		ModelNormalOffset = o_data.Normal;

		SMBC::SM_Block& Block = this->BlockList[block_idx];
		this->CacheManager.LoadBlockIntoCache(Block);

		SMBC::CubeMesh _Cube(Block.bounds / 2.0f, Block.position, Block.tiling);

		o_data.Vertex += (long long)_Cube.Vertices.size();
		NewDataChunk.vertices.reserve(_Cube.Vertices.size());
		for (uint32_t a = 0u; a < _Cube.Vertices.size(); a++) {
			glm::vec3 _RotPt = SMBC::BPFunction::GetPartRotation(_Cube.Vertices[a], Block.bounds, Block.xAxis, Block.zAxis) + Block.position;
			NewDataChunk.vertices.push_back(_RotPt);
		}

		if (conv_data.export_uvs && !_Cube.TexturePoints.empty()) {
			o_data.Texture += (long long)_Cube.TexturePoints.size();
			NewDataChunk.uvs.reserve(_Cube.TexturePoints.size());
			NewDataChunk.uvs.insert(NewDataChunk.uvs.end(), _Cube.TexturePoints.begin(), _Cube.TexturePoints.end());
		}

		if (conv_data.export_normals && !_Cube.Normals.empty()) {
			o_data.Normal += (long long)_Cube.Normals.size();
			NewDataChunk.normals.reserve(_Cube.Normals.size());
			for (uint32_t a = 0u; a < _Cube.Normals.size(); a++) {
				glm::vec3 _RotPt = SMBC::BPFunction::GetPartRotation(_Cube.Normals[a], glm::vec3(0.0f), Block.xAxis, Block.zAxis);

				NewDataChunk.normals.push_back(_RotPt);
			}
		}

		NewSubMeshCache._MeshName = Block.uuid;
		NewSubMeshCache._TexPath = Block.tex_list.dif;

		NewSubMeshCache._DataIdx.reserve(_Cube.DataIndexes.size());
		for (uint32_t a = 0u; a < _Cube.DataIndexes.size(); a++) {
			std::vector<std::vector<long long>> FaceData = {};

			FaceData.reserve(_Cube.DataIndexes[a].size());
			for (uint32_t b = 0u; b < _Cube.DataIndexes[a].size(); b++) {
				std::vector<uint32_t>& D_Idx = _Cube.DataIndexes[a][b];

				FaceData.push_back({
					(long long)D_Idx[0] + ModelFaceIdxOffset,
					(conv_data.export_uvs ? (long long)D_Idx[1] + ModelTextureOffset : -1ll),
					(conv_data.export_normals ? (long long)D_Idx[2] + ModelNormalOffset : -1ll)
					});
			}

			NewSubMeshCache._DataIdx.push_back(FaceData);
		}

		if (conv_data.apply_texture) {
			NewDataChunk.SubMeshCache.push_back(NewSubMeshCache);
			NewSubMeshCache = SMBC::SubMeshCache();
		}

		if (conv_data.separate_parts) {
			NewDataChunk.name.append(L"Block_");
			NewDataChunk.name.append(std::to_wstring(block_idx + 1));

			ModelDataChunks.push_back(NewDataChunk);
			NewDataChunk = SMBC::ModelDataChunk();
		}

		SMBC::BlueprintConversionData::ProgressBarValue++;
	}
}

void SMBC::ConvertedModel::LoadBlueprintParts(
	std::vector<SMBC::ModelDataChunk>& ModelDataChunks,
	SMBC::ModelDataChunk& NewDataChunk,
	SMBC::SubMeshCache& NewSubMeshCache,
	SMBC::ConvertedModel::OffsetData& o_data
) {
	long long ModelFaceIdxOffset = 0ll;
	long long ModelTextureOffset = 0ll;
	long long ModelNormalOffset = 0ll;

	SMBC::BlueprintConversionData::SetNewStage(4, (uint32_t)this->PartList.size());
	for (uint32_t part_idx = 0u; part_idx < this->PartList.size(); part_idx++) {
		ModelFaceIdxOffset = o_data.Vertex;
		ModelTextureOffset = o_data.Texture;
		ModelNormalOffset = o_data.Normal;

		SMBC::SM_Part& Part = this->PartList[part_idx];

		SMBC::CachedMesh _CachedMesh;

		if (!this->CacheManager.GetCachedMesh(Part.uuid, _CachedMesh))
			this->CacheManager.LoadPartIntoCache(Part, _CachedMesh, conv_data.export_uvs, conv_data.export_normals);

		o_data.Vertex += (long long)_CachedMesh.Vertices.size();
		NewDataChunk.vertices.reserve(_CachedMesh.Vertices.size());
		for (uint32_t a = 0u; a < _CachedMesh.Vertices.size(); a++) {
			glm::vec3 _RotPt = SMBC::BPFunction::GetPartRotation(_CachedMesh.Vertices[a], Part.bounds, Part.xAxis, Part.zAxis) + Part.position;

			NewDataChunk.vertices.push_back(_RotPt);
		}

		if (conv_data.export_uvs && !_CachedMesh.TexturePoints.empty()) {
			o_data.Texture += (long long)_CachedMesh.TexturePoints.size();
			NewDataChunk.uvs.reserve(_CachedMesh.TexturePoints.size());
			NewDataChunk.uvs.insert(NewDataChunk.uvs.end(), _CachedMesh.TexturePoints.begin(), _CachedMesh.TexturePoints.end());
		}

		if (conv_data.export_normals && !_CachedMesh.Normals.empty()) {
			o_data.Normal += (long long)_CachedMesh.Normals.size();
			NewDataChunk.normals.reserve(_CachedMesh.Normals.size());
			for (uint32_t a = 0u; a < _CachedMesh.Normals.size(); a++) {
				glm::vec3 _RotNor = SMBC::BPFunction::GetPartRotation(_CachedMesh.Normals[a], glm::vec3(0.0f), Part.xAxis, Part.zAxis);

				NewDataChunk.normals.push_back(_RotNor);
			}
		}

		if (conv_data.apply_texture) NewDataChunk.SubMeshCache.reserve(_CachedMesh.SubMeshCache.size());

		for (uint32_t a = 0u; a < _CachedMesh.SubMeshCache.size(); a++) {
			SMBC::SubMeshCache& _SMCache = _CachedMesh.SubMeshCache[a];

			NewSubMeshCache._DataIdx.reserve(_SMCache._DataIdx.size());
			for (uint32_t b = 0u; b < _SMCache._DataIdx.size(); b++) {
				std::vector<std::vector<long long>> FaceIdx = {};

				FaceIdx.reserve(_SMCache._DataIdx[b].size());
				for (uint32_t c = 0u; c < _SMCache._DataIdx[b].size(); c++) {
					std::vector<long long>& F_Idx = _SMCache._DataIdx[b][c];

					FaceIdx.push_back({
						F_Idx[0] + ModelFaceIdxOffset,
						(F_Idx[1] > -1) ? (F_Idx[1] + ModelTextureOffset) : -1ll,
						(F_Idx[2] > -1) ? (F_Idx[2] + ModelNormalOffset) : -1ll
						});
				}

				NewSubMeshCache._DataIdx.push_back(FaceIdx);
			}

			if (conv_data.apply_texture) {
				NewSubMeshCache._MeshName = _SMCache._MeshName;
				NewSubMeshCache._TexPath = _SMCache._TexPath;

				NewDataChunk.SubMeshCache.push_back(NewSubMeshCache);
				NewSubMeshCache = SMBC::SubMeshCache();
			}
		}

		if (conv_data.separate_parts) {
			NewDataChunk.name.append(L"Part_");
			NewDataChunk.name.append(std::to_wstring(part_idx + 1));

			ModelDataChunks.push_back(NewDataChunk);
			NewDataChunk = SMBC::ModelDataChunk();
		}

		SMBC::BlueprintConversionData::ProgressBarValue++;
	}
}

bool SMBC::ConvertedModel::WriteChunksToFile(
	std::vector<SMBC::ModelDataChunk>& chunks
) {
	fs::create_directory(L"./Converted Models");
	std::wstring _ModelPath = L"./Converted Models/" + this->ModelName;
	std::ofstream _writer;

	if (conv_data.tex_list || conv_data.apply_texture) {
		fs::create_directory(_ModelPath);
		std::wstring _path = _ModelPath + L"/" + this->ModelName;
		_writer.open(_path + L".obj");

		if (conv_data.tex_list) this->CacheManager.WriteTexturePaths(_ModelPath + L"/Used Textures.json");
		if (conv_data.apply_texture) this->CacheManager.WriteMtlFile(_path + L".mtl");
	}
	else _writer.open(_ModelPath + L".obj");

	this->CacheManager.ClearCachedMeshList();

	if (!_writer.is_open()) return false;

	if (conv_data.apply_texture) {
		std::string _mtl_lib = "mtllib ";
		_mtl_lib.append(SMBC::Other::WideToUtf8(this->ModelName));
		_mtl_lib.append(".mtl\n");

		_writer.write(_mtl_lib.c_str(), _mtl_lib.length());
	}

	for (uint32_t a = 0u; a < chunks.size(); a++) {
		SMBC::ModelDataChunk& DChunk = chunks[a];

		if (conv_data.apply_texture) {
			std::string _c_name = "o ";
			_c_name.append(SMBC::Other::WideToUtf8(DChunk.name));
			_c_name.append("\n");

			_writer.write(_c_name.c_str(), _c_name.length());
		}

		SMBC::BlueprintConversionData::SetNewStage(5, (uint32_t)DChunk.vertices.size());
		for (uint32_t b = 0u; b < DChunk.vertices.size(); b++) {
			std::string _v_str = "v ";
			_v_str.append(SMBC::Other::VecToString(DChunk.vertices[b]));
			_v_str.append("\n");

			_writer.write(_v_str.c_str(), _v_str.length());
			SMBC::BlueprintConversionData::ProgressBarValue++;
		}
		DChunk.vertices.clear();

		if (conv_data.export_uvs) {
			SMBC::BlueprintConversionData::SetNewStage(8, (uint32_t)DChunk.uvs.size());
			for (uint32_t b = 0u; b < DChunk.uvs.size(); b++) {
				glm::vec2& _uv = DChunk.uvs[b];

				std::string _uv_str = "vt ";
				_uv_str.append(std::to_string(_uv.x));
				_uv_str.append(" ");
				_uv_str.append(std::to_string(_uv.y));
				_uv_str.append("\n");

				_writer.write(_uv_str.c_str(), _uv_str.length());
				SMBC::BlueprintConversionData::ProgressBarValue++;
			}
			DChunk.uvs.clear();
		}

		if (conv_data.export_normals) {
			SMBC::BlueprintConversionData::SetNewStage(9, (uint32_t)DChunk.normals.size());
			for (uint32_t b = 0u; b < DChunk.normals.size(); b++) {
				std::string _n_str = "vn ";
				_n_str.append(SMBC::Other::VecToString(DChunk.normals[b]));
				_n_str.append("\n");

				_writer.write(_n_str.c_str(), _n_str.length());
				SMBC::BlueprintConversionData::ProgressBarValue++;
			}
			DChunk.normals.clear();
		}

		SMBC::BlueprintConversionData::SetNewStage(6, (uint32_t)DChunk.SubMeshCache.size());
		for (uint32_t b = 0u; b < DChunk.SubMeshCache.size(); b++) {
			SMBC::SubMeshCache& _SubCache = DChunk.SubMeshCache[b];

			if (conv_data.apply_texture) {
				std::string _u_mtl = "usemtl ";
				_u_mtl.append(SMBC::Other::WideToUtf8(_SubCache._MeshName));
				_u_mtl.append("\n");

				_writer.write(_u_mtl.c_str(), _u_mtl.length());
			}

			for (uint32_t c = 0u; c < _SubCache._DataIdx.size(); c++) {
				std::string _f_output = "f";

				for (uint32_t d = 0u; d < _SubCache._DataIdx[c].size(); d++) {
					std::vector<long long>& D_Idx = _SubCache._DataIdx[c][d];

					bool _HasUv = (D_Idx[1] > -1);
					bool _HasNormal = (D_Idx[2] > -1);

					_f_output.append(" ");
					_f_output.append(std::to_string(D_Idx[0] + 1));

					if (!_HasUv && !_HasNormal) continue;

					_f_output.append("/");

					if (_HasUv) _f_output.append(std::to_string(D_Idx[1] + 1));
					if (_HasNormal) {
						_f_output.append("/");
						_f_output.append(std::to_string(D_Idx[2] + 1));
					}

					D_Idx.clear();
				}

				_f_output.append("\n");
				_writer.write(_f_output.c_str(), _f_output.length());

				_SubCache._DataIdx[c].clear();
			}
			SMBC::BlueprintConversionData::ProgressBarValue++;
		}
		DChunk.SubMeshCache.clear();
	}

	chunks.clear();
	return true;
}

#include <assimp/postprocess.h>
#include <assimp/scene.h>

int SMBC::ConvertedModel::ConvertAndWrite() {
	if (this->PartList.size() <= 0 && this->BlockList.size() <= 0) return 3;

	SMBC::ConvertedModel::OffsetData offset_data;

	std::vector<SMBC::ModelDataChunk> ModelDataChunks = {};

	SMBC::ModelDataChunk NewDataChunk;
	SMBC::SubMeshCache NewSubMeshCache;

	this->LoadBlueprintBlocks(ModelDataChunks, NewDataChunk, NewSubMeshCache, offset_data);
	this->BlockList.clear();

	this->LoadBlueprintParts(ModelDataChunks, NewDataChunk, NewSubMeshCache, offset_data);
	this->PartList.clear();

	if (!conv_data.separate_parts) {
		NewDataChunk.SubMeshCache.push_back(NewSubMeshCache);
		ModelDataChunks.push_back(NewDataChunk);
	}

	if (!this->WriteChunksToFile(ModelDataChunks))
		return 2;

	return 0;
}