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
		Uuid m_uuid;
		std::wstring m_name;
		Mod* m_modPtr;

		virtual ObjectType Type() const = 0;

		ObjectData() = default;
		virtual ~ObjectData() = default;
	};

	class BlockData : public ObjectData
	{
	public:
		SMBC::Texture::TextureData* m_textureList;
		int m_tiling;

		inline ObjectType Type() const override { return ObjectType::Block; }

		inline BlockData(const SMBC::Uuid& uuid,
			const std::wstring& name,
			SMBC::Texture::TextureData* textures,
			const int& tiling,
			Mod* pMod)
		{
			this->m_uuid = uuid;
			this->m_name = name;
			this->m_tiling = tiling;
			this->m_textureList = textures;
			this->m_modPtr = pMod;
		}

		inline ~BlockData() { delete m_textureList; }
	};

	class PartData : public ObjectData
	{
	public:
		SMBC::Texture::SubMeshDataBase* m_textureList;
		std::wstring m_path;
		glm::vec3 m_bounds;

		inline ObjectType Type() const override { return ObjectType::Part; }

		PartData(const SMBC::Uuid& uuid,
			const std::wstring& path,
			const std::wstring& name,
			SMBC::Texture::SubMeshDataBase* textures,
			const glm::vec3& bounds,
			Mod* pMod)
		{
			this->m_uuid = uuid;
			this->m_path = path;
			this->m_name = name;
			this->m_textureList = textures;
			this->m_bounds = bounds;
			this->m_modPtr = pMod;
		}

		inline ~PartData() { delete m_textureList; }
	};
}