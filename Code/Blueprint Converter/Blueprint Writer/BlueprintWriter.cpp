#include <filesystem>
#include <fstream>

#include "BlueprintWriter.h"
#include "Blueprint Converter/BlueprintConverter.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/String/String.h"
#include "Lib/File/FileFunc.h"
#include "Lib/Json/JsonFunc.h"
#include "Blueprint Converter/Convert Settings/ConvertSettings.h"

#include "DebugCon.h"

#if _DEBUG
#include <assimp/DefaultLogger.hpp>
#endif

namespace fs = std::filesystem;

namespace SMBC
{
	bool ObjectCollection::IsEmpty()
	{
		return (ObjectList.size() == 0);
	}

	bool ConvertedModel::IsLastCollectionEmpty()
	{
		if (this->ObjCollection.size() > 0)
			return this->ObjCollection[this->ObjCollection.size() - 1].IsEmpty();

		return false;
	}

	std::size_t ConvertedModel::CreateNewCollection()
	{
		this->ObjCollection.push_back(SMBC::ObjectCollection());
		return this->ObjCollection.size() - 1;
	}

	std::size_t ConvertedModel::HasUuidCollection(const SMBC::Uuid& uuid, const std::wstring& color, const bool& useColor)
	{
		for (std::size_t a = 0u; a < this->ObjCollection.size(); a++)
		{
			SMBC::ObjectCollection& objCol = this->ObjCollection[a];

			for (SMBC::Object*& cur_obj : objCol.ObjectList)
				if (cur_obj->Uuid == uuid && ((useColor && cur_obj->Color == color)) || !useColor)
					return a;
		}

		return this->CreateNewCollection();
	}

	bool ConvertedModel::AddJointToChildShape(SMBC::Part& joint)
	{
		for (SMBC::ObjectCollection& col : this->ObjCollection)
			for (SMBC::Object*& cur_obj : col.ObjectList)
			{
				if (cur_obj->_index != joint._index) continue;
				
				//CHANGE THAT SHIT LATER
				col.ObjectList.push_back(&joint);
				return true;
			}

		return false;
	}

	bool ConvertedModel::HasStuffToConvert()
	{
		std::size_t _StuffCounter = 0;
		for (SMBC::ObjectCollection& _CurCol : this->ObjCollection)
		{
			_StuffCounter += _CurCol.ObjectList.size();

			if (_StuffCounter > 0) return true;
		}

		return false;
	}

	void OffsetData::UpdateValues(const glm::vec3& val)
	{
		this->pt_sum += val;
		this->point_count++;
	}

	std::size_t ConvertedModel::CountTotalObjectAmount()
	{
		std::size_t obj_amount = 0;

		for (const ObjectCollection& cur_col : this->ObjCollection)
			obj_amount += cur_col.ObjectList.size();

		return obj_amount;
	}

	void ConvertedModel::LoadCollections(const std::size_t& total_obj_count)
	{
		ConvData::SetState(State::ReadingObjects, total_obj_count);
		for (ObjectCollection& cur_col : this->ObjCollection)
			for (SMBC::Object*& object : cur_col.ObjectList)
			{
				ObjectStorage::LoadObject(object);

				object->CalcCenterPoint(offsetData);

				ConvData::ProgressValue++;
			}
	}

