#pragma once

#include <assimp/Importer.hpp>

#include "Object Database/Texture Database/TextureDatabase.h"
//#include "Blueprint Converter/Object Definitions/ObjectDefinitions.h"
#include "Blueprint Converter/Object Definitions/Object/Object.h"
#include "Blueprint Converter/Cache Manager/Cache Object/CacheObject.h"

#include <unordered_map>

namespace SMBC
{
	class ModelStorage
	{
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

	class ObjectStorage
	{
		static std::unordered_map<std::wstring, SMBC::CachedObject*> CachedObjects;

		static CachedObject* LoadCachedBlock(SMBC::Block* block);
		static CachedObject* LoadCachedPart(SMBC::Part* part);
	public:
		static void LoadObject(SMBC::Object* object);

		static void WriteMtlFile(const std::wstring& path);
		static void WriteTexturePaths(const std::wstring& path);

		static void ClearStorage();
	};
}