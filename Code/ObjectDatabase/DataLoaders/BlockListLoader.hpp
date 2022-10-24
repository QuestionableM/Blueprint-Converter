#pragma once

#include <nlohmann\json.hpp>

namespace SMBC
{
	namespace Texture
	{
		struct TextureData;
	}

	class BlockListLoader
	{
		BlockListLoader()  = default;
		~BlockListLoader() = default;

		static void GetBlockMaterials(const nlohmann::json& block, Texture::TextureData* tex);
		static void GetBlockTextures(const nlohmann::json& block, Texture::TextureData* tex);

	public:
		static void Load(const nlohmann::json& block_list, class Mod* pMod);
	};
}