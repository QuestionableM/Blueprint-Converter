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
		static std::vector<SMBC::ModData> ModDB;

		static bool GetPart(const std::wstring& uuid, SMBC::ObjectData& object);
		static bool GetBlock(const std::wstring& uuid, SMBC::BlockData& block);

		static long long CountLoadedObjects();
		static void ReloadDatabase();
	};
};