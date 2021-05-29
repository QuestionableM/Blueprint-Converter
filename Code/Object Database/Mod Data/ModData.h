#pragma once

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Language Database/LanguageDatabase.h"

#include <string>
#include <glm.hpp>

namespace SMBC {
	struct ObjectData {
		std::wstring _obj_uuid;
		std::wstring _obj_name;
		std::wstring _obj_path;
		SMBC::Texture::Texture obj_textures;
		glm::vec3 _obj_bounds;

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
		SMBC::Texture::TextureList _block_tex;
		std::wstring _obj_uuid;
		std::wstring _obj_name;
		int _tiling;

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
		std::vector<ObjectData> ObjectDB;
		std::vector<BlockData> BlockDB;
		SMBC::LangDB LanguageDB;

		std::wstring uuid;
		std::wstring name;
		std::wstring workshop_id;
		std::wstring path;

		void LoadTranslations(const std::wstring& path = L"");
		void LoadObjects();

		void AddBlockToDatabase(SMBC::BlockData& block);
		void AddPartToDatabase(SMBC::ObjectData& part);

		ModData(
			const std::wstring& uuid,
			const std::wstring& name,
			const std::wstring& workshop_id,
			const std::wstring& path
		);

		ModData() = default;
	};
}