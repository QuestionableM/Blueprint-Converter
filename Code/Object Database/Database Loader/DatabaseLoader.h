#pragma once

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Mod Data/ModData.h"
#include "Lib/Json/JsonFunc.h"
#include <glm.hpp>

namespace SMBC {
	class DatabaseLoader {
		static void LoadGameDatabase();
		static void LoadModDatabase();
		static void FullCleanup();
	public:
		static void LoadDatabase();
	};
}