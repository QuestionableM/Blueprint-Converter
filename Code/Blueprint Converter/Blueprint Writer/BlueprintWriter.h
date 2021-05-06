#pragma once

#include <vector>
#include "Blueprint Converter/BlueprintConverter.h"
#include "Blueprint Converter/Cache Manager/CacheManager.h"

namespace SMBC {
	struct ConvertedModel {
		struct ConvertedModelData {
			bool tex_list;
			bool apply_texture;
			bool export_uvs;
			bool export_normals;
			bool separate_parts;
			bool mat_by_color;
		};

		std::wstring ModelName;
		std::vector<SMBC::SM_Part> PartList;
		std::vector<SMBC::SM_Block> BlockList;

		ConvertedModel(SMBC::ConvertedModel::ConvertedModelData& cm_data);

	private:
		ConvertedModelData conv_data;
		SMBC::CacheManager CacheManager;
		Assimp::Importer Importer;

		struct OffsetData {
			long long Vertex = 0ll;
			long long Normal = 0ll;
			long long Texture = 0ll;
		};

		void LoadBlueprintBlocks(
			std::vector<SMBC::Model>& Models,
			SMBC::Model& NewModel,
			SMBC::SubMeshData& NewSubMeshData,
			OffsetData& o_data
		);
		void LoadBlueprintParts(
			std::vector<SMBC::Model>& Models,
			SMBC::Model& NewModel,
			SMBC::SubMeshData& NewSubMeshData,
			OffsetData& o_data
		);

	public:
		bool WriteChunksToFile(std::vector<SMBC::Model>& models);
		int ConvertAndWrite();
	};
}