	Error ConvertedModel::WriteBlueprintToFile(const std::size_t& objectCount)
	{
		if (!File::SafeCreateDir(L"./Converted Models"))
			return Error::MainDirCreate;

		std::wstring _ModelPath = L"./Converted Models/" + this->ModelName;

		if (File::IsBadPath(_ModelPath))
			return Error::InvalidPath;

		std::ofstream _writer;
		
		if (ConvertSettings::TextureList || ConvertSettings::ApplyTextures)
		{
			if (!File::SafeCreateDir(_ModelPath))
				return Error::BPDirCreate;

			std::wstring _path = _ModelPath + L"/" + this->ModelName;
			_writer.open(_path + L".obj");

			if (ConvertSettings::TextureList) ObjectStorage::WriteTexturePaths(_ModelPath + L"/Used Textures.json");
			if (ConvertSettings::ApplyTextures) ObjectStorage::WriteMtlFile(_path + L".mtl");
		}
		else _writer.open(_ModelPath + L".obj");

		if (!_writer.is_open()) return Error::Write;

		if (ConvertSettings::ApplyTextures)
		{
			std::string _mtl_lib = "mtllib ";
			String::Combine(_mtl_lib, this->ModelName, ".mtl\n");

			_writer.write(_mtl_lib.c_str(), _mtl_lib.length());
		}

		glm::vec3 offsetVec = offsetData.pt_sum / (float)offsetData.point_count;

		const int sep_method = ConvertSettings::SeparationMethod;
		bool _uuid_separation = (sep_method == Sep_Uuid || sep_method == Sep_UuidAndColor);
		bool _joint_sep = (sep_method == Sep_Joints);

		SMBC::ConvData::SetState(SMBC::State::WritingObjects, objectCount);
		for (std::size_t colIdx = 0; colIdx < this->ObjCollection.size(); colIdx++)
		{
			SMBC::ObjectCollection& curCol = this->ObjCollection[colIdx];

			if (_joint_sep || _uuid_separation)
			{
				std::string obj_label = "o ";
				SMBC::String::Combine(obj_label, "Collection_", colIdx + 1, "\n");

				_writer.write(obj_label.c_str(), obj_label.length());
			}

			for (std::size_t obj_idx = 0; obj_idx < curCol.ObjectList.size(); obj_idx++)
			{
				const SMBC::Object* cObject = curCol.ObjectList[obj_idx];
				cObject->WriteToFile(_writer, offsetData, obj_idx, offsetVec);

				ConvData::ProgressValue++;
			}
		}

		_writer.close();

		SMBC::ObjectStorage::ClearStorage();
		SMBC::ModelStorage::ClearStorage();

		return SMBC::Error::None;
	}

