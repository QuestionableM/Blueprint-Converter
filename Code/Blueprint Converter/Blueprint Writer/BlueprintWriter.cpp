#include <filesystem>
#include <fstream>

#include "BlueprintWriter.h"
#include "Blueprint Converter/BlueprintConverter.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/String/String.h"
#include "Lib/File/FileFunc.h"
#include "Lib/Json/JsonFunc.h"

namespace fs = std::filesystem;

bool SMBC::ObjectCollection::is_empty() {
	return (this->BlockList.size() <= 0 && this->PartList.size() <= 0);
}

typedef SMBC::ConvertedModel _ConvertedModel;

bool _ConvertedModel::IsLastCollectionEmpty() {
	if (this->ObjCollection.size() > 0)
		return this->ObjCollection[this->ObjCollection.size() - 1].is_empty();

	return false;
}

std::size_t _ConvertedModel::CreateNewCollection() {
	this->ObjCollection.push_back(SMBC::ObjectCollection());
	return this->ObjCollection.size() - 1;
}

std::size_t _ConvertedModel::HasUuidCollection(const std::wstring& uuid, const std::wstring& color, const bool& useColor) {
	for (std::size_t a = 0u; a < this->ObjCollection.size(); a++) {
		SMBC::ObjectCollection& objCol = this->ObjCollection[a];

		for (SMBC::SM_Block& curBlock : objCol.BlockList)
			if (curBlock.uuid == uuid && ((useColor && curBlock.color == color) || !useColor))
				return a;

		for (SMBC::SM_Part& curPart : objCol.PartList)
			if (curPart.objPtr->Uuid == uuid && ((useColor && curPart.color == color) || !useColor))
				return a;
	}

	return this->CreateNewCollection();
}

