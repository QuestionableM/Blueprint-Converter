#include "BlueprintData.h"

#include "Lib\ConvData.h"
#include "Lib\String.h"
#include "Lib\File.h"
#include "Lib\Json.h"

#include "BlueprintConverter\BlueprintConverter.h"
#include "BlueprintConverter\ConvertSettings.h"
#include "BlueprintConverter\ModelStorage.h"
#include "BlueprintConverter\OffsetData.h"

#include "BlueprintConverter\ObjectDefinitions\Joint.h"
#include "BlueprintConverter\ObjectDefinitions\Part.h"
#include "BlueprintConverter\ObjectDefinitions\Block.h"
#include "BlueprintConverter\ObjectDefinitions\Body.h"

#include "DebugCon.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace SMBC
{
	void BlueprintData::Bind_NoSeparation(BlueprintData* bpData, Entity* pEntity)
	{
		SMBC::Body* cCollection = nullptr;

		if (bpData->mCollections.size() == 0)
		{
			cCollection = new SMBC::Body("UnseparatedCollection");
			bpData->mCollections.push_back(cCollection);
		}
		else
		{
			cCollection = bpData->mCollections[0];
		}

		cCollection->Add(pEntity);
	}

	void BlueprintData::Bind_SeparateAll(BlueprintData* bpData, Entity* pEntity)
	{
		bpData->CreateAndAddObjToCollection("Object_" + std::to_string(bpData->ObjectIndex + 1), pEntity);
	}

	void BlueprintData::Bind_SeparateJoints(BlueprintData* bpData, Entity* pEntity)
	{
		if (pEntity->Type() == EntityType::Joint)
		{
			const std::size_t mChildIdx = pEntity->GetIndex();

			const IndexStorageMap::iterator it = bpData->mIndexMap.find(mChildIdx);
			if (it != bpData->mIndexMap.end())
			{
				it->second->Add(pEntity);
			}
			else
			{
				bpData->CreateAndAddObjToCollection("Joints", pEntity);
			}
		}
		else
		{
			bpData->CreateAndAddObjToCollection("Objects_" + std::to_string(bpData->BodyIndex + 1), pEntity);
		}
	}

	void BlueprintData::Bind_SeparateUuid(BlueprintData* bpData, Entity* pEntity)
	{
		bpData->CreateAndAddObjToCollection(pEntity->GetUuid().ToString(), pEntity);
	}

	void BlueprintData::Bind_SeparateColor(BlueprintData* bpData, Entity* pEntity)
	{
		bpData->CreateAndAddObjToCollection(pEntity->GetColor().StringHex(), pEntity);
	}

	void BlueprintData::Bind_SeparateUuidAndColor(BlueprintData* bpData, Entity* pEntity)
	{
		bpData->CreateAndAddObjToCollection(pEntity->GetUuid().ToString() + "_" + pEntity->GetColor().StringHex(), pEntity);
	}

	void BlueprintData::CreateAndAddObjToCollection(const std::string& cName, Entity* pEntity)
	{
		SMBC::Body* cCollection = nullptr;

		const CollectionStorageMap::iterator it = mCollectionsMap.find(cName);
		if (it != mCollectionsMap.end())
		{
			cCollection = it->second;
		}
		else
		{
			cCollection = new SMBC::Body(cName);

			DebugOutL("Created a new collection: ", cName);

			mCollectionsMap.insert(std::make_pair(cName, cCollection));
			mCollections.push_back(cCollection);
		}

		cCollection->Add(pEntity);
		mIndexMap.insert(std::make_pair(pEntity->GetIndex(), cCollection));
	}

	void BlueprintData::SelectSeparationMethod()
	{
		switch (ConvertSettings::SeparationMethod)
		{
		case Sep_Blocks:
			CollectionBindFunction = BlueprintData::Bind_SeparateAll;
			break;
		case Sep_Joints:
			CollectionBindFunction = BlueprintData::Bind_SeparateJoints;
			break;
		case Sep_Uuid:
			CollectionBindFunction = BlueprintData::Bind_SeparateUuid;
			break;
		case Sep_Color:
			CollectionBindFunction = BlueprintData::Bind_SeparateColor;
			break;
		case Sep_UuidAndColor:
			CollectionBindFunction = BlueprintData::Bind_SeparateUuidAndColor;
			break;
		default:
			CollectionBindFunction = BlueprintData::Bind_NoSeparation;
			break;
		}
	}

	BlueprintData::~BlueprintData()
	{
		for (Body* pBody : this->mCollections)
			delete pBody;
	}

	glm::vec3 BlueprintData::JsonToVec(const nlohmann::json& jObj)
	{
		if (jObj.is_object())
		{
			const auto& pos_x = Json::Get(jObj, "x");
			const auto& pos_y = Json::Get(jObj, "y");
			const auto& pos_z = Json::Get(jObj, "z");

			if (pos_x.is_number() && pos_y.is_number() && pos_z.is_number())
				return glm::vec3(pos_x.get<float>(), pos_y.get<float>(), pos_z.get<float>());
		}

		return glm::vec3(0.0f);
	}

	void BlueprintData::LoadChild(const nlohmann::json& bpChild)
	{
		if (!bpChild.is_object()) return;

		const auto& mUuid = Json::Get(bpChild, "shapeId");
		const auto& mColor = Json::Get(bpChild, "color");
		const auto& xAxis = Json::Get(bpChild, "xaxis");
		const auto& zAxis = Json::Get(bpChild, "zaxis");

		const auto& mPos = Json::Get(bpChild, "pos");
		const auto& mBounds = Json::Get(bpChild, "bounds");

		if (!mUuid.is_string() || !mColor.is_string()) return;

		AxisData mAxisData;
		mAxisData.x = (xAxis.is_number() ? xAxis.get<short>() : 1);
		mAxisData.z = (zAxis.is_number() ? zAxis.get<short>() : 3);

		const glm::vec3 pPosVec = BlueprintData::JsonToVec(mPos);
		SMBC::Uuid mObjUuid = mUuid.get<std::string>();
		SMBC::Color mObjColor = mColor.get<std::string>();

		if (mBounds.is_object())
		{
			const glm::vec3 mBlockBounds = BlueprintData::JsonToVec(mBounds);
			if (mBlockBounds.x <= 0.0f || mBlockBounds.y <= 0.0f || mBlockBounds.z <= 0.0f) return;

			const SMBC::BlockData* block_data = Mod::GetBlock(mObjUuid);
			if (!block_data) return;

			this->CollectionBindFunction(this, new Block(block_data, pPosVec, mBlockBounds, mAxisData, mObjColor, this->ObjectIndex));
		}
		else
		{
			const SMBC::PartData* part_data = Mod::GetPart(mObjUuid);
			if (!part_data) return;

			SMBC::Model* pModel = ModelStorage::LoadModel(part_data->Path);
			if (!pModel) return;

			this->CollectionBindFunction(this, new Part(part_data, pModel, pPosVec, mAxisData, mObjColor, this->ObjectIndex));
		}
	}

	void BlueprintData::LoadObjects(const nlohmann::json& bpJson)
	{
		const auto& bArray = SMBC::Json::Get(bpJson, "bodies");
		if (!bArray.is_array()) return;

		ConvData::SetState(ConvState::BP_ReadingObjects, 0);
		for (const auto& _Body : bArray)
		{
			const auto& _Childs = Json::Get(_Body, "childs");
			if (!_Childs.is_array()) continue;

			ConvData::ProgressMax += _Childs.size();
			for (const auto& _Child : _Childs)
			{
				this->LoadChild(_Child);
				
				this->ObjectIndex++;
				ConvData::ProgressValue++;
			}

			this->BodyIndex++;
		}
	}

	void BlueprintData::LoadJoint(const nlohmann::json& bpJoint)
	{
		if (!bpJoint.is_object()) return;

		const auto& jUuid     = Json::Get(bpJoint, "shapeId");
		const auto& jColor    = Json::Get(bpJoint, "color");
		const auto& xAxis     = Json::Get(bpJoint, "xaxisA");
		const auto& zAxis     = Json::Get(bpJoint, "zaxisA");
		const auto& jPosition = Json::Get(bpJoint, "posA");
		const auto& jChildA   = Json::Get(bpJoint, "childA");

		if (!jUuid.is_string() || !jColor.is_string() || !jChildA.is_number()) return;

		const SMBC::Uuid jObjUuid = jUuid.get<std::string>();

		const SMBC::PartData* joint_data = Mod::GetPart(jObjUuid);
		if (!joint_data) return;

		SMBC::Model* pModel = ModelStorage::LoadModel(joint_data->Path);
		if (!pModel) return;

		{
			const std::size_t jChildIdx = jChildA.get<std::size_t>();
			const glm::vec3 jPosVec = BlueprintData::JsonToVec(jPosition);
			const SMBC::Color jObjColor = jColor.get<std::string>();

			AxisData jAxisData;
			jAxisData.x = (xAxis.is_number() ? xAxis.get<short>() : 1);
			jAxisData.z = (zAxis.is_number() ? zAxis.get<short>() : 3);

			this->CollectionBindFunction(this, new Joint(joint_data, pModel, jPosVec, jAxisData, jObjColor, jChildIdx));
		}
	}

	void BlueprintData::LoadJoints(const nlohmann::json& bpJson)
	{
		const auto& jArray = Json::Get(bpJson, "joints");
		if (!jArray.is_array()) return;

		ConvData::SetState(ConvState::BP_ReadingJoints, jArray.size());
		for (const auto& _Joint : jArray)
		{
			this->LoadJoint(_Joint);

			this->ObjectIndex++;
			ConvData::ProgressValue++;
		}
	}

	std::size_t BlueprintData::GetAmountOfObjects() const
	{
		std::size_t obj_sum = 0;

		for (SMBC::Body* pEntity : this->mCollections)
			obj_sum += pEntity->GetAmountOfObjects();

		return obj_sum;
	}

	nlohmann::json BlueprintData::LoadAndCheckBlueprint(const std::wstring& path, ConvertError& cError)
	{
		ConvData::SetState(ConvState::BP_ReadingJson);
		nlohmann::json bpJson = Json::LoadParseJson(path);
		if (!bpJson.is_object())
		{
			cError = L"Couldn't read the specified blueprint file. Possible reasons: Invalid Json, Invalid Path";
			return {};
		}

		const bool has_bodies = (bpJson.find("bodies") != bpJson.end());
		const bool has_joints = (bpJson.find("joints") != bpJson.end());

		if (!has_bodies && !has_joints)
		{
			cError = L"The specified blueprint has no objects to convert!";
			return {};
		}

		return bpJson;
	}

	BlueprintData* BlueprintData::LoadFromFile(const std::wstring& path, ConvertError& cError)
	{
		nlohmann::json bpJson = BlueprintData::LoadAndCheckBlueprint(path, cError);
		if (cError) return nullptr;

		BlueprintData* new_bp_data = new BlueprintData();
		new_bp_data->SelectSeparationMethod();

		new_bp_data->LoadObjects(bpJson);
		new_bp_data->LoadJoints (bpJson);

		ConvData::SetState(ConvState::BP_ClearingJson, 0);
		return new_bp_data;
	}

	void BlueprintData::WriteMtlFile(const std::wstring& path) const
	{
		if (!ConvertSettings::ApplyTextures) return;

		ConvData::SetState(ConvState::BP_WritingMtl, 0);

		std::unordered_map<std::string, ObjectTextureData> tData;
		for (const SMBC::Entity* pEntity : this->mCollections)
			pEntity->FillTextureMap(tData);

		std::ofstream oMtl(path);
		if (!oMtl.is_open()) return;

		std::string color_data = "0.8 0.8 0.8";

		for (const auto& tDatum : tData)
		{
			std::string output_str = "newmtl " + tDatum.first;

			if (ConvertSettings::MatByColor)
				color_data = tDatum.second.mColor.StringNormalized();

			output_str.append("\nNs 324");
			output_str.append("\nKa 1 1 1\nKd ");
			output_str.append(color_data);
			output_str.append("\nKs 0.5 0.5 0.5");
			output_str.append("\nKe 0 0 0");
			output_str.append("\nNi 1.45");
			output_str.append("\nd 1");
			output_str.append("\nillum 2");

			{
				const Texture::TextureList& tList = tDatum.second.mTextures;

				if (!tList.nor.empty()) output_str.append("\nmap_Bump " + String::ToUtf8(tList.nor));
				if (!tList.dif.empty()) output_str.append("\nmap_Kd "   + String::ToUtf8(tList.dif));
				if (!tList.asg.empty()) output_str.append("\nmap_Ks "   + String::ToUtf8(tList.asg));
			}

			output_str.append("\n\n");

			oMtl.write(output_str.c_str(), output_str.size());

			ConvData::ProgressValue++;
		}
	}

	void BlueprintData::WriteTexPaths(const std::wstring& path) const
	{
		if (!ConvertSettings::TextureList) return;

		ConvData::SetState(ConvState::BP_WritingTextures, 0);

		nlohmann::json tList = nlohmann::json::object();
		for (const SMBC::Entity* pEntity : this->mCollections)
			pEntity->FillTextureJson(tList);

		Json::WriteToFile(path, tList);
	}

	std::wstring BlueprintData::PrepareForWrite(const std::wstring& name, ConvertError& cError) const
	{
		static const std::wstring OutputDirectory = L"./Converted Models";

		if (!File::SafeCreateDir(OutputDirectory))
		{
			cError = L"Couldn't create the main directory!";
			return L"";
		}

		const std::wstring model_dir = OutputDirectory + L"/" + name;

		{
			std::error_code ec;
			const bool exists_test = fs::exists(model_dir, ec);

			if (ec)
			{
				cError = L"Blueprint name is invalid!";
				return L"";
			}

			if (!File::SafeCreateDir(model_dir))
			{
				cError = L"Couldn't create a blueprint output directory!";
				return L"";
			}
		}

		return model_dir;
	}

	glm::vec3 BlueprintData::CalcCenterPoint() const
	{
		glm::vec3 pCenter(0.0f);

		for (Entity* pEntity : mCollections)
			pCenter += pEntity->GetCenterPoint();

		return pCenter / (float)mCollections.size();
	}

	void BlueprintData::WriteObjects(std::ofstream& mOutput) const
	{
		OffsetData mOffsetData;
		mOffsetData.PointOffset = this->CalcCenterPoint();

		const std::size_t object_count = this->GetAmountOfObjects();
		ConvData::SetState(ConvState::BP_WritingObjects, object_count);

		for (Entity* pEntity : this->mCollections)
			pEntity->WriteObjectToFile(mOutput, mOffsetData);
	}

	void BlueprintData::WriteToFile(const std::wstring& name, ConvertError& cError) const
	{
		const std::wstring model_dir = this->PrepareForWrite(name, cError);
		if (cError) return;

		std::ofstream output_stream(model_dir + L"/" + name + L".obj");
		if (!output_stream.is_open())
		{
			cError = L"Couldn't write the specified blueprint to a file!";
			return;
		}

		this->WriteObjects(output_stream);

		this->WriteMtlFile (model_dir + L"/" + name + L".mtl");
		this->WriteTexPaths(model_dir + L"/UsedTextures.json");
	}
}