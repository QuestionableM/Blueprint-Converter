#pragma once

#include "ObjectDatabase\TextureDatabase.hpp"
#include "Lib\Uuid.hpp"

#include <glm.hpp>
#include <string>

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

		inline ObjectType Type() const override { return ObjectType::Block; }

		inline BlockData(const SMBC::Uuid& uuid,
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

		~BlockData() = default;
	};

	class PartData : public ObjectData
	{
	public:
		SMBC::Texture::Texture TextureList;
		std::wstring Path;
		glm::vec3 Bounds;

		inline ObjectType Type() const override { return ObjectType::Part; }

		PartData(const SMBC::Uuid& uuid,
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

		~PartData() = default;
	};
}