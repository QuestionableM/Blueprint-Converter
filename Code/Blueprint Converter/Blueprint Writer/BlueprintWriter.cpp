#include <filesystem>
#include <fstream>

#include "BlueprintWriter.h"
#include "Blueprint Converter/BlueprintConverter.h"
#include "Lib/Functions/Functions.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/File/FileFunc.h"

namespace fs = std::filesystem;

bool SMBC::ObjectCollection::is_empty() {
	return (this->BlockList.size() <= 0 && this->PartList.size() <= 0);
}

bool SMBC::ConvertedModel::AddJointToChildShape(SMBC::SM_Part& joint) {
	for (uint32_t col_idx = 0u; col_idx < this->ObjCollection.size(); col_idx++) {
		SMBC::ObjectCollection& _cur_col = this->ObjCollection[col_idx];

		for (uint32_t block_itr = 0u; block_itr < _cur_col.BlockList.size(); block_itr++) {
			SMBC::SM_Block& cur_block = _cur_col.BlockList[block_itr];

			if (cur_block._index == joint._index) {
				this->ObjCollection[col_idx].PartList.push_back(joint);
				return true;
			}
		}

		for (uint32_t part_itr = 0u; part_itr < _cur_col.PartList.size(); part_itr++) {
			SMBC::SM_Part& cur_part = _cur_col.PartList[part_itr];

			if (cur_part._index == joint._index) {
				this->ObjCollection[col_idx].PartList.push_back(joint);
				return true;
			}
		}
	}

	return false;
}

bool SMBC::ConvertedModel::HasStuffToConvert() {
	long long _StuffCounter = 0;
	for (uint32_t a = 0u; a < this->ObjCollection.size(); a++) {
		SMBC::ObjectCollection& _CurCol = this->ObjCollection[a];

		_StuffCounter += (long long)_CurCol.PartList.size();
		_StuffCounter += (long long)_CurCol.BlockList.size();
	}

	return (_StuffCounter > 0);
}

SMBC::ConvertedModel::ConvertedModel(SMBC::ConvertedModel::ConvertedModelData& cm_data) {
	this->conv_data.apply_texture = cm_data.apply_texture;
	this->conv_data.export_normals = cm_data.export_normals;
	this->conv_data.export_uvs = cm_data.export_uvs;
	this->conv_data.separation_method = cm_data.separation_method;
	this->conv_data.tex_list = cm_data.tex_list;
	this->conv_data.mat_by_color = cm_data.mat_by_color;
}

