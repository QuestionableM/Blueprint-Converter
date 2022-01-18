#include "BlueprintData.h"

#include "Lib/Json/JsonFunc.h"
#include "Lib/ConvData/ConvData.h"
#include "Lib/File/FileFunc.h"

#include "Blueprint Converter/Convert Settings/ConvertSettings.h"
#include "Blueprint Converter/Cache Manager/Model Storage/ModelStorage.h"

#include "Blueprint Converter/Object Definitions/Entity/Part/Part.h"
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

	void BlueprintData::LoadObjects(const nlohmann::json& bpJson)
	{
		const auto& bArray = SMBC::Json::Get(bpJson, "bodies");
		if (!bArray.is_array()) return;

		std::size_t body_index = 0;

		ConvData::SetState(State::GettingObjects);
		for (auto& _Body : bArray)
		{
			const auto& _Childs = Json::Get(_Body, "childs");
			if (!_Childs.is_array()) continue;

			SMBC::Body* new_body = new SMBC::Body(body_index);

			ConvData::ProgressMax += _Childs.size();
			for (auto& _Child : _Childs)
			{
				const auto& _ShapeId = Json::Get(_Child, "shapeId");
				const auto& _Pos = Json::Get(_Child, "pos");
				const auto& _XAxis = Json::Get(_Child, "xaxis");
				const auto& _ZAxis = Json::Get(_Child, "zaxis");
				const auto& _Bounds = Json::Get(_Child, "bounds");
				const auto& _Color = Json::Get(_Child, "color");

				if (!(_ShapeId.is_string() && _Pos.is_object() && _XAxis.is_number() && _ZAxis.is_number())) continue;
				const auto& _PosX = Json::Get(_Pos, "x");
				const auto& _PosY = Json::Get(_Pos, "y");
				const auto& _PosZ = Json::Get(_Pos, "z");

				if (!(_PosX.is_number() && _PosY.is_number() && _PosZ.is_number())) continue;
				glm::vec3 _PosVec(_PosX.get<float>(), _PosY.get<float>(), _PosZ.get<float>());

				SMBC::Uuid uuid_obj(_ShapeId.get<std::string>());
				std::string color_str = (_Color.is_string() ? _Color.get<std::string>() : "000000");
				AxisData mAxisData = { _XAxis.get<short>(), _ZAxis.get<short>() };

				if (_Bounds.is_object())
				{
					const auto& _BoundX = Json::Get(_Bounds, "x");
					const auto& _BoundY = Json::Get(_Bounds, "y");
					const auto& _BoundZ = Json::Get(_Bounds, "z");

					if (!(_BoundX.is_number() && _BoundY.is_number() && _BoundZ.is_number())) continue;
					glm::vec3 _BoundsVec(_BoundX.get<float>(), _BoundY.get<float>(), _BoundZ.get<float>());

					const SMBC::BlockData* _BlockD = Mod::GetBlock(uuid_obj);
					if (!_BlockD) continue;

					
					/*SMBC::Block* _Block = new SMBC::Block();
					_Block->blkPtr = (SMBC::BlockData*)_BlockD;
					_Block->Bounds = _BoundsVec;
					_Block->Color = color_str;
					_Block->Position = _PosVec;
					_Block->Uuid = _BlockD->Uuid;
					_Block->xAxis = _XAxis.get<int>();
					_Block->zAxis = _ZAxis.get<int>();
					_Block->ObjectIndex = this->objectIndexValue;

					this->CollectionBindFunction(*this, _Block, false, body_index);*/
				}
				else
				{
					const SMBC::PartData* part_data = SMBC::Mod::GetPart(uuid_obj);
					if (!part_data) continue;

					SMBC::Model* pModel = ModelStorage::LoadModel(part_data->Path);
					if (!pModel) continue;

					new_body->Add(new SMBC::Part(part_data, pModel, new_body, _PosVec, mAxisData, color_str, this->ObjectIndex));
				}
				
				this->ObjectIndex++;
				ConvData::ProgressValue++;
			}

			body_index++;
			this->Objects.push_back(new_body);
		}
	}

	void BlueprintData::LoadJoints(const nlohmann::json& bpJson)
	{

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

	void BlueprintData::WriteToFile(const std::wstring& name, ConvertError& cError) const
	{
		static const std::wstring OutputDirectory = L"./Converted Models";

		if (!File::SafeCreateDir(OutputDirectory))
		{
			cError = L"Couldn't create the main directory!";
			return;
		}
		
		const std::wstring model_dir = OutputDirectory + L"/" + name;

		{
			std::error_code ec;
			const bool exists_test = fs::exists(model_dir, ec);

			if (ec)
			{
				cError = L"Blueprint name is invalid!";
				return;
			}
			else
			{
				if (!File::SafeCreateDir(model_dir))
				{
					cError = L"Couldn't create a blueprint output directory!";
					return;
				}
			}
		}

		const std::wstring model_path = model_dir + L"/" + name + L".obj";

		DebugOutL("ModelPath: ", model_path);

		std::ofstream output_stream(model_path);
		if (!output_stream.is_open())
		{
			cError = L"Couldn't write the specified blueprint to a file!";
			return;
		}

		OffsetData mOffsetData;

		const std::size_t object_count = this->GetAmountOfObjects();
		SMBC::ConvData::SetState(SMBC::State::WritingObjects, object_count);

		for (SMBC::Entity* pEntity : this->Objects)
			pEntity->WriteObjectToFile(output_stream, mOffsetData);

		DebugOutL("Finished loading models to file!");
	}
}