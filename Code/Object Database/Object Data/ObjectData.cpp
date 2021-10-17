#include "ObjectData.h"

namespace SMBC
{
	const BlockData* ObjectData::ToBlock() const
	{
		if (this->Type != ObjectType::Block)
			return nullptr;

		return static_cast<const BlockData*>(this);
	}

	const PartData* ObjectData::ToPart() const
	{
		if (this->Type != ObjectType::Part)
			return nullptr;

		return static_cast<const PartData*>(this);
	}

	BlockData::BlockData(
		const std::wstring& uuid,
		const std::wstring& name,
		SMBC::Texture::TextureList& textures,
		const int& tiling
	) {
		this->Uuid = uuid;
		this->Name = name;
		this->Tiling = tiling;
		this->TextureList = textures;

		this->Type = ObjectType::Block;
	}

	PartData::PartData(
		const std::wstring& uuid,
		const std::wstring& path,
		const std::wstring& name,
		SMBC::Texture::Texture& textures,
		const glm::vec3& bounds
	) {
		this->Uuid = uuid;
		this->Path = path;
		this->Name = name;
		this->TextureList = textures;
		this->Bounds = bounds;

		this->Type = ObjectType::Part;
	}
}