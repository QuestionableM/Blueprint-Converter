#pragma once

#include <string>
#include <unordered_map>
#include <vec3.hpp>
#include <nlohmann/json.hpp>

#include "Texture Database/TextureDatabase.h"
#include "Language Database/LanguageDatabase.h"
#include "Mod Data/ModData.h"

namespace SMBC {
	class ObjectDatabase {
	public:
		static std::unordered_map<std::wstring, SMBC::ModData*> ModDB;

		static SMBC::ObjectData* GetPart(const std::wstring& uuid);
		static SMBC::BlockData* GetBlock(const std::wstring& uuid);

		static std::size_t CountLoadedObjects();
		static void ClearDatabase();
		static void ReloadDatabase();
	};
};