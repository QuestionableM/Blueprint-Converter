#pragma once

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Language Database/LanguageDatabase.h"

#include <string>
#include <glm.hpp>

#include <unordered_map>

namespace SMBC {
	struct ObjectData {
		std::wstring uuid;
		std::wstring name;
		std::wstring path;
		SMBC::Texture::Texture texList;
		glm::vec3 bounds;

		ObjectData(
			const std::wstring& obj_uuid,
			const std::wstring& obj_path,
			const std::wstring& obj_name,
			SMBC::Texture::Texture& obj_textures,
			const glm::vec3& obj_bounds
		);

		ObjectData() = default;
	};

	struct BlockData {
		SMBC::Texture::TextureList texList;
		std::wstring uuid;
		std::wstring name;
		int tiling;

		BlockData(
			const SMBC::Texture::TextureList& tex_list,
			const std::wstring& uuid,
			const std::wstring& name,
			const int& tiling
		);

		BlockData() = default;
	};

	class ModData {
	public:
		std::unordered_map<std::wstring, ObjectData*> ObjectDB;
		std::unordered_map<std::wstring, BlockData*> BlockDB;
		SMBC::LangDB LanguageDB;

		std::wstring name;
		std::wstring workshop_id;
		std::wstring path;

		void LoadTranslations(const std::wstring& path = L"");
		void LoadObjects();

		bool UuidExists(const std::wstring& uuid);
		void AddBlock(SMBC::BlockData*& block);
		void AddPart(SMBC::ObjectData*& part);

		ModData(
			const std::wstring& name,
			const std::wstring& workshop_id,
			const std::wstring& path
		);

		ModData() = default;
		~ModData();
	};
}