#pragma once

#include <vector>
#include <glm.hpp>
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

		bool AddJointToChildShape(SMBC::SM_Part& joint);
		bool HasStuffToConvert();

		ConvertedModel(SMBC::ConvertedModel::ConvertedModelData& cm_data);

	private:
		ConvertedModelData conv_data;
		SMBC::CacheManager CacheManager;
		Assimp::Importer Importer;

		struct OffsetData {
			long long Vertex = 0ll;
			long long Normal = 0ll;
			long long Texture = 0ll;

			glm::vec3 pt_sum = { 0.0f, 0.0f, 0.0f };
			long long point_count = 0;

			void UpdateValues(const glm::vec3& val);
		};

		std::vector<SMBC::Model> Models;
		std::vector<SMBC::ObjectCollection> Collections;
		SMBC::Model NewModel;
		SMBC::SubMeshData NewSubMeshData;
		OffsetData offsetData;

		void LoadBlueprintBlocks(SMBC::ObjectCollection& collection);
		void LoadBlueprintParts(SMBC::ObjectCollection& collection);

	public:
		bool WriteChunksToFile(std::vector<SMBC::Model>& models, glm::vec3& posOffset);
		int ConvertAndWrite();
	};
}