void SMBC::ConvertedModel::LoadBlueprintBlocks(
	std::vector<SMBC::Model>& Models,
	SMBC::Model& NewModel,
	SMBC::SubMeshData& NewSubMeshData,
	SMBC::ConvertedModel::OffsetData& o_data,
	uint32_t& collection_idx
) {
	long long ModelFaceIdxOffset = 0ll;
	long long ModelTextureOffset = 0ll;
	long long ModelNormalOffset = 0ll;

	SMBC::ObjectCollection& _collection = this->ObjCollection[collection_idx];

	bool _separate_parts = (conv_data.separation_method == SMBC_SEPARATION_BLOCKS);

	SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_READING_BLOCKS, (uint32_t)_collection.BlockList.size());
	for (uint32_t block_idx = 0u; block_idx < _collection.BlockList.size(); block_idx++) {
		ModelFaceIdxOffset = o_data.Vertex;
		ModelTextureOffset = o_data.Texture;
		ModelNormalOffset = o_data.Normal;

		SMBC::SM_Block& Block = _collection.BlockList[block_idx];
		this->CacheManager.LoadBlock(Block, conv_data.mat_by_color);

		SMBC::CubeMesh _Cube(Block.bounds / 2.0f, Block.position, Block.tiling);

		o_data.Vertex += (long long)_Cube.Vertices.size();
		NewModel.vertices.reserve(_Cube.Vertices.size());
		for (uint32_t a = 0u; a < _Cube.Vertices.size(); a++) {
			glm::vec3 _RotPt = SMBC::BPFunction::GetPartRotation(_Cube.Vertices[a], Block.bounds, Block.xAxis, Block.zAxis) + Block.position;

			NewModel.vertices.push_back(_RotPt);
		}

		if (conv_data.export_uvs && !_Cube.TexturePoints.empty()) {
			o_data.Texture += (long long)_Cube.TexturePoints.size();
			NewModel.uvs.reserve(_Cube.TexturePoints.size());
			NewModel.uvs.insert(NewModel.uvs.end(), _Cube.TexturePoints.begin(), _Cube.TexturePoints.end());
		}

		if (conv_data.export_normals && !_Cube.Normals.empty()) {
			o_data.Normal += (long long)_Cube.Normals.size();
			NewModel.normals.reserve(_Cube.Normals.size());
			for (uint32_t a = 0u; a < _Cube.Normals.size(); a++) {
				glm::vec3 _RotPt = SMBC::BPFunction::GetPartRotation(_Cube.Normals[a], glm::vec3(0.0f), Block.xAxis, Block.zAxis);

				NewModel.normals.push_back(_RotPt);
			}
		}

		NewSubMeshData.DataIdx.reserve(_Cube.DataIndexes.size());
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

			NewSubMeshData.DataIdx.push_back(FaceData);
		}

		if (conv_data.apply_texture) {
			std::wstring _MatName = Block.uuid;
			if (conv_data.mat_by_color) {
				_MatName.append(L" ");
				_MatName.append(Block.color);
			}
			NewSubMeshData.MaterialName = _MatName;

			NewModel.subMeshData.push_back(NewSubMeshData);
			NewSubMeshData = SMBC::SubMeshData();
		}

		if (_separate_parts) {
			NewModel.meshPath.append(L"Block_");
			NewModel.meshPath.append(std::to_wstring(block_idx + 1));

			if (!conv_data.apply_texture) {
				NewModel.subMeshData.push_back(NewSubMeshData);
				NewSubMeshData = SMBC::SubMeshData();
			}

			Models.push_back(NewModel);
			NewModel = SMBC::Model();
		}

		SMBC::BlueprintConversionData::ProgressBarValue++;
	}
}

