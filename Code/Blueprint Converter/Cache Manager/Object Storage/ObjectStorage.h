#pragma once

#include "Blueprint Converter/Cache Manager/Cache Object/CacheObject.h"
#include "Blueprint Converter/Object Definitions/Object/Object.h"

#include <unordered_map>
#include <string>

namespace SMBC
{
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