	SMBC::Error ConvertedModel::LoadBlueprintData(const std::wstring& blueprint_path)
	{
		nlohmann::json _BlueprintJson = SMBC::Json::LoadParseJson(blueprint_path);

		if (!_BlueprintJson.is_object())
			return SMBC::Error::File;

		if (!_BlueprintJson.contains("bodies") && !_BlueprintJson.contains("joints"))
			return SMBC::Error::NoBpData;

		const int sep_method = ConvertSettings::SeparationMethod;
		bool _separate_joints	   = (sep_method == Sep_Joints);
		bool _uuidColor_separation = (sep_method == Sep_UuidAndColor);
		bool _uuid_separation	   = (sep_method == Sep_Uuid || _uuidColor_separation);

		std::size_t collectionIdx = 0;

		if (!_separate_joints && !_uuid_separation)
			collectionIdx = this->CreateNewCollection();

		SMBC::ConvData::SetState(SMBC::State::GettingObjects);
		const auto& _BodiesArray = SMBC::Json::Get(_BlueprintJson, "bodies");
		if (_BodiesArray.is_array())
		{
			long _ObjectIndexValue = 0;

			for (auto& _Body : _BodiesArray)
			{
				const auto& _Childs = SMBC::Json::Get(_Body, "childs");
				if (!_Childs.is_array()) continue;

				if (_separate_joints && !this->IsLastCollectionEmpty())
					collectionIdx = this->CreateNewCollection();

				SMBC::ConvData::ProgressMax += _Childs.size();
				for (auto& _Child : _Childs)
				{
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

					SMBC::Uuid uuid_obj(_ShapeId.get<std::string>());
					std::wstring _ColorWstr = (_Color.is_string() ? SMBC::String::ToWide(_Color.get<std::string>()) : L"000000");

					if (_Bounds.is_object())
					{
						const auto& _BoundX = SMBC::Json::Get(_Bounds, "x");
						const auto& _BoundY = SMBC::Json::Get(_Bounds, "y");
						const auto& _BoundZ = SMBC::Json::Get(_Bounds, "z");

						if (!(_BoundX.is_number() && _BoundY.is_number() && _BoundZ.is_number())) continue;
						glm::vec3 _BoundsVec(_BoundX.get<float>(), _BoundY.get<float>(), _BoundZ.get<float>());

						const SMBC::BlockData* _BlockD = SMBC::Mod::GetBlock(uuid_obj);
						bool blockExists = (_BlockD != nullptr);

						SMBC::Block* _Block = new SMBC::Block();
						_Block->blkPtr = (SMBC::BlockData*)_BlockD;
						_Block->Bounds = _BoundsVec;
						_Block->Color = _ColorWstr;
						_Block->Position = _PosVec;
						_Block->Uuid = uuid_obj;
						_Block->xAxis = _XAxis.get<int>();
						_Block->zAxis = _ZAxis.get<int>();
						_Block->_index = _ObjectIndexValue;

						//std::size_t _localColIdx = collectionIdx;

						//if (_uuid_separation)
						//	_localColIdx = this->HasUuidCollection(_Block.Uuid, _Block.Color, _uuidColor_separation);

						//this->ObjCollection[_localColIdx].BlockList.push_back(_Block);
						this->ObjCollection[collectionIdx].ObjectList.push_back(_Block);


						_ObjectIndexValue++;
						SMBC::ConvData::ProgressValue++;
					}
					else
					{
						const SMBC::PartData* part_data = SMBC::Mod::GetPart(uuid_obj);
						if (part_data == nullptr) continue;

						SMBC::Part* _Part = new SMBC::Part();
						_Part->objPtr = (SMBC::PartData*)part_data;
						_Part->Uuid = _Part->objPtr->Uuid;
						_Part->Color = _ColorWstr;
						_Part->Bounds = _Part->objPtr->Bounds;
						_Part->Position = _PosVec;
						_Part->xAxis = _XAxis.get<int>();
						_Part->zAxis = _ZAxis.get<int>();
						_Part->_index = _ObjectIndexValue;

						SMBC::BPFunction::GetPartPosAndBounds(_Part->Position, _Part->Bounds, _Part->xAxis, _Part->zAxis, false);

						//std::size_t _localColIdx = collectionIdx;
						//if (_uuid_separation)
						//	_localColIdx = this->HasUuidCollection(_Part.objPtr->Uuid, _Part.Color, _uuidColor_separation);

						this->ObjCollection[collectionIdx].ObjectList.push_back(_Part);
						//this->ObjCollection[_localColIdx].PartList.push_back(_Part);

						_ObjectIndexValue++;
						SMBC::ConvData::ProgressValue++;
					}
				}
			}
		}

		SMBC::ConvData::SetState(SMBC::State::GettingJoints);
		const auto& _JointsArray = SMBC::Json::Get(_BlueprintJson, "joints");
		if (_JointsArray.is_array())
		{
			SMBC::ConvData::ProgressMax = _JointsArray.size();

			for (auto& _Joint : _JointsArray)
			{
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

				SMBC::Uuid uuid_obj(_ShapeId.get<std::string>());

				const SMBC::PartData* _jnt_data = SMBC::Mod::GetPart(uuid_obj);
				if (_jnt_data == nullptr || _jnt_data->Path.empty()) continue;

				SMBC::Part* _jnt = new SMBC::Part();
				_jnt->Uuid = _jnt_data->Uuid;
				_jnt->objPtr = (SMBC::PartData*)_jnt_data;
				_jnt->Color = (_Color.is_string() ? SMBC::String::ToWide(_Color.get<std::string>()) : L"000000");
				_jnt->Bounds = _jnt->objPtr->Bounds;
				_jnt->Position = _JointPos;
				_jnt->xAxis = _XAxis.get<int>();
				_jnt->zAxis = _ZAxis.get<int>();

				SMBC::BPFunction::GetPartPosAndBounds(_jnt->Position, _jnt->Bounds, _jnt->xAxis, _jnt->zAxis, true);
				_jnt->_index = (_ChildA.is_number() ? _ChildA.get<int>() : -1);

				this->ObjCollection[collectionIdx].ObjectList.push_back(_jnt);
				//if (_uuid_separation)
				//{
				//	std::size_t col_idx = this->HasUuidCollection(_jnt.objPtr->Uuid, _jnt.Color, _uuidColor_separation);

				//	this->ObjCollection[col_idx].PartList.push_back(_jnt);
				//}
				//else if (!_separate_joints || !this->AddJointToChildShape(_jnt))
				//	this->ObjCollection[collectionIdx].PartList.push_back(_jnt);

				SMBC::ConvData::ProgressValue++;
			}
		}

		return SMBC::Error::None;
	}

	SMBC::Error ConvertedModel::ConvertAndWrite()
	{
		if (!this->HasStuffToConvert())
			return SMBC::Error::NoBlocks;

	#if _DEBUG
		Assimp::Logger* logger = Assimp::DefaultLogger::create("AssimpDebugLog.txt", Assimp::Logger::LogSeverity::DEBUGGING);
	#endif

		std::size_t total_obj_count = this->CountTotalObjectAmount();
		this->LoadCollections(total_obj_count);

	#if _DEBUG
		Assimp::DefaultLogger::kill();
	#endif

		return this->WriteBlueprintToFile(total_obj_count);
	}
}