#pragma once

#include <string>
#include <glm.hpp>

#include "Object Database/Texture Database/TextureDatabase.h"

namespace SMBC
{
	enum class ObjectType { None, Block, Part };
	
	class BlockData;
	class PartData;
	class Mod;

	class ObjectData
	{
	protected:
		ObjectType Type = ObjectType::None;
	public:
		std::wstring Uuid;
		std::wstring Name;
		Mod* ModPtr;

		const BlockData* ToBlock() const;
		const PartData*  ToPart()  const;
	};

	class BlockData : public ObjectData
	{
	public:
		SMBC::Texture::TextureList TextureList;
		int Tiling;

		BlockData(
			const std::wstring& uuid,
			const std::wstring& name,
			SMBC::Texture::TextureList& textures,
			const int& tiling
		);
	};

	class PartData : public ObjectData
	{
	public:
		SMBC::Texture::Texture TextureList;
		std::wstring Path;
		glm::vec3 Bounds;

		PartData(
			const std::wstring& uuid,
			const std::wstring& path,
			const std::wstring& name,
			SMBC::Texture::Texture& textures,
			const glm::vec3& bounds
		);
	};
}