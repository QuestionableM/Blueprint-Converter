#include "BlueprintWriter.h"

#include "Blueprint Converter/Object Definitions/Entity/BlueprintData/BlueprintData.h"
/*#include "Blueprint Converter/BlueprintConverter.h"
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

namespace fs = std::filesystem;*/

namespace SMBC
{
	/*bool ObjectCollection::IsEmpty()
	{
		return (ObjectList.size() == 0);
	}

	ObjectCollection::~ObjectCollection()
	{
		for (SMBC::Entity*& cur_object : ObjectList)
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
			for (SMBC::Entity*& object : cur_col->ObjectList)
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

	void ConvertedModel::Bind_NoSeparation(ConvertedModel& cModel, SMBC::Entity* object, const bool& is_joint, const std::size_t& body_idx)
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

	void ConvertedModel::Bind_SeparateJoints(ConvertedModel& cModel, SMBC::Entity* object, const bool& is_joint, const std::size_t& body_idx)
	{
		if (is_joint)
		{
			SMBC::ObjectCollection* obj_collection = cModel.GetCollectionFromObjectIndex(object->ObjectIndex);
			if (!obj_collection)
			{
				cModel.CreateAndAddObjectToCollection("Joints", object);
			}
			else
			{
				obj_collection->ObjectList.push_back(object);
			}
		}
		else
		{
			cModel.CreateAndAddObjectToCollection("Objects_" + std::to_string(body_idx), object);
		}
	}

	void ConvertedModel::Bind_SeparateUuid(ConvertedModel& cModel, SMBC::Entity* object, const bool& is_joint, const std::size_t& body_idx)
	{
		cModel.CreateAndAddObjectToCollection(object->Uuid.ToString(), object);
	}

	void ConvertedModel::Bind_SeparateColor(ConvertedModel& cModel, SMBC::Entity* object, const bool& is_joint, const std::size_t& body_idx)
	{
		cModel.CreateAndAddObjectToCollection(object->Color.StringHex(), object);
	}

	void ConvertedModel::Bind_SeparateUuidAndColor(ConvertedModel& cModel, SMBC::Entity* object, const bool& is_joint, const std::size_t& body_idx)
	{
		cModel.CreateAndAddObjectToCollection(object->Uuid.ToString() + " " + object->Color.StringHex(), object);
	}

	SMBC::ObjectCollection* ConvertedModel::GetCollectionFromObjectIndex(const int& idx)
	{
		if (CollectionIndexMap.find(idx) != CollectionIndexMap.end())
			return CollectionIndexMap.at(idx);

		return nullptr;
	}

	void ConvertedModel::CreateAndAddObjectToCollection(const std::string& col_name, SMBC::Object* object)
	{
		SMBC::ObjectCollection* cur_collection = nullptr;

		if (ObjCollectionMap.find(col_name) != ObjCollectionMap.end())
		{
			cur_collection = ObjCollectionMap.at(col_name);
		}
		else
		{
			cur_collection = new SMBC::ObjectCollection();

			ObjCollectionMap.insert(std::make_pair(col_name, cur_collection));
			ObjCollection.push_back(cur_collection);
		}

		cur_collection->ObjectList.push_back(object);
		CollectionIndexMap.insert(std::make_pair(object->ObjectIndex, cur_collection));
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
	}*/
}