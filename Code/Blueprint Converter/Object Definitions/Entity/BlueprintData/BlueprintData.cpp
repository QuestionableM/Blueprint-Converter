#include "BlueprintData.h"

#include "Lib/Json/JsonFunc.h"
#include "Lib/ConvData/ConvData.h"
#include "Lib/File/FileFunc.h"
#include "Lib/String/String.h"

#include "Blueprint Converter/Convert Settings/ConvertSettings.h"
#include "Blueprint Converter/Model Storage/ModelStorage.h"

#include "Blueprint Converter/Object Definitions/Entity/Joint/Joint.h"
#include "Blueprint Converter/Object Definitions/Entity/Part/Part.h"
#include "Blueprint Converter/Object Definitions/Entity/Block/Block.h"
#include "Blueprint Converter/Object Definitions/Entity/Body/Body.h"
#include "Blueprint Converter/OffsetData/OffsetData.h"

#include "DebugCon.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace SMBC
{
	BlueprintData::~BlueprintData()
	{
		for (Entity* pEntity : this->Objects)
			delete pEntity;
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

	void BlueprintData::LoadChild(const nlohmann::json& bpChild, Body* pBody, const std::size_t& obj_idx)
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

			pBody->Add(new SMBC::Block(block_data, pBody, pPosVec, mBlockBounds, mAxisData, mObjColor, obj_idx));
		}
		else
		{
			const SMBC::PartData* part_data = Mod::GetPart(mObjUuid);
			if (!part_data) return;

			SMBC::Model* pModel = ModelStorage::LoadModel(part_data->Path);
			if (!pModel) return;

			pBody->Add(new SMBC::Part(part_data, pModel, pBody, pPosVec, mAxisData, mObjColor, obj_idx));
		}
	}

	void BlueprintData::LoadObjects(const nlohmann::json& bpJson)
	{
		const auto& bArray = SMBC::Json::Get(bpJson, "bodies");
		if (!bArray.is_array()) return;

		ConvData::SetState(State::GettingObjects);
		for (const auto& _Body : bArray)
		{
			const auto& _Childs = Json::Get(_Body, "childs");
			if (!_Childs.is_array()) continue;

			SMBC::Body* new_body = new SMBC::Body(this->Objects.size());

			ConvData::ProgressMax += _Childs.size();
			for (const auto& _Child : _Childs)
			{
				BlueprintData::LoadChild(_Child, new_body, this->ObjectIndex);
				
				this->ObjectIndex++;
				ConvData::ProgressValue++;
			}

			this->Objects.push_back(new_body);
		}
	}

	void BlueprintData::LoadJoint(const nlohmann::json& bpJoint, Body* pBody)
	{
		if (!bpJoint.is_object()) return;

		const auto& jUuid     = Json::Get(bpJoint, "shapeId");
		const auto& jColor    = Json::Get(bpJoint, "color");
		const auto& xAxis     = Json::Get(bpJoint, "xaxisA");
		const auto& zAxis     = Json::Get(bpJoint, "zaxisA");
		const auto& jPosition = Json::Get(bpJoint, "posA");
		const auto& jChildA   = Json::Get(bpJoint, "childA");

		if (!jUuid.is_string() || !jColor.is_string()) return;

		const SMBC::Uuid jObjUuid = jUuid.get<std::string>();

		const SMBC::PartData* joint_data = Mod::GetPart(jObjUuid);
		if (!joint_data) return;

		SMBC::Model* pModel = ModelStorage::LoadModel(joint_data->Path);
		if (!pModel) return;

		{
			const glm::vec3 jPosVec = BlueprintData::JsonToVec(jPosition);
			const SMBC::Color jObjColor = jColor.get<std::string>();

			AxisData jAxisData;
			jAxisData.x = (xAxis.is_number() ? xAxis.get<short>() : 1);
			jAxisData.z = (zAxis.is_number() ? zAxis.get<short>() : 3);

			pBody->Add(new SMBC::Joint(joint_data, pBody, pModel, jPosVec, jAxisData, jObjColor));
		}
	}

	void BlueprintData::LoadJoints(const nlohmann::json& bpJson)
	{
		const auto& jArray = Json::Get(bpJson, "joints");
		if (!jArray.is_array()) return;

		SMBC::Body* joint_body = new SMBC::Body(this->Objects.size());

		ConvData::SetState(State::GettingJoints, jArray.size());
		for (const auto& _Joint : jArray)
		{
			BlueprintData::LoadJoint(_Joint, joint_body);

			ConvData::ProgressValue++;
		}

		this->Objects.push_back(joint_body);
	}

	std::size_t BlueprintData::GetAmountOfObjects() const
	{
		std::size_t obj_sum = 0;

		for (SMBC::Entity* pEntity : this->Objects)
			obj_sum += pEntity->GetAmountOfObjects();

		return obj_sum;
	}

	nlohmann::json BlueprintData::LoadAndCheckBlueprint(const std::wstring& path, ConvertError& cError)
	{
		SMBC::ConvData::SetState(SMBC::State::ReadingJson);
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
		new_bp_data->LoadObjects(bpJson);
		new_bp_data->LoadJoints (bpJson);

		return new_bp_data;
	}

	void BlueprintData::WriteMtlFile(const std::wstring& path) const
	{
		if (!ConvertSettings::ApplyTextures) return;

		std::unordered_map<std::string, ObjectTextureData> tData;
		for (const SMBC::Entity* pEntity : this->Objects)
		{
			pEntity->FillTextureMap(tData);

			ConvData::ProgressMax = tData.size();
		}

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

		nlohmann::json tList = nlohmann::json::object();
		for (const SMBC::Entity* pEntity : this->Objects)
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

	void BlueprintData::WriteObjects(std::ofstream& mOutput) const
	{
		OffsetData mOffsetData;

		const std::size_t object_count = this->GetAmountOfObjects();
		SMBC::ConvData::SetState(SMBC::State::WritingObjects, object_count);

		for (SMBC::Entity* pEntity : this->Objects)
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