void SMBC::ConvertedModel::LoadBlueprintParts(
	std::vector<SMBC::Model>& Models,
	SMBC::Model& NewModel,
	SMBC::SubMeshData& NewSubMeshData,
	SMBC::ConvertedModel::OffsetData& o_data,
	uint32_t& collection_idx
) {
	long long ModelFaceIdxOffset = 0ll;
	long long ModelTextureOffset = 0ll;
	long long ModelNormalOffset = 0ll;

	SMBC::ObjectCollection& _collection = this->ObjCollection[collection_idx];

	bool _separate_parts = (conv_data.separation_method == SMBC_SEPARATION_BLOCKS);

	SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_READING_PARTS, (uint32_t)_collection.PartList.size());
	for (uint32_t part_idx = 0u; part_idx < _collection.PartList.size(); part_idx++) {
		ModelFaceIdxOffset = o_data.Vertex;
		ModelTextureOffset = o_data.Texture;
		ModelNormalOffset = o_data.Normal;

		SMBC::SM_Part& Part = _collection.PartList[part_idx];

		SMBC::CachedPart _CachedPart;
		this->CacheManager.LoadPart(Part, _CachedPart, conv_data.export_uvs, conv_data.export_normals, conv_data.mat_by_color);

		SMBC::Model CachedModel;
		if (!this->CacheManager.LoadModel(Part.object_path, CachedModel, conv_data.export_uvs, conv_data.export_normals))
			continue;

		o_data.Vertex += (long long)CachedModel.vertices.size();
		NewModel.vertices.reserve(CachedModel.vertices.size());
		for (uint32_t a = 0u; a < CachedModel.vertices.size(); a++) {
			glm::vec3 _RotPt = SMBC::BPFunction::GetPartRotation(CachedModel.vertices[a], Part.bounds, Part.xAxis, Part.zAxis) + Part.position;

			NewModel.vertices.push_back(_RotPt);
		}

		if (conv_data.export_uvs && !CachedModel.uvs.empty()) {
			o_data.Texture += (long long)CachedModel.uvs.size();
			NewModel.uvs.reserve(CachedModel.uvs.size());
			NewModel.uvs.insert(NewModel.uvs.end(), CachedModel.uvs.begin(), CachedModel.uvs.end());
		}

		if (conv_data.export_normals && !CachedModel.normals.empty()) {
			o_data.Normal += (long long)CachedModel.normals.size();
			NewModel.normals.reserve(CachedModel.normals.size());
			for (uint32_t a = 0u; a < CachedModel.normals.size(); a++) {
				glm::vec3 _RotNor = SMBC::BPFunction::GetPartRotation(CachedModel.normals[a], glm::vec3(0.0f), Part.xAxis, Part.zAxis);

				NewModel.normals.push_back(_RotNor);
			}
		}

		if (conv_data.apply_texture) NewModel.subMeshData.reserve(CachedModel.subMeshData.size());
		for (uint32_t a = 0u; a < CachedModel.subMeshData.size(); a++) {
			SMBC::SubMeshData& _SMData = CachedModel.subMeshData[a];

			NewSubMeshData.DataIdx.reserve(_SMData.DataIdx.size());
			for (uint32_t b = 0u; b < _SMData.DataIdx.size(); b++) {
				std::vector<std::vector<long long>> FaceIdx = {};

				FaceIdx.reserve(_SMData.DataIdx[b].size());
				for (uint32_t c = 0u; c < _SMData.DataIdx[b].size(); c++) {
					std::vector<long long>& F_Idx = _SMData.DataIdx[b][c];

					bool _HasTextures = (F_Idx[1] > -1) && conv_data.export_uvs;
					bool _HasNormals = (F_Idx[2] > -1) && conv_data.export_normals;

					FaceIdx.push_back({
						F_Idx[0] + ModelFaceIdxOffset,
						(_HasTextures) ? (F_Idx[1] + ModelTextureOffset) : -1ll,
						(_HasNormals) ? (F_Idx[2] + ModelNormalOffset) : -1ll
						});
				}

				NewSubMeshData.DataIdx.push_back(FaceIdx);
			}

			if (conv_data.apply_texture) {
				std::wstring _MatName = Part.uuid;
				if (conv_data.mat_by_color) {
					_MatName.append(L" ");
					_MatName.append(Part.color);
				}
				_MatName.append(L" ");
				_MatName.append(std::to_wstring(_SMData.SubMeshIndex));
				NewSubMeshData.MaterialName = _MatName;

				NewModel.subMeshData.push_back(NewSubMeshData);
				NewSubMeshData = SMBC::SubMeshData();
			}
		}

		if (_separate_parts) {
			NewModel.meshPath.append(L"Part_");
			NewModel.meshPath.append(std::to_wstring(part_idx + 1));

			if (!conv_data.apply_texture) {
				NewModel.subMeshData.push_back(NewSubMeshData);
				NewSubMeshData = SMBC::SubMeshData();
			}

			Models.push_back(NewModel);
			NewModel = SMBC::Model();
		}

		SMBC::BlueprintConversionData::ProgressBarValue++;
	}
}

