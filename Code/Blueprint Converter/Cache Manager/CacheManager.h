#pragma once

#include <assimp/Importer.hpp>

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Blueprint Converter/Object Definitions/ObjectDefinitions.h"
#include "Lib/Uuid/Uuid.h"

#include <unordered_map>

namespace SMBC {
	class ModelStorage {
		static std::unordered_map<std::wstring, SMBC::Model*> CachedModels;
		static Assimp::Importer Importer;

	public:
		static SMBC::Model* LoadModel(
			const std::wstring& path,
			const bool& load_uvs,
			const bool& load_normals
		);

		static void ClearStorage();
	};

	class ObjectStorage {
		static std::unordered_map<std::wstring, SMBC::CachedBlock*> CachedBlocks;
		static std::unordered_map<std::wstring, SMBC::CachedPart*> CachedParts;

	public:
		static SMBC::CachedBlock* LoadBlock(SMBC::SM_Block& block, const bool& mat_by_color);
		static SMBC::CachedPart* LoadPart(
			SMBC::SM_Part& part,
			const bool& load_uvs,
			const bool& load_normals,
			const bool& mat_by_color
		);

		static void WriteMtlFile(const std::wstring& path, const bool& add_colors);
		static void WriteTexturePaths(const std::wstring& path);

		static void ClearStorage();
	};
}