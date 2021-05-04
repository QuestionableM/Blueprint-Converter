#pragma once

#include <assimp/Importer.hpp>

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Blueprint Converter/Object Definitions/ObjectDefinitions.h"

namespace SMBC {
	struct SubMeshCache {
		std::wstring _MeshName;
		std::wstring _TexPath;
		std::vector<std::vector<std::vector<long long>>> _DataIdx;
	};

	struct ModelDataChunk {
		std::wstring name;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> vertices;

		std::vector<SMBC::SubMeshCache> SubMeshCache;
	};

	struct CachedMesh {
		std::vector<glm::vec3> Vertices;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> TexturePoints;
		std::vector<SMBC::SubMeshCache> SubMeshCache;

		std::wstring MeshUuid;
		std::wstring MeshPath;
		std::wstring MeshName;
		SMBC::Texture::Texture TexPaths;

		~CachedMesh();
	};

	struct CachedBlock {
		std::wstring name;
		std::wstring uuid;
		SMBC::Texture::TextureList TexList;
	};

	class CacheManager {
		std::vector<SMBC::CachedBlock> CachedBlocks;
		std::vector<SMBC::CachedMesh> CachedParts;

		Assimp::Importer Importer;
	public:
		void LoadBlockIntoCache(SMBC::SM_Block& block);
		void LoadPartIntoCache(
			SMBC::SM_Part& part,
			SMBC::CachedMesh& _CachedMesh,
			const bool& load_uvs,
			const bool& load_normals
		);

		void ClearCachedMeshList();
		bool GetCachedMesh(const std::wstring& uuid, SMBC::CachedMesh& c_mesh);

		void WriteMtlFile(const std::wstring& path);
		void WriteTexturePaths(const std::wstring& path);
	};
}