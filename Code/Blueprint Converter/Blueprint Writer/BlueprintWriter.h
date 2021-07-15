#pragma once

#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Blueprint Converter/BlueprintConverter.h"
#include "Blueprint Converter/Cache Manager/CacheManager.h"

namespace SMBC {
	struct ObjectCollection {
		std::vector<SMBC::SM_Part> PartList;
		std::vector<SMBC::SM_Block> BlockList;

		bool is_empty();
	};

	struct ConvertedModel {
		struct ConvertedModelData {
			bool tex_list;
			bool apply_texture;
			bool export_uvs;
			bool export_normals;
			bool mat_by_color;

			int separation_method;
		};

		std::wstring ModelName;
		std::vector<SMBC::ObjectCollection> ObjCollection;

		bool IsLastCollectionEmpty();
		std::size_t CreateNewCollection();
		std::size_t HasUuidCollection(const std::wstring& uuid, const std::wstring& color, const bool& useColor);
		bool AddJointToChildShape(SMBC::SM_Part& joint);
		bool HasStuffToConvert();

		ConvertedModel(SMBC::ConvertedModel::ConvertedModelData& cm_data);

	private:
		ConvertedModelData conv_data;

		struct OffsetData {
			long long Vertex = 0ll;
			long long Normal = 0ll;
			long long Texture = 0ll;

			glm::vec3 pt_sum = { 0.0f, 0.0f, 0.0f };
			long long point_count = 0;

			void UpdateValues(const glm::vec3& val);
		};

		OffsetData offsetData;
		const glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		void LoadBlueprintBlocks(SMBC::ObjectCollection& collection);
		void LoadBlueprintParts(SMBC::ObjectCollection& collection);

		int WriteBlueprintToFile(const long& objectCount);

	public:
		int LoadBlueprintData(const std::wstring& blueprint_path);
		int ConvertAndWrite();
	};
}