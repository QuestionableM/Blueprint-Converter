#pragma once

#include <nlohmann\json.hpp>

namespace SMBC
{
	namespace Texture
	{
		struct TextureList;
	}

	class BlockListLoader
	{
		BlockListLoader()  = default;
		~BlockListLoader() = default;

		static void GetBlockMaterials(const nlohmann::json& block, Texture::TextureList& tex);
		static bool GetBlockTextures(const nlohmann::json& block, Texture::TextureList& tex);

	public:
		static void Load(const nlohmann::json& block_list, class Mod* pMod);
	};
}