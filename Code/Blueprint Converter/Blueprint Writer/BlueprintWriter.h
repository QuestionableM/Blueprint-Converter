#pragma once

#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Blueprint Converter/BlueprintConverter.h"
#include "Blueprint Converter/Cache Manager/CacheManager.h"

#include "Lib/ConvData/ConvData.h"

namespace SMBC
{
	struct ObjectCollection
	{
		std::vector<SMBC::Object*> ObjectList = {};

		bool IsEmpty();
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
		std::vector<SMBC::ObjectCollection> ObjCollection;

		bool IsLastCollectionEmpty();
		std::size_t CreateNewCollection();
		std::size_t HasUuidCollection(const SMBC::Uuid& uuid, const std::wstring& color, const bool& useColor);
		bool AddJointToChildShape(SMBC::Part& joint);
		bool HasStuffToConvert();

		ConvertedModel() = default;

	private:
		OffsetData offsetData;
		const glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		std::size_t CountTotalObjectAmount();
		void LoadCollections(const std::size_t& total_obj_count);

		SMBC::Error WriteBlueprintToFile(const std::size_t& objectCount);

	public:
		SMBC::Error LoadBlueprintData(const std::wstring& blueprint_path);
		SMBC::Error ConvertAndWrite();
	};
}