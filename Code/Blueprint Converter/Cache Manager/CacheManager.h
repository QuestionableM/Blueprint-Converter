#pragma once

#include <assimp/Importer.hpp>

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Blueprint Converter/Object Definitions/ObjectDefinitions.h"

namespace SMBC {
	struct SubMeshData {
		std::wstring MaterialName;
		int SubMeshIndex;

		std::vector<std::vector<std::vector<long long>>> DataIdx;

		bool is_empty();
	};

	struct Model {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<SMBC::SubMeshData> subMeshData;

		std::wstring meshPath;

		bool is_empty();
	};

	struct CachedBlock {
		std::wstring uuid;
		std::wstring name;
		std::wstring color;
		SMBC::Texture::TextureList texList;
	};

	struct CachedPart {
		std::wstring uuid;
		std::wstring name;
		std::wstring color;
		std::wstring meshPath;

		SMBC::Texture::Texture texPaths;
	};

	class CacheManager {
		std::vector<SMBC::Model> CachedModels;
		std::vector<SMBC::CachedBlock> CachedBlocks;
		std::vector<SMBC::CachedPart> CachedParts;

		Assimp::Importer Importer;
	public:
		bool LoadModel(
			std::wstring& path,
			SMBC::Model& model,
			const bool& load_uvs,
			const bool& load_normals
		);
		void LoadPart(
			SMBC::SM_Part& part,
			SMBC::CachedPart& cached_part,
			const bool& load_uvs,
			const bool& load_normal,
			const bool& mat_by_color
		);
		void LoadBlock(SMBC::SM_Block& block, const bool& mat_by_color);

		bool GetModel(const std::wstring& path, SMBC::Model& model);

		void ClearCache();

		void WriteMtlFile(const std::wstring& path);
		void WriteTexturePaths(const std::wstring& path);
	};
}