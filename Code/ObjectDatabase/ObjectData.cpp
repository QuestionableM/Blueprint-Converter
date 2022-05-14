#include "ObjectData.h"

namespace SMBC
{
	ObjectType BlockData::Type() const
	{
		return ObjectType::Block;
	}

	BlockData::BlockData(
		const SMBC::Uuid& uuid,
		const std::wstring& name,
		SMBC::Texture::TextureList& textures,
		const int& tiling,
		Mod* pMod)
	{
		this->Uuid = uuid;
		this->Name = name;
		this->Tiling = tiling;
		this->TextureList = textures;
		this->ModPtr = pMod;
	}

	ObjectType PartData::Type() const
	{
		return ObjectType::Part;
	}

	PartData::PartData(
		const SMBC::Uuid& uuid,
		const std::wstring& path,
		const std::wstring& name,
		SMBC::Texture::Texture& textures,
		const glm::vec3& bounds,
		Mod* pMod)
	{
		this->Uuid = uuid;
		this->Path = path;
		this->Name = name;
		this->TextureList = textures;
		this->Bounds = bounds;
		this->ModPtr = pMod;
	}
}