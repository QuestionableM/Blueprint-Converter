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
	for (SMBC::ObjectCollection& col : this->ObjCollection) {
		for (SMBC::SM_Block& cur_block : col.BlockList) {
			if (cur_block._index != joint._index) continue;

			col.PartList.push_back(joint);
			return true;
		}

		for (SMBC::SM_Part& cur_part : col.PartList) {
			if (cur_part._index != joint._index) continue;

			col.PartList.push_back(joint);
			return true;
		}
	}

	return false;
}

bool SMBC::ConvertedModel::HasStuffToConvert() {
	int64_t _StuffCounter = 0;
	for (SMBC::ObjectCollection& _CurCol : this->ObjCollection) {
		_StuffCounter += (int64_t)_CurCol.PartList.size();
		_StuffCounter += (int64_t)_CurCol.BlockList.size();
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

void SMBC::ConvertedModel::OffsetData::UpdateValues(const glm::vec3& val) {
	if (this->min_val.x > val.x) this->min_val.x = val.x;
	if (this->min_val.y > val.y) this->min_val.y = val.y;
	if (this->min_val.z > val.z) this->min_val.z = val.z;

	if (this->max_val.x < val.x) this->max_val.x = val.x;
	if (this->max_val.y < val.y) this->max_val.y = val.y;
	if (this->max_val.z < val.z) this->max_val.z = val.z;
}

void SMBC::ConvertedModel::LoadBlueprintBlocks(SMBC::ObjectCollection& collection) {
	bool _separate_parts = (conv_data.separation_method == SMBC_SEPARATION_BLOCKS);

	uint32_t blk_list_size = (uint32_t)collection.BlockList.size();
	SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_READING_BLOCKS, blk_list_size);
	for (uint32_t block_idx = 0u; block_idx < blk_list_size; block_idx++) {
		SMBC::SM_Block& Block = collection.BlockList[block_idx];

		this->CacheManager.LoadBlock(Block, conv_data.mat_by_color);
		SMBC::CubeMesh _Cube(Block.bounds / 2.0f, Block.position, Block.tiling);

		NewModel.vertices.reserve(_Cube.Vertices.size());
		for (glm::vec3& _vert : _Cube.Vertices) {
			glm::vec3& _RotPt = SMBC::BPFunction::GetPartRotation(_vert, Block.bounds, Block.xAxis, Block.zAxis) + Block.position;
			this->offsetData.UpdateValues(_RotPt);

			NewModel.vertices.push_back(_RotPt);
		}

		if (conv_data.export_uvs && !_Cube.TexturePoints.empty()) {
			NewModel.uvs.reserve(_Cube.TexturePoints.size());
			NewModel.uvs.insert(NewModel.uvs.end(), _Cube.TexturePoints.begin(), _Cube.TexturePoints.end());
		}

		if (conv_data.export_normals && !_Cube.Normals.empty()) {
			NewModel.normals.reserve(_Cube.Normals.size());
			for (glm::vec3& _norm : _Cube.Normals) {
				glm::vec3& _RotPt = SMBC::BPFunction::GetPartRotation(_norm, glm::vec3(0.0f), Block.xAxis, Block.zAxis);

				NewModel.normals.push_back(_RotPt);
			}
		}

		NewSubMeshData.DataIdx.reserve(_Cube.DataIndexes.size());
		for (uint32_t a = 0u; a < _Cube.DataIndexes.size(); a++) {
			std::vector<std::vector<int64_t>> FaceData = {};

			FaceData.reserve(_Cube.DataIndexes[a].size());
			for (std::vector<uint32_t>& D_Idx : _Cube.DataIndexes[a]) {
				FaceData.push_back({
					(int64_t)D_Idx[0] + this->offsetData.Vertex,
					(conv_data.export_uvs ? (int64_t)D_Idx[1] + this->offsetData.Texture : -1ll),
					(conv_data.export_normals ? (int64_t)D_Idx[2] + this->offsetData.Normal : -1ll)
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

		this->offsetData.Vertex += (int64_t)_Cube.Vertices.size();
		if (conv_data.export_uvs) this->offsetData.Texture += (int64_t)_Cube.TexturePoints.size();
		if (conv_data.export_normals) this->offsetData.Normal += (int64_t)_Cube.Normals.size();

		SMBC::BlueprintConversionData::ProgressBarValue++;
	}
}

void SMBC::ConvertedModel::LoadBlueprintParts(SMBC::ObjectCollection& collection) {
	bool _separate_parts = (conv_data.separation_method == SMBC_SEPARATION_BLOCKS);

	SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_READING_PARTS, (uint32_t)collection.PartList.size());
	for (uint32_t part_idx = 0u; part_idx < collection.PartList.size(); part_idx++) {
		SMBC::SM_Part& Part = collection.PartList[part_idx];

		SMBC::CachedPart _CachedPart;
		this->CacheManager.LoadPart(Part, _CachedPart, conv_data.export_uvs, conv_data.export_normals, conv_data.mat_by_color);

		SMBC::Model CachedModel;
		if (!this->CacheManager.LoadModel(Part.object_path, CachedModel, conv_data.export_uvs, conv_data.export_normals))
			continue;

		NewModel.vertices.reserve(CachedModel.vertices.size());
		for (glm::vec3& vert : CachedModel.vertices) {
			glm::vec3& _RotPt = SMBC::BPFunction::GetPartRotation(vert, Part.bounds, Part.xAxis, Part.zAxis) + Part.position;
			this->offsetData.UpdateValues(_RotPt);

			NewModel.vertices.push_back(_RotPt);
		}

		if (conv_data.export_uvs && !CachedModel.uvs.empty()) {
			NewModel.uvs.reserve(CachedModel.uvs.size());
			NewModel.uvs.insert(NewModel.uvs.end(), CachedModel.uvs.begin(), CachedModel.uvs.end());
		}

		if (conv_data.export_normals && !CachedModel.normals.empty()) {
			NewModel.normals.reserve(CachedModel.normals.size());

			for (glm::vec3& norm : CachedModel.normals) {
				glm::vec3& _RotNor = SMBC::BPFunction::GetPartRotation(norm, glm::vec3(0.0f), Part.xAxis, Part.zAxis);

				NewModel.normals.push_back(_RotNor);
			}
		}

		if (conv_data.apply_texture) NewModel.subMeshData.reserve(CachedModel.subMeshData.size());
		for (SMBC::SubMeshData& _SMData : CachedModel.subMeshData) {
			NewSubMeshData.DataIdx.reserve(_SMData.DataIdx.size());
			for (uint32_t b = 0u; b < _SMData.DataIdx.size(); b++) {
				std::vector<std::vector<int64_t>> FaceIdx = {};

				FaceIdx.reserve(_SMData.DataIdx[b].size());
				for (std::vector<int64_t>& F_Idx : _SMData.DataIdx[b]) {
					bool _HasTextures = (F_Idx[1] > -1) && conv_data.export_uvs;
					bool _HasNormals = (F_Idx[2] > -1) && conv_data.export_normals;

					FaceIdx.push_back({
						F_Idx[0] + this->offsetData.Vertex,
						(_HasTextures) ? (F_Idx[1] + this->offsetData.Texture) : -1ll,
						(_HasNormals) ? (F_Idx[2] + this->offsetData.Normal) : -1ll
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

		this->offsetData.Vertex += (int64_t)CachedModel.vertices.size();
		if (conv_data.export_uvs) this->offsetData.Texture += (int64_t)CachedModel.uvs.size();
		if (conv_data.export_normals) this->offsetData.Normal += (int64_t)CachedModel.normals.size();

		SMBC::BlueprintConversionData::ProgressBarValue++;
	}
}

bool SMBC::ConvertedModel::WriteChunksToFile(
	std::vector<SMBC::Model>& models,
	glm::vec3& posOffset
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
		if (conv_data.apply_texture) this->CacheManager.WriteMtlFile(_path + L".mtl", conv_data.mat_by_color);
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

	for (SMBC::Model& DChunk : models) {
		if (_any_separation) {
			std::string _c_name = "o ";
			_c_name.append(SMBC::Other::WideToUtf8(DChunk.meshPath));
			_c_name.append("\n");

			_writer.write(_c_name.c_str(), _c_name.length());
		}

		SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_WRITING_VERTICES, (uint32_t)DChunk.vertices.size());
		for (glm::vec3& _vert : DChunk.vertices) {
			std::string _v_str = "v ";

			_v_str.append(SMBC::Other::VecToString(_vert - posOffset));
			_v_str.append("\n");

			_writer.write(_v_str.c_str(), _v_str.length());
			SMBC::BlueprintConversionData::ProgressBarValue++;
		}

		if (conv_data.export_uvs) {
			SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_WRITING_UVS, (uint32_t)DChunk.uvs.size());
			for (glm::vec2& _uv : DChunk.uvs) {
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
			for (glm::vec3& _norm : DChunk.normals) {
				std::string _n_str = "vn ";

				_n_str.append(SMBC::Other::VecToString(_norm));
				_n_str.append("\n");

				_writer.write(_n_str.c_str(), _n_str.length());
				SMBC::BlueprintConversionData::ProgressBarValue++;
			}
		}

		SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_WRITING_FACES, (uint32_t)DChunk.subMeshData.size());
		for (SMBC::SubMeshData& _SubData : DChunk.subMeshData) {
			if (_SubData.is_empty()) continue;

			if (conv_data.apply_texture) {
				std::string _u_mtl = "usemtl ";
				_u_mtl.append(SMBC::Other::WideToUtf8(_SubData.MaterialName));
				_u_mtl.append("\n");

				_writer.write(_u_mtl.c_str(), _u_mtl.length());
			}

			for (uint32_t c = 0u; c < _SubData.DataIdx.size(); c++) {
				std::string _f_output = "f";

				for (std::vector<int64_t>& D_Idx : _SubData.DataIdx[c]) {
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
#if _DEBUG
#include <assimp/DefaultLogger.hpp>
#endif

int SMBC::ConvertedModel::ConvertAndWrite() {
	if (!this->HasStuffToConvert()) return SMBC_ERROR_NO_BLOCKS;

#if _DEBUG
	Assimp::Logger* logger = Assimp::DefaultLogger::create("AssimpDebugLog.txt", Assimp::Logger::LogSeverity::DEBUGGING);
#endif

	bool _joint_separation = (conv_data.separation_method == SMBC_SEPARATION_JOINTS);

	for (int a = 0; a < (int)this->ObjCollection.size(); a++) {
		SMBC::ObjectCollection& cur_col = this->ObjCollection[a];

		this->LoadBlueprintBlocks(cur_col);
		this->LoadBlueprintParts(cur_col);

		if (_joint_separation) {
			NewModel.meshPath = this->ModelName + L' ' + std::to_wstring(a);

			NewModel.subMeshData.push_back(NewSubMeshData);
			NewSubMeshData = SMBC::SubMeshData();

			Models.push_back(NewModel);
			NewModel = SMBC::Model();
		}
	}

#if _DEBUG
	delete logger;
#endif

	if (conv_data.separation_method == 0) {
		NewModel.subMeshData.push_back(NewSubMeshData);
		Models.push_back(NewModel);
	}

	glm::vec3 _PosOffset = (offsetData.min_val + offsetData.max_val) / 2.0f;
	if (!this->WriteChunksToFile(Models, _PosOffset))
		return SMBC_ERROR_WRITE;

	return SMBC_ERROR_NONE;
}