#pragma once

#include <string>
#include <glm.hpp>

#include "ObjectDatabase\TextureDatabase.h"

#include "Lib\Uuid.h"

namespace SMBC
{
	enum class ObjectType
	{
		Block,
		Part
	};
	
	class BlockData;
	class PartData;
	class Mod;

	class ObjectData
	{
	public:
		Uuid Uuid;
		std::wstring Name;
		Mod* ModPtr;

		virtual ObjectType Type() const = 0;
		virtual ~ObjectData() = default;
	};

	class BlockData : public ObjectData
	{
	public:
		SMBC::Texture::TextureList TextureList;
		int Tiling;

		ObjectType Type() const override;

		BlockData(
			const SMBC::Uuid& uuid,
			const std::wstring& name,
			SMBC::Texture::TextureList& textures,
			const int& tiling,
			Mod* pMod
		);
		~BlockData() = default;
	};

	class PartData : public ObjectData
	{
	public:
		SMBC::Texture::Texture TextureList;
		std::wstring Path;
		glm::vec3 Bounds;

		ObjectType Type() const override;

		PartData(
			const SMBC::Uuid& uuid,
			const std::wstring& path,
			const std::wstring& name,
			SMBC::Texture::Texture& textures,
			const glm::vec3& bounds,
			Mod* pMod
		);
		~PartData() = default;
	};
}