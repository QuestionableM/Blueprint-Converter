#pragma once

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Mod Data/ModData.h"
#include "Lib/Json/JsonFunc.h"
#include <glm.hpp>

namespace SMBC {
	class DatabaseLoader {
		static void LoadGameDatabase();
		static void LoadModDatabase();
	public:
		static void LoadDatabase();
	};
}