bool _ConvertedModel::AddJointToChildShape(SMBC::SM_Part& joint) {
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

bool _ConvertedModel::HasStuffToConvert() {
	int64_t _StuffCounter = 0;
	for (SMBC::ObjectCollection& _CurCol : this->ObjCollection) {
		_StuffCounter += (int64_t)_CurCol.PartList.size();
		_StuffCounter += (int64_t)_CurCol.BlockList.size();

		if (_StuffCounter > 0) return true;
	}

	return false;
}

_ConvertedModel::ConvertedModel(SMBC::ConvertedModel::ConvertedModelData& cm_data) {
	this->conv_data.apply_texture = cm_data.apply_texture;
	this->conv_data.export_normals = cm_data.export_normals;
	this->conv_data.export_uvs = cm_data.export_uvs;
	this->conv_data.separation_method = cm_data.separation_method;
	this->conv_data.tex_list = cm_data.tex_list;
	this->conv_data.mat_by_color = cm_data.mat_by_color;
}

void _ConvertedModel::OffsetData::UpdateValues(const glm::vec3& val) {
	this->pt_sum += val;
	this->point_count++;
}

void _ConvertedModel::LoadBlueprintBlocks(SMBC::ObjectCollection& collection) {
	for (std::size_t blk_idx = 0; blk_idx < collection.BlockList.size(); blk_idx++) {
		SMBC::SM_Block& block = collection.BlockList[blk_idx];

		SMBC::ObjectStorage::LoadBlock(block, conv_data.mat_by_color);

		SMBC::CubeMesh _Cube(
			block.bounds / 2.0f,
			block.position,
			(block.blkPtr != nullptr) ? block.blkPtr->Tiling : 4
		);

		for (glm::vec3& vert : _Cube.Vertices) {
			glm::vec3& rotVert = SMBC::BPFunction::GetPartRotation(vert, block.bounds, block.xAxis, block.zAxis) + block.position;
			rotVert = glm::vec4(rotVert, 1.0f) * this->rotMatrix;

			offsetData.UpdateValues(rotVert);
		}

		SMBC::ConvData::ProgressValue++;
	}
}

void _ConvertedModel::LoadBlueprintParts(SMBC::ObjectCollection& collection) {
	for (std::size_t prt_idx = 0; prt_idx < collection.PartList.size(); prt_idx++) {
		SMBC::SM_Part& part = collection.PartList[prt_idx];

		SMBC::CachedPart* c_Part = SMBC::ObjectStorage::LoadPart(part, conv_data.export_uvs, conv_data.export_normals, conv_data.mat_by_color);
		part.modelPtr = c_Part->modelPtr;

		if (c_Part->modelPtr == nullptr) continue;

		for (glm::vec3& vert : c_Part->modelPtr->vertices) {
			glm::vec3& rotVert = SMBC::BPFunction::GetPartRotation(vert, part.bounds, part.xAxis, part.zAxis) + part.position;
			rotVert = glm::vec4(rotVert, 1.0f) * this->rotMatrix;

			offsetData.UpdateValues(rotVert);
		}

		SMBC::ConvData::ProgressValue++;
	}
}

SMBC::Error _ConvertedModel::WriteBlueprintToFile(const unsigned long long& objectCount) {
	if (!SMBC::File::SafeCreateDir(L"./Converted Models"))
		return SMBC::Error::MainDirCreate;

	std::wstring _ModelPath = L"./Converted Models/" + this->ModelName;

	if (SMBC::File::IsBadPath(_ModelPath))
		return SMBC::Error::InvalidPath;

	std::ofstream _writer;

	if (conv_data.tex_list || conv_data.apply_texture) {
		if (!SMBC::File::SafeCreateDir(_ModelPath))
			return SMBC::Error::BPDirCreate;
		
		std::wstring _path = _ModelPath + L"/" + this->ModelName;
		_writer.open(_path + L".obj");

		if (conv_data.tex_list) SMBC::ObjectStorage::WriteTexturePaths(_ModelPath + L"/Used Textures.json");
		if (conv_data.apply_texture) SMBC::ObjectStorage::WriteMtlFile(_path + L".mtl", conv_data.mat_by_color);
	}
	else _writer.open(_ModelPath + L".obj");

	if (!_writer.is_open()) return SMBC::Error::Write;

	if (conv_data.apply_texture) {
		std::string _mtl_lib = "mtllib ";
		SMBC::String::Combine(_mtl_lib, this->ModelName, ".mtl\n");

		_writer.write(_mtl_lib.c_str(), _mtl_lib.length());
	}

	glm::vec3 offsetVec = offsetData.pt_sum / (float)offsetData.point_count;

	bool _uuid_separation = (conv_data.separation_method == SMBC::Sep_Uuid || conv_data.separation_method == SMBC::Sep_UuidAndColor);

	SMBC::ConvData::SetState(SMBC::State::WritingObjects, objectCount);
	for (std::size_t colIdx = 0; colIdx < this->ObjCollection.size(); colIdx++) {
		SMBC::ObjectCollection& curCol = this->ObjCollection[colIdx];

		if (conv_data.separation_method == SMBC::Sep_Joints || _uuid_separation) {
			std::string obj_label = "o ";
			SMBC::String::Combine(obj_label, "Collection_", colIdx + 1, "\n");

			_writer.write(obj_label.c_str(), obj_label.length());
		}

		for (std::size_t blk_idx = 0; blk_idx < curCol.BlockList.size(); blk_idx++) {
			SMBC::SM_Block& curBlock = curCol.BlockList[blk_idx];

			if (conv_data.separation_method == SMBC::Sep_Blocks) {
				std::string obj_label = "o ";
				SMBC::String::Combine(obj_label, "Block_", blk_idx + 1, "\n");

				_writer.write(obj_label.c_str(), obj_label.length());
			}

			SMBC::CubeMesh _Cube(
				curBlock.bounds / 2.0f,
				curBlock.position,
				(curBlock.blkPtr != nullptr) ? curBlock.blkPtr->Tiling : 4
			);

			for (glm::vec3& vert : _Cube.Vertices) {
				glm::vec3& rotVert = SMBC::BPFunction::GetPartRotation(vert, curBlock.bounds, curBlock.xAxis, curBlock.zAxis) + curBlock.position;
				rotVert = glm::vec4(rotVert, 1.0f) * this->rotMatrix;

				std::string _vert_str = "v ";
				SMBC::String::Combine(_vert_str, SMBC::Other::VecToString(rotVert - offsetVec), "\n");

				_writer.write(_vert_str.c_str(), _vert_str.length());
			}

			if (conv_data.export_uvs) {
				for (glm::vec2& uv : _Cube.TexturePoints) {
					std::string _uv_str = "vt ";
					SMBC::String::Combine(_uv_str, uv.x, " ", uv.y, "\n");

					_writer.write(_uv_str.c_str(), _uv_str.length());
				}
			}

			if (conv_data.export_normals) {
				for (glm::vec3& norm : _Cube.Normals) {
					glm::vec3& rotNorm = SMBC::BPFunction::GetPartRotation(norm, glm::vec3(0.0f), curBlock.xAxis, curBlock.zAxis);
					rotNorm = glm::vec4(rotNorm, 1.0f) * this->rotMatrix;

					std::string _norm_str = "vn ";
					SMBC::String::Combine(_norm_str, SMBC::Other::VecToString(rotNorm), "\n");

					_writer.write(_norm_str.c_str(), _norm_str.length());
				}
			}

			if (conv_data.apply_texture) {
				std::string _mat_name = "usemtl ";
				SMBC::String::Combine(_mat_name, curBlock.uuid);

				if (conv_data.mat_by_color)
					SMBC::String::Combine(_mat_name, " ", curBlock.color);

				_mat_name.append("\n");

				_writer.write(_mat_name.c_str(), _mat_name.length());
			}

			for (std::size_t a = 0; a < _Cube.DataIndexes.size(); a++) {
				std::string _f_str = "f";

				for (std::vector<uint32_t>& D_Idx : _Cube.DataIndexes[a]) {
					SMBC::String::Combine(_f_str, " ", D_Idx[0] + offsetData.Vertex + 1);

					if (!conv_data.export_normals && !conv_data.export_uvs) continue;

					_f_str.append("/");

					if (conv_data.export_uvs)
						_f_str.append(std::to_string(D_Idx[1] + offsetData.Texture + 1));

					if (conv_data.export_normals)
						SMBC::String::Combine(_f_str, "/", D_Idx[2] + offsetData.Normal + 1);
				}

				_f_str.append("\n");
				_writer.write(_f_str.c_str(), _f_str.length());
			}

			offsetData.Vertex += (long long)_Cube.Vertices.size();
			offsetData.Texture += (long long)_Cube.TexturePoints.size();
			offsetData.Normal += (long long)_Cube.Normals.size();

			SMBC::ConvData::ProgressValue++;
		}

		for (std::size_t prt_idx = 0; prt_idx < curCol.PartList.size(); prt_idx++) {
			SMBC::SM_Part& curPart = curCol.PartList[prt_idx];

			if (curPart.modelPtr == nullptr) continue;
			SMBC::Model*& curModel = curPart.modelPtr;

			if (conv_data.separation_method == SMBC::Sep_Blocks) {
				std::string obj_label = "o ";
				SMBC::String::Combine(obj_label, "Part_", prt_idx + 1, "\n");

				_writer.write(obj_label.c_str(), obj_label.length());
			}

			for (glm::vec3& vert : curModel->vertices) {
				glm::vec3& rotVert = SMBC::BPFunction::GetPartRotation(vert, curPart.bounds, curPart.xAxis, curPart.zAxis) + curPart.position;
				rotVert = glm::vec4(rotVert, 1.0f) * this->rotMatrix;

				std::string _vert_str = "v ";
				SMBC::String::Combine(_vert_str, SMBC::Other::VecToString(rotVert - offsetVec), "\n");

				_writer.write(_vert_str.c_str(), _vert_str.length());
			}

			if (conv_data.export_uvs) {
				for (glm::vec2& uv : curModel->uvs) {
					std::string _uv_str = "vt ";
					SMBC::String::Combine(_uv_str, uv.x, " ", uv.y, "\n");

					_writer.write(_uv_str.c_str(), _uv_str.length());
				}
			}

			if (conv_data.export_normals) {
				for (glm::vec3& norm : curModel->normals) {
					glm::vec3& rotNorm = SMBC::BPFunction::GetPartRotation(norm, glm::vec3(0.0f), curPart.xAxis, curPart.zAxis);
					rotNorm = glm::vec4(rotNorm, 1.0f) * this->rotMatrix;

					std::string _norm_str = "vn ";
					SMBC::String::Combine(_norm_str, SMBC::Other::VecToString(rotNorm), "\n");

					_writer.write(_norm_str.c_str(), _norm_str.length());
				}
			}

			for (std::size_t subDataIdx = 0; subDataIdx < curModel->subMeshData.size(); subDataIdx++) {
				SMBC::SubMeshData*& subData = curModel->subMeshData[subDataIdx];

				if (conv_data.apply_texture) {
					std::string _mat_name = "usemtl ";
					SMBC::String::Combine(_mat_name, curPart.objPtr->Uuid);

					if (conv_data.mat_by_color)
						SMBC::String::Combine(_mat_name, " ", curPart.color);

					SMBC::String::Combine(_mat_name, " ", subDataIdx, "\n");

					_writer.write(_mat_name.c_str(), _mat_name.length());
				}

				for (std::size_t a = 0; a < subData->DataIdx.size(); a++) {
					std::string _f_str = "f";

					for (std::vector<long long>& D_Idx : subData->DataIdx[a]) {
						bool _HasUv = (D_Idx[1] > -1) && conv_data.export_uvs;
						bool _HasNormal = (D_Idx[2] > -1) && conv_data.export_normals;

						SMBC::String::Combine(_f_str, " ", D_Idx[0] + offsetData.Vertex + 1);

						if (!_HasUv && !_HasNormal) continue;

						_f_str.append("/");

						if (_HasUv)
							_f_str.append(std::to_string(D_Idx[1] + offsetData.Texture + 1));
						
						if (_HasNormal)
							SMBC::String::Combine(_f_str, "/", D_Idx[2] + offsetData.Normal + 1);
					}

					_f_str.append("\n");
					_writer.write(_f_str.c_str(), _f_str.length());
				}
			}

			offsetData.Vertex += (long long)curModel->vertices.size();
			offsetData.Texture += (long long)curModel->uvs.size();
			offsetData.Normal += (long long)curModel->normals.size();

			SMBC::ConvData::ProgressValue++;
		}
	}

	_writer.close();

	SMBC::ObjectStorage::ClearStorage();
	SMBC::ModelStorage::ClearStorage();

	return SMBC::Error::None;
}

SMBC::Error _ConvertedModel::LoadBlueprintData(const std::wstring& blueprint_path) {
	nlohmann::json _BlueprintJson = SMBC::Json::LoadParseJson(blueprint_path);

	if (!_BlueprintJson.is_object())
		return SMBC::Error::File;

	if (!_BlueprintJson.contains("bodies") && !_BlueprintJson.contains("joints"))
		return SMBC::Error::NoBpData;

	bool _separate_joints = (conv_data.separation_method == SMBC::Sep_Joints);
	bool _uuidColor_separation = (conv_data.separation_method == SMBC::Sep_UuidAndColor);
	bool _uuid_separation = (conv_data.separation_method == SMBC::Sep_Uuid || _uuidColor_separation);

	std::size_t collectionIdx = 0;

	if (!_separate_joints && !_uuid_separation)
		collectionIdx = this->CreateNewCollection();

	SMBC::ConvData::SetState(SMBC::State::GettingObjects);
	const auto& _BodiesArray = SMBC::Json::Get(_BlueprintJson, "bodies");
	if (_BodiesArray.is_array()) {
		long _ObjectIndexValue = 0;

		for (auto& _Body : _BodiesArray) {
			const auto& _Childs = SMBC::Json::Get(_Body, "childs");
			if (!_Childs.is_array()) continue;

			if (_separate_joints && !this->IsLastCollectionEmpty())
				collectionIdx = this->CreateNewCollection();

			SMBC::ConvData::ProgressMax += _Childs.size();
			for (auto& _Child : _Childs) {
				const auto& _ShapeId = SMBC::Json::Get(_Child, "shapeId");
				const auto& _Pos = SMBC::Json::Get(_Child, "pos");
				const auto& _XAxis = SMBC::Json::Get(_Child, "xaxis");
				const auto& _ZAxis = SMBC::Json::Get(_Child, "zaxis");
				const auto& _Bounds = SMBC::Json::Get(_Child, "bounds");
				const auto& _Color = SMBC::Json::Get(_Child, "color");

				if (!(_ShapeId.is_string() && _Pos.is_object() && _XAxis.is_number() && _ZAxis.is_number())) continue;
				const auto& _PosX = SMBC::Json::Get(_Pos, "x");
				const auto& _PosY = SMBC::Json::Get(_Pos, "y");
				const auto& _PosZ = SMBC::Json::Get(_Pos, "z");

				if (!(_PosX.is_number() && _PosY.is_number() && _PosZ.is_number())) continue;
				glm::vec3 _PosVec(_PosX.get<float>(), _PosY.get<float>(), _PosZ.get<float>());

				std::wstring _UuidWstr = SMBC::String::ToWide(_ShapeId.get<std::string>());
				std::wstring _ColorWstr = (_Color.is_string() ? SMBC::String::ToWide(_Color.get<std::string>()) : L"000000");

				if (_Bounds.is_object()) {
					const auto& _BoundX = SMBC::Json::Get(_Bounds, "x");
					const auto& _BoundY = SMBC::Json::Get(_Bounds, "y");
					const auto& _BoundZ = SMBC::Json::Get(_Bounds, "z");

					if (!(_BoundX.is_number() && _BoundY.is_number() && _BoundZ.is_number())) continue;
					glm::vec3 _BoundsVec(_BoundX.get<float>(), _BoundY.get<float>(), _BoundZ.get<float>());

					const SMBC::BlockData* _BlockD = SMBC::Mod::GetBlock(_UuidWstr);
					bool blockExists = (_BlockD != nullptr);

					SMBC::SM_Block _Block;
					_Block.blkPtr = (SMBC::BlockData*)_BlockD;
					_Block.bounds = _BoundsVec;
					_Block.color = _ColorWstr;
					_Block.position = _PosVec;
					_Block.uuid = _UuidWstr;
					_Block.xAxis = _XAxis.get<int>();
					_Block.zAxis = _ZAxis.get<int>();
					_Block._index = _ObjectIndexValue;

					std::size_t _localColIdx = collectionIdx;

					if (_uuid_separation)
						_localColIdx = this->HasUuidCollection(_Block.uuid, _Block.color, _uuidColor_separation);

					this->ObjCollection[_localColIdx].BlockList.push_back(_Block);

					_ObjectIndexValue++;
					SMBC::ConvData::ProgressValue++;
				}
				else {
					const SMBC::PartData* part_data = SMBC::Mod::GetPart(_UuidWstr);
					if (part_data == nullptr) continue;

					SMBC::SM_Part _Part;
					_Part.objPtr = (SMBC::PartData*)part_data;
					_Part.color = _ColorWstr;
					_Part.bounds = _Part.objPtr->Bounds;
					_Part.position = _PosVec;
					_Part.xAxis = _XAxis.get<int>();
					_Part.zAxis = _ZAxis.get<int>();
					_Part._index = _ObjectIndexValue;

					SMBC::BPFunction::GetPartPosAndBounds(_Part.position, _Part.bounds, _Part.xAxis, _Part.zAxis, false);

					std::size_t _localColIdx = collectionIdx;
					if (_uuid_separation)
						_localColIdx = this->HasUuidCollection(_Part.objPtr->Uuid, _Part.color, _uuidColor_separation);

					this->ObjCollection[_localColIdx].PartList.push_back(_Part);

					_ObjectIndexValue++;
					SMBC::ConvData::ProgressValue++;
				}
			}
		}
	}

	SMBC::ConvData::SetState(SMBC::State::GettingJoints);
	const auto& _JointsArray = SMBC::Json::Get(_BlueprintJson, "joints");
	if (_JointsArray.is_array()) {
		SMBC::ConvData::ProgressMax = _JointsArray.size();

		for (auto& _Joint : _JointsArray) {
			const auto& _Position = SMBC::Json::Get(_Joint, "posA");
			const auto& _XAxis = SMBC::Json::Get(_Joint, "xaxisA");
			const auto& _ZAxis = SMBC::Json::Get(_Joint, "zaxisA");
			const auto& _ShapeId = SMBC::Json::Get(_Joint, "shapeId");
			const auto& _Color = SMBC::Json::Get(_Joint, "color");
			const auto& _ChildA = SMBC::Json::Get(_Joint, "childA");

			if (!(_ShapeId.is_string() && _Position.is_object() && _XAxis.is_number() && _ZAxis.is_number())) continue;
			const auto& _PosX = SMBC::Json::Get(_Joint, "x");
			const auto& _PosY = SMBC::Json::Get(_Joint, "y");
			const auto& _PosZ = SMBC::Json::Get(_Joint, "z");

			if (!(_PosX.is_number() && _PosY.is_number() && _PosZ.is_number())) continue;
			glm::vec3 _JointPos(_PosX.get<float>(), _PosY.get<float>(), _PosZ.get<float>());

			std::wstring _UuidWstr = SMBC::String::ToWide(_ShapeId.get<std::string>());

			const SMBC::PartData* _jnt_data = SMBC::Mod::GetPart(_UuidWstr);
			if (_jnt_data == nullptr || _jnt_data->Path.empty()) continue;

			SMBC::SM_Part _jnt;
			_jnt.objPtr = (SMBC::PartData*)_jnt_data;
			_jnt.color = (_Color.is_string() ? SMBC::String::ToWide(_Color.get<std::string>()) : L"000000");
			_jnt.bounds = _jnt.objPtr->Bounds;
			_jnt.position = _JointPos;
			_jnt.xAxis = _XAxis.get<int>();
			_jnt.zAxis = _ZAxis.get<int>();

			SMBC::BPFunction::GetPartPosAndBounds(_jnt.position, _jnt.bounds, _jnt.xAxis, _jnt.zAxis, true);
			_jnt._index = (_ChildA.is_number() ? _ChildA.get<int>() : -1);

			if (_uuid_separation) {
				std::size_t col_idx = this->HasUuidCollection(_jnt.objPtr->Uuid, _jnt.color, _uuidColor_separation);

				this->ObjCollection[col_idx].PartList.push_back(_jnt);
			}
			else if (!_separate_joints || !this->AddJointToChildShape(_jnt))
				this->ObjCollection[collectionIdx].PartList.push_back(_jnt);

			SMBC::ConvData::ProgressValue++;
		}
	}

	return SMBC::Error::None;
}

#if _DEBUG
#include <assimp/DefaultLogger.hpp>
#endif

SMBC::Error _ConvertedModel::ConvertAndWrite() {
	if (!this->HasStuffToConvert())
		return SMBC::Error::NoBlocks;

#if _DEBUG
	Assimp::Logger* logger = Assimp::DefaultLogger::create("AssimpDebugLog.txt", Assimp::Logger::LogSeverity::DEBUGGING);
#endif

	unsigned long long TotalObjectCount = 0ull;
	for (SMBC::ObjectCollection& cur_col : this->ObjCollection) {
		TotalObjectCount += cur_col.BlockList.size();
		TotalObjectCount += cur_col.PartList.size();
	}
	
	SMBC::ConvData::SetState(SMBC::State::ReadingObjects, TotalObjectCount);
	for (std::size_t a = 0; a < this->ObjCollection.size(); a++) {
		SMBC::ObjectCollection& cur_col = this->ObjCollection[a];

		this->LoadBlueprintBlocks(cur_col);
		this->LoadBlueprintParts(cur_col);
	}

#if _DEBUG
	Assimp::DefaultLogger::kill();
#endif

	return this->WriteBlueprintToFile(TotalObjectCount);
}