#include "Block.h"

namespace SMBC
{
	Block::Block(
		const BlockData* pParent,
		const Body* pBody,
		const glm::vec3& position,
		const glm::vec3& size,
		const AxisData& mAxisData,
		const Color& mColor,
		const std::size_t& obj_idx)
	{
		this->pParent = pParent;
		this->pBody = pBody;
		this->mPosition = position;
		this->mScale = size;
		this->mAxis = mAxisData;
		this->mColor = mColor;
		this->mIndex = obj_idx;
	}

	EntityType Block::Type() const
	{
		return EntityType::Block;
	}

	std::string Block::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
	{
		return pParent->Uuid.ToString() + " " + mColor.StringHex() + " 1";
	}

	void Block::FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const
	{
		const std::string mtl_name = this->GetMtlName(L"", 0);

		if (tex_map.find(mtl_name) != tex_map.end())
			return;

		ObjectTextureData oTexData;
		oTexData.mTextures = pParent->TextureList;
		oTexData.mColor = this->mColor;

		tex_map.insert(std::make_pair(mtl_name, oTexData));
	}
}