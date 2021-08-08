#pragma once

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Mod Data/ModData.h"
#include "Lib/Json/JsonFunc.h"
#include <glm.hpp>

namespace SMBC {
	class DatabaseLoader {
		static glm::vec3 LoadBlockCollision(const nlohmann::json& collision);

		static void GetTextureArray(
			const nlohmann::json& textureList,
			std::wstring& dif,
			std::wstring& asg,
			std::wstring& nor
		);
		static void GetRenderableData(
			const nlohmann::json& renderable,
			SMBC::Texture::Texture& texture,
			std::wstring& model_path
		);

	public:
		static void LoadObjectFile(
			SMBC::ModData* mod,
			const std::wstring& path,
			SMBC::LangDB& translations
		);
		static void LoadGameDatabase();
		static void LoadModDatabase();
	};
}