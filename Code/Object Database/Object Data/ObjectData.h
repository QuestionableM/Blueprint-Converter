#pragma once

#include <string>
#include <glm.hpp>

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Lib/Uuid/Uuid.h"

namespace SMBC
{
	enum class ObjectType { Block, Part };
	
	class BlockData;
	class PartData;
	class Mod;

	class ObjectData
	{
	public:
		Uuid Uuid;
		std::wstring Name;
		Mod* ModPtr;

		virtual ObjectType Type() = 0;

		static const BlockData* ToBlock(ObjectData* data_ptr);
		static const PartData*  ToPart(ObjectData* data_ptr);
	};

	class BlockData : public ObjectData
	{
	public:
		SMBC::Texture::TextureList TextureList;
		int Tiling;

		ObjectType Type() override;

		BlockData(
			const SMBC::Uuid& uuid,
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

		ObjectType Type() override;

		PartData(
			const SMBC::Uuid& uuid,
			const std::wstring& path,
			const std::wstring& name,
			SMBC::Texture::Texture& textures,
			const glm::vec3& bounds
		);
	};
}