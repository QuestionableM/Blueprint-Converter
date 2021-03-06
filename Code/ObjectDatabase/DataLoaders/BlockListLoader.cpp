#include "BlockListLoader.h"

#include "ObjectDatabase\ModData.h"
#include "ObjectDatabase\KeywordReplacer.h"

#include "Lib\ConvData.h"
#include "Lib\String.h"
#include "Lib\Json.h"

#include "DebugCon.h"

namespace SMBC
{
	void BlockListLoader::GetBlockMaterials(const nlohmann::json& block, Texture::TextureList& tex)
	{
		const auto& bGlass = Json::Get(block, "glass");
		const auto& bAlpha = Json::Get(block, "alpha");

		if (bGlass.is_boolean() && bGlass.get<bool>())
		{
			tex.material = L"BlockGlass";
		}
		else
		{
			tex.material = L"BlockDifAsgNor";

			if (bAlpha.is_boolean() && bAlpha.get<bool>())
				tex.material.append(L"Alpha");
		}
	}

	static const std::string blkTexNames[3] = { "dif", "asg", "nor" };
	bool BlockListLoader::GetBlockTextures(const nlohmann::json& block, Texture::TextureList& tex)
	{
		for (int a = 0; a < 3; a++)
		{
			const auto& bTexture = Json::Get(block, blkTexNames[a]);

			if (bTexture.is_string())
			{
				std::wstring& strRef = tex.GetStringRef(a);

				strRef = String::ToWide(bTexture.get<std::string>());
				strRef = PathReplacer::ReplaceKey(strRef);
			}
		}

		return tex.HasTextures();
	}

	void BlockListLoader::Load(const nlohmann::json& block_list, Mod* pMod)
	{
		ConvData::ProgressMax += block_list.size();
		for (const auto& l_block : block_list)
		{
			if (!l_block.is_object()) continue;

			const auto& b_uuid = Json::Get(l_block, "uuid");
			if (!b_uuid.is_string()) continue;

			const auto& b_tiling = Json::Get(l_block, "tiling");
			const auto& b_color  = Json::Get(l_block, "color");

			SMBC::Uuid uuid_obj(b_uuid.get<std::string>());
			if (Mod::AllObjects.find(uuid_obj) != Mod::AllObjects.end())
			{
				DebugWarningL("An object with this uuid already exists! (", uuid_obj.ToString(), ")");
				continue;
			}

			Texture::TextureList tex_list;
			if (!BlockListLoader::GetBlockTextures(l_block, tex_list))
				continue;

			BlockListLoader::GetBlockMaterials(l_block, tex_list);

			int tiling_val = (b_tiling.is_number() ? b_tiling.get<int>() : 4);
			if (tiling_val > 16 || tiling_val <= 0) tiling_val = 4;

			const std::wstring l_NameWstr = pMod->m_LanguageDb.GetTranslation(uuid_obj);

			BlockData* p_new_blk = new BlockData(uuid_obj, l_NameWstr, tex_list, tiling_val, pMod);

			const auto new_pair = std::make_pair(p_new_blk->Uuid, p_new_blk);
			Mod::AllObjects.insert(new_pair);
			pMod->m_Objects.insert(new_pair);

			ConvData::ProgressValue++;
		}
	}
}