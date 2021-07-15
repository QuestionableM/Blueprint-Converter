#pragma once

#include <string>
#include <vector>
#include <vec3.hpp>
#include <nlohmann/json.hpp>

#include "Texture Database/TextureDatabase.h"
#include "Language Database/LanguageDatabase.h"
#include "Mod Data/ModData.h"

namespace SMBC {
	class ObjectDatabase {
	public:
		static std::vector<SMBC::ModData*> ModDB;

		static SMBC::ObjectData* GetPart(const std::wstring& uuid);
		static SMBC::BlockData* GetBlock(const std::wstring& uuid);

		static long long CountLoadedObjects();
		static void ClearDatabase();
		static void ReloadDatabase();
	};
};