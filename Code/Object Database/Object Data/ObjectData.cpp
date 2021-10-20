#include "ObjectData.h"

namespace SMBC
{
	const BlockData* ObjectData::ToBlock(ObjectData* data_ptr)
	{
		if (data_ptr->Type() != ObjectType::Block)
			return nullptr;

		return static_cast<const BlockData*>(data_ptr);
	}

	const PartData* ObjectData::ToPart(ObjectData* data_ptr)
	{
		if (data_ptr->Type() != ObjectType::Part)
			return nullptr;

		return static_cast<const PartData*>(data_ptr);
	}

	ObjectType BlockData::Type()
	{
		return ObjectType::Block;
	}

	BlockData::BlockData(
		const SMBC::Uuid& uuid,
		const std::wstring& name,
		SMBC::Texture::TextureList& textures,
		const int& tiling
	) {
		this->Uuid = uuid;
		this->Name = name;
		this->Tiling = tiling;
		this->TextureList = textures;
	}

	ObjectType PartData::Type()
	{
		return ObjectType::Part;
	}

	PartData::PartData(
		const SMBC::Uuid& uuid,
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
	}
}