bool SMBC::ConvertedModel::WriteChunksToFile(
	std::vector<SMBC::Model>& models
) {
	fs::create_directory(L"./Converted Models");
	std::wstring _ModelPath = L"./Converted Models/" + this->ModelName;
	if (SMBC::FILE::IsBadPath(_ModelPath)) return false;

	std::ofstream _writer;

	if (conv_data.tex_list || conv_data.apply_texture) {
		fs::create_directory(_ModelPath);
		std::wstring _path = _ModelPath + L"/" + this->ModelName;
		_writer.open(_path + L".obj");

		if (conv_data.tex_list) this->CacheManager.WriteTexturePaths(_ModelPath + L"/Used Textures.json");
		if (conv_data.apply_texture) this->CacheManager.WriteMtlFile(_path + L".mtl");
	}
	else _writer.open(_ModelPath + L".obj");

	this->CacheManager.ClearCache();

	if (!_writer.is_open()) return false;

	if (conv_data.apply_texture) {
		std::string _mtl_lib = "mtllib ";
		_mtl_lib.append(SMBC::Other::WideToUtf8(this->ModelName));
		_mtl_lib.append(".mtl\n");

		_writer.write(_mtl_lib.c_str(), _mtl_lib.length());
	}

	bool _any_separation = (conv_data.separation_method != 0);

	for (uint32_t a = 0u; a < models.size(); a++) {
		SMBC::Model& DChunk = models[a];

		if (_any_separation) {
			std::string _c_name = "o ";
			_c_name.append(SMBC::Other::WideToUtf8(DChunk.meshPath));
			_c_name.append("\n");

			_writer.write(_c_name.c_str(), _c_name.length());
		}

		SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_WRITING_VERTICES, (uint32_t)DChunk.vertices.size());
		for (uint32_t b = 0u; b < DChunk.vertices.size(); b++) {
			std::string _v_str = "v ";
			_v_str.append(SMBC::Other::VecToString(DChunk.vertices[b]));
			_v_str.append("\n");

			_writer.write(_v_str.c_str(), _v_str.length());
			SMBC::BlueprintConversionData::ProgressBarValue++;
		}

		if (conv_data.export_uvs) {
			SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_WRITING_UVS, (uint32_t)DChunk.uvs.size());
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
		}

		if (conv_data.export_normals) {
			SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_WRITING_NORMALS, (uint32_t)DChunk.normals.size());
			for (uint32_t b = 0u; b < DChunk.normals.size(); b++) {
				std::string _n_str = "vn ";
				_n_str.append(SMBC::Other::VecToString(DChunk.normals[b]));
				_n_str.append("\n");

				_writer.write(_n_str.c_str(), _n_str.length());
				SMBC::BlueprintConversionData::ProgressBarValue++;
			}
		}

		SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_WRITING_FACES, (uint32_t)DChunk.subMeshData.size());
		for (uint32_t b = 0u; b < DChunk.subMeshData.size(); b++) {
			SMBC::SubMeshData& _SubData = DChunk.subMeshData[b];

			if (conv_data.apply_texture) {
				std::string _u_mtl = "usemtl ";
				_u_mtl.append(SMBC::Other::WideToUtf8(_SubData.MaterialName));
				_u_mtl.append("\n");

				_writer.write(_u_mtl.c_str(), _u_mtl.length());
			}

			for (uint32_t c = 0u; c < _SubData.DataIdx.size(); c++) {
				std::string _f_output = "f";

				for (uint32_t d = 0u; d < _SubData.DataIdx[c].size(); d++) {
					std::vector<long long>& D_Idx = _SubData.DataIdx[c][d];

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
			}
			SMBC::BlueprintConversionData::ProgressBarValue++;
		}
	}

	return true;
}

#include <assimp/postprocess.h>
#include <assimp/scene.h>

int SMBC::ConvertedModel::ConvertAndWrite() {
	if (!this->HasStuffToConvert()) return SMBC_ERROR_NO_BLOCKS;

	SMBC::ConvertedModel::OffsetData offset_data;
	std::vector<SMBC::Model> Models = {};
	SMBC::Model NewModel;
	SMBC::SubMeshData NewSubMeshData;

	bool _joint_separation = (conv_data.separation_method == SMBC_SEPARATION_JOINTS);

	for (uint32_t collection_idx = 0u; collection_idx < this->ObjCollection.size(); collection_idx++) {
		this->LoadBlueprintBlocks(Models, NewModel, NewSubMeshData, offset_data, collection_idx);

		this->LoadBlueprintParts(Models, NewModel, NewSubMeshData, offset_data, collection_idx);

		if (_joint_separation) {
			NewModel.meshPath = this->ModelName + L' ' + std::to_wstring(collection_idx);

			NewModel.subMeshData.push_back(NewSubMeshData);
			NewSubMeshData = SMBC::SubMeshData();
			Models.push_back(NewModel);
			NewModel = SMBC::Model();
		}
	}

	if (conv_data.separation_method == 0) {
		NewModel.subMeshData.push_back(NewSubMeshData);
		Models.push_back(NewModel);
	}

	if (!this->WriteChunksToFile(Models))
		return SMBC_ERROR_WRITE;

	return SMBC_ERROR_NONE;
}