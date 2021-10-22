#include "BlueprintWriter.h"
#include "Blueprint Converter/BlueprintConverter.h"
#include "Blueprint Converter/Convert Settings/ConvertSettings.h"
#include "Blueprint Converter/Cache Manager/Object Storage/ObjectStorage.h"
#include "Blueprint Converter/Cache Manager/Model Storage/ModelStorage.h"

#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/String/String.h"
#include "Lib/File/FileFunc.h"
#include "Lib/Json/JsonFunc.h"

#include "DebugCon.h"

#include <filesystem>
#include <fstream>

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

	ObjectCollection::~ObjectCollection()
	{
		for (SMBC::Object*& cur_object : ObjectList)
			delete cur_object;
	}

	bool ConvertedModel::HasStuffToConvert()
	{
		std::size_t _StuffCounter = 0;
		for (const SMBC::ObjectCollection* _CurCol : this->ObjCollection)
		{
			_StuffCounter += _CurCol->ObjectList.size();

			if (_StuffCounter > 0) return true;
		}

		return false;
	}

	ConvertedModel::~ConvertedModel()
	{
		DebugOutL("Removing ", ObjCollection.size(), " collections from memory");
		for (SMBC::ObjectCollection*& obj : ObjCollection)
			delete obj;
	}

	void OffsetData::UpdateValues(const glm::vec3& val)
	{
		this->pt_sum += val;
		this->point_count++;
	}

	std::size_t ConvertedModel::CountTotalObjectAmount()
	{
		std::size_t obj_amount = 0;

		for (const ObjectCollection* cur_col : this->ObjCollection)
			obj_amount += cur_col->ObjectList.size();

		return obj_amount;
	}

	void ConvertedModel::LoadCollections(const std::size_t& total_obj_count)
	{
		ConvData::SetState(State::ReadingObjects, total_obj_count);
		for (ObjectCollection*& cur_col : this->ObjCollection)
			for (SMBC::Object*& object : cur_col->ObjectList)
			{
				ObjectStorage::LoadObject(object);

				object->CalcCenterPoint(offsetData);

				ConvData::ProgressValue++;
			}
	}

	void ConvertedModel::OpenWriter(const std::wstring& path, std::ofstream& out)
	{
		if (ConvertSettings::TextureList || ConvertSettings::ApplyTextures)
		{
			if (!File::SafeCreateDir(path))
				throw Error::BPDirCreate;
			
			std::wstring _path = path + L"/" + this->ModelName;
			out.open(_path + L".obj");

			if (ConvertSettings::TextureList) ObjectStorage::WriteTexturePaths(path + L"/Used Textures.json");
			if (ConvertSettings::ApplyTextures) ObjectStorage::WriteMtlFile(_path + L".mtl");
		}
		else
		{
			out.open(path + L".obj");
		}

		if (!out.is_open()) throw Error::Write;
	}

	void ConvertedModel::CreateAndOpenWriter(std::ofstream& out)
	{
		if (!File::SafeCreateDir(L"./Converted Models"))
			throw Error::MainDirCreate;

		std::wstring _ModelPath = L"./Converted Models/" + this->ModelName;

		if (File::IsBadPath(_ModelPath))
			throw Error::InvalidPath;

		this->OpenWriter(_ModelPath, out);
	}

	void ConvertedModel::WriteMtlHeader(std::ofstream& out)
	{
		if (!ConvertSettings::ApplyTextures) return;

		std::string _mtl_lib = "mtllib ";
		String::Combine(_mtl_lib, this->ModelName, ".mtl\n");

		out.write(_mtl_lib.c_str(), _mtl_lib.size());
	}

	void ConvertedModel::WriteCollectionHeader(std::ofstream& out, const std::size_t& idx, const bool& should_write)
	{
		if (!should_write) return;

		std::string obj_label = "o ";
		String::Combine(obj_label, "Collection_", idx, "\n");

		out.write(obj_label.c_str(), obj_label.size());
	}

	void ConvertedModel::WriteCollectionToFile(std::ofstream& out, const std::vector<SMBC::Object*>& obj_vec, const glm::vec3& offsetVec)
	{
		for (std::size_t obj_idx = 0; obj_idx < obj_vec.size(); obj_idx++)
		{
			const Object* cObject = obj_vec[obj_idx];
			cObject->WriteToFile(out, offsetData, obj_idx, offsetVec);

			ConvData::ProgressValue++;
		}
	}

	Error ConvertedModel::WriteBlueprintToFile(const std::size_t& objectCount)
	{
		try
		{
			std::ofstream _writer;
			this->CreateAndOpenWriter(_writer);
			this->WriteMtlHeader(_writer);

			const glm::vec3 offsetVec = offsetData.pt_sum / (float)offsetData.point_count;
			const bool write_collection_label = (this->ObjCollection.size() > 1);

			ConvData::SetState(State::WritingObjects, objectCount);
			for (std::size_t colIdx = 0; colIdx < this->ObjCollection.size(); colIdx++)
			{
				ObjectCollection*& curCol = this->ObjCollection[colIdx];

				this->WriteCollectionHeader(_writer, colIdx + 1, write_collection_label);
				this->WriteCollectionToFile(_writer, curCol->ObjectList, offsetVec);
			}

			_writer.close();
		}
		catch (Error& error_enum)
		{
			ObjectStorage::ClearStorage();
			ModelStorage::ClearStorage();
			return error_enum;
		}

		ObjectStorage::ClearStorage();
		ModelStorage::ClearStorage();

		return Error::None;
	}

	nlohmann::json ConvertedModel::ReadAndCheckBlueprintFile(const std::wstring& path)
	{
		nlohmann::json bpJson = Json::LoadParseJson(path);

		if (!bpJson.is_object())
			throw Error::File;

		bool has_bodies = (bpJson.find("bodies") != bpJson.end());
		bool has_joints = (bpJson.find("joints") != bpJson.end());

		if (!has_bodies && !has_joints)
			throw Error::NoBpData;

		return bpJson;
	}

	void ConvertedModel::Bind_NoSeparation(ConvertedModel& cModel, SMBC::Object* object, const bool& is_joint)
	{
		SMBC::ObjectCollection* cur_collection = nullptr;

		if (cModel.ObjCollection.size() == 0)
		{
			cur_collection = new SMBC::ObjectCollection();
			cModel.ObjCollection.push_back(cur_collection);
		}
		else
		{
			cur_collection = cModel.ObjCollection[0];
		}

		cur_collection->ObjectList.push_back(object);
	}

	void ConvertedModel::Bind_SeparateJoints(ConvertedModel& cModel, SMBC::Object* object, const bool& is_joint)
	{
		cModel.CreateAndAddObjectToCollection(is_joint ? "joints" : "objects", object);
	}

	void ConvertedModel::Bind_SeparateUuid(ConvertedModel& cModel, SMBC::Object* object, const bool& is_joint)
	{
		cModel.CreateAndAddObjectToCollection(object->Uuid.ToString(), object);
	}

	void ConvertedModel::Bind_SeparateColor(ConvertedModel& cModel, SMBC::Object* object, const bool& is_joint)
	{
		cModel.CreateAndAddObjectToCollection(object->Color.StringHex(), object);
	}

	void ConvertedModel::Bind_SeparateUuidAndColor(ConvertedModel& cModel, SMBC::Object* object, const bool& is_joint)
	{
		cModel.CreateAndAddObjectToCollection(object->Uuid.ToString() + " " + object->Color.StringHex(), object);
	}

	void ConvertedModel::CreateAndAddObjectToCollection(const std::string& col_name, SMBC::Object* object)
	{
		SMBC::ObjectCollection* cur_collection = nullptr;

		if (ObjCollectionMap.find(col_name) != ObjCollectionMap.end())
			cur_collection = ObjCollectionMap.at(col_name);
		else
		{
			cur_collection = new SMBC::ObjectCollection();
			ObjCollectionMap.insert(std::make_pair(col_name, cur_collection));
			ObjCollection.push_back(cur_collection);
		}

		cur_collection->ObjectList.push_back(object);
	}

	void ConvertedModel::LoadBlueprintBodies(const nlohmann::json& bpJson)
	{
		const auto& bArray = SMBC::Json::Get(bpJson, "bodies");
		if (!bArray.is_array()) return;

		bool bSeparateJoints = (ConvertSettings::SeparationMethod == Sep_Joints);

		ConvData::SetState(State::GettingObjects);
		for (auto& _Body : bArray)
		{
			const auto& _Childs = Json::Get(_Body, "childs");
			if (!_Childs.is_array()) continue;

			ConvData::ProgressMax += _Childs.size();
			for (auto& _Child : _Childs)
			{
				const auto& _ShapeId = Json::Get(_Child, "shapeId");
				const auto& _Pos	 = Json::Get(_Child, "pos");
				const auto& _XAxis	 = Json::Get(_Child, "xaxis");
				const auto& _ZAxis	 = Json::Get(_Child, "zaxis");
				const auto& _Bounds  = Json::Get(_Child, "bounds");
				const auto& _Color	 = Json::Get(_Child, "color");

				if (!(_ShapeId.is_string() && _Pos.is_object() && _XAxis.is_number() && _ZAxis.is_number())) continue;
				const auto& _PosX = Json::Get(_Pos, "x");
				const auto& _PosY = Json::Get(_Pos, "y");
				const auto& _PosZ = Json::Get(_Pos, "z");

				if (!(_PosX.is_number() && _PosY.is_number() && _PosZ.is_number())) continue;
				glm::vec3 _PosVec(_PosX.get<float>(), _PosY.get<float>(), _PosZ.get<float>());

				SMBC::Uuid uuid_obj(_ShapeId.get<std::string>());
				std::string color_str = (_Color.is_string() ? _Color.get<std::string>() : "000000");

				if (_Bounds.is_object())
				{
					const auto& _BoundX = Json::Get(_Bounds, "x");
					const auto& _BoundY = Json::Get(_Bounds, "y");
					const auto& _BoundZ = Json::Get(_Bounds, "z");

					if (!(_BoundX.is_number() && _BoundY.is_number() && _BoundZ.is_number())) continue;
					glm::vec3 _BoundsVec(_BoundX.get<float>(), _BoundY.get<float>(), _BoundZ.get<float>());

					const SMBC::BlockData* _BlockD = Mod::GetBlock(uuid_obj);
					if (!_BlockD) continue;

					SMBC::Block* _Block = new SMBC::Block();
					_Block->blkPtr	 = (SMBC::BlockData*)_BlockD;
					_Block->Bounds	 = _BoundsVec;
					_Block->Color	 = color_str;
					_Block->Position = _PosVec;
					_Block->Uuid	 = _BlockD->Uuid;
					_Block->xAxis	 = _XAxis.get<int>();
					_Block->zAxis	 = _ZAxis.get<int>();
					_Block->_index	 = this->objectIndexValue;

					this->CollectionBindFunction(*this, _Block, false);
				}
				else
				{
					const SMBC::PartData* part_data = SMBC::Mod::GetPart(uuid_obj);
					if (!part_data) continue;

					SMBC::Part* _Part = new SMBC::Part();
					_Part->objPtr	= (SMBC::PartData*)part_data;
					_Part->Uuid		= _Part->objPtr->Uuid;
					_Part->Color	= color_str;
					_Part->Bounds	= _Part->objPtr->Bounds;
					_Part->Position = _PosVec;
					_Part->xAxis	= _XAxis.get<int>();
					_Part->zAxis	= _ZAxis.get<int>();
					_Part->_index	= this->objectIndexValue;

					BPFunction::GetPartPosAndBounds(_Part->Position, _Part->Bounds, _Part->xAxis, _Part->zAxis, false);

					this->CollectionBindFunction(*this, _Part, false);
				}

				this->objectIndexValue++;
				ConvData::ProgressValue++;
			}
		}
	}

	void ConvertedModel::LoadBlueprintJoints(const nlohmann::json& bpJson)
	{
		const auto& jArray = Json::Get(bpJson, "joints");
		if (!jArray.is_array()) return;

		ConvData::SetState(State::GettingJoints, jArray.size());
		for (const auto& _Joint : jArray)
		{
			const auto& _Position = Json::Get(_Joint, "posA");
			const auto& _XAxis =	Json::Get(_Joint, "xaxisA");
			const auto& _ZAxis =	Json::Get(_Joint, "zaxisA");
			const auto& _ShapeId =	Json::Get(_Joint, "shapeId");
			const auto& _Color =	Json::Get(_Joint, "color");
			const auto& _ChildA =	Json::Get(_Joint, "childA");

			if (!(_ShapeId.is_string() && _Position.is_object() && _XAxis.is_number() && _ZAxis.is_number())) continue;
			const auto& _PosX = Json::Get(_Position, "x");
			const auto& _PosY = Json::Get(_Position, "y");
			const auto& _PosZ = Json::Get(_Position, "z");

			if (!(_PosX.is_number() && _PosY.is_number() && _PosZ.is_number())) continue;
			glm::vec3 _JointPos(_PosX.get<float>(), _PosY.get<float>(), _PosZ.get<float>());

			SMBC::Uuid uuid_obj(_ShapeId.get<std::string>());

			const SMBC::PartData* _jnt_data = Mod::GetPart(uuid_obj);
			if (!_jnt_data) continue;

			SMBC::Part* _jnt = new SMBC::Part();
			_jnt->Uuid     = _jnt_data->Uuid;
			_jnt->objPtr   = (SMBC::PartData*)_jnt_data;
			_jnt->Color    = (_Color.is_string() ? _Color.get<std::string>() : "000000");
			_jnt->Bounds   = _jnt->objPtr->Bounds;
			_jnt->Position = _JointPos;
			_jnt->xAxis    = _XAxis.get<int>();
			_jnt->zAxis    = _ZAxis.get<int>();

			BPFunction::GetPartPosAndBounds(_jnt->Position, _jnt->Bounds, _jnt->xAxis, _jnt->zAxis, true);
			_jnt->_index = (_ChildA.is_number() ? _ChildA.get<int>() : -1);

			this->CollectionBindFunction(*this, _jnt, true);

			ConvData::ProgressValue++;
		}
	}

	void ConvertedModel::SelectCollectionBinding()
	{
		DebugOutL("Current separation method: ", ConvertSettings::SeparationMethod);
		switch (ConvertSettings::SeparationMethod)
		{
		case Sep_Joints:
			CollectionBindFunction = ConvertedModel::Bind_SeparateJoints;
			break;
		case Sep_Uuid:
			CollectionBindFunction = ConvertedModel::Bind_SeparateUuid;
			break;
		case Sep_Color:
			CollectionBindFunction = ConvertedModel::Bind_SeparateColor;
			break;
		case Sep_UuidAndColor:
			CollectionBindFunction = ConvertedModel::Bind_SeparateUuidAndColor;
			break;
		default:
			CollectionBindFunction = ConvertedModel::Bind_NoSeparation;
			break;
		}
	}

	SMBC::Error ConvertedModel::LoadBlueprintData(const std::wstring& blueprint_path)
	{
		try
		{
			nlohmann::json bpJson = this->ReadAndCheckBlueprintFile(blueprint_path);

			this->objectIndexValue = 0;

			this->SelectCollectionBinding();

			this->LoadBlueprintBodies(bpJson);
			this->LoadBlueprintJoints(bpJson);
		}
		catch (Error& error_enum)
		{
			return error_enum;
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