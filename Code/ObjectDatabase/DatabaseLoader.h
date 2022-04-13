#pragma once

#include "ObjectDatabase\TextureDatabase.h"
#include "ObjectDatabase\ModData.h"

#include "Lib\JsonFunc.h"

#include <glm.hpp>

namespace SMBC {
	class DatabaseLoader {
		static void LoadGameDatabase();
		static void LoadModDatabase();
	public:
		static void LoadDatabase();
	};
}