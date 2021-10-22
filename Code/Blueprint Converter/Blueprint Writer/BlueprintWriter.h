#pragma once

#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Blueprint Converter/BlueprintConverter.h"
#include "Blueprint Converter/Object Definitions/Object/Object.h"

#include "Lib/ConvData/ConvData.h"

namespace SMBC
{
	struct ObjectCollection
	{
		std::vector<SMBC::Object*> ObjectList = {};

		bool IsEmpty();
		~ObjectCollection();
	};

	struct OffsetData
	{
		std::size_t Vertex = 0ll;
		std::size_t Normal = 0ll;
		std::size_t Texture = 0ll;

		glm::vec3 pt_sum = { 0.0f, 0.0f, 0.0f };
		std::size_t point_count = 0;

		void UpdateValues(const glm::vec3& val);
	};

	struct ConvertedModel
	{
		std::wstring ModelName;
		std::vector<SMBC::ObjectCollection*> ObjCollection = {};
		std::unordered_map<std::wstring, SMBC::ObjectCollection*> ObjCollectionMap = {};

		bool HasStuffToConvert();

		ConvertedModel() = default;
		~ConvertedModel();

	private:
		OffsetData offsetData;
		const glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		std::size_t CountTotalObjectAmount();
		void LoadCollections(const std::size_t& total_obj_count);

		void OpenWriter(const std::wstring& path, std::ofstream& out);
		void CreateAndOpenWriter(std::ofstream& out);
		void WriteMtlHeader(std::ofstream& out);
		void WriteCollectionHeader(std::ofstream& out, const std::size_t& idx, const bool& should_write);
		void WriteCollectionToFile(std::ofstream& out, const std::vector<SMBC::Object*>& obj_vec, const glm::vec3& offsetVec);
		SMBC::Error WriteBlueprintToFile(const std::size_t& objectCount);

		static nlohmann::json ReadAndCheckBlueprintFile(const std::wstring& path);
		static void Bind_NoSeparation(ConvertedModel& cModel, SMBC::Object* object, const bool& is_joint);
		static void Bind_SeparateJoints(ConvertedModel& cModel, SMBC::Object* object, const bool& is_joint);
		static void Bind_SeparateUuid(ConvertedModel& cModel, SMBC::Object* object, const bool& is_joint);
		static void Bind_SeparateUuidAndColor(ConvertedModel& cModel, SMBC::Object* object, const bool& is_joint);

		void CreateAndAddObjectToCollection(const std::wstring& col_name, SMBC::Object* object);
		void LoadBlueprintBodies(const nlohmann::json& bpJson);
		void LoadBlueprintJoints(const nlohmann::json& bpJson);

		std::size_t objectIndexValue = 0;

		void (*CollectionBindFunction)(ConvertedModel&, SMBC::Object*, const bool&) = nullptr;

		void SelectCollectionBinding();
	public:
		SMBC::Error LoadBlueprintData(const std::wstring& blueprint_path);
		SMBC::Error ConvertAndWrite();
	};
}