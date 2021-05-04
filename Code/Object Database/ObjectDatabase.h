#pragma once

#include <string>
#include <vector>
#include <vec3.hpp>
#include <nlohmann/json.hpp>

#include "Texture Database/TextureDatabase.h"
#include "Language Database/LanguageDatabase.h"

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

	class ObjectDatabase {
	public:
		static std::wstring _sm_path;
		static std::vector<std::wstring> _mods_path;
		static std::vector<std::wstring> _sm_dir_database;
		static std::vector<ObjectData> ObjectDB;
		static std::vector<BlockData> BlockDB;
		static SMBC::LangDB GameTranslations;

		static void ClearDatabase();

		static void LoadLanguageFile(const std::wstring& path);
		static void AddObjectToDatabase(SMBC::ObjectData& obj_data);
		static glm::vec3 LoadBlockCollision(const nlohmann::json& collision);

		static bool GetBlockByUuid(const std::wstring& uuid, SMBC::BlockData& object);
		static bool GetPartByUuid(const std::wstring& uuid, SMBC::ObjectData& object);
		static void AddBlockToDatabase(SMBC::BlockData& blk_data);

		static void GetRenderableData(
			const nlohmann::json& renderable,
			SMBC::Texture::Texture& texture,
			std::wstring& model_path
		);

		static void LoadObjectFile(const std::wstring& path, SMBC::LangDB& translations);
		static void LoadGameDatabase();
		static void LoadModDatabase();
		static void LoadConfig(const std::wstring& path);
		static void LoadJsonWstrArray(const nlohmann::json& file, const std::string& keyword, std::vector<std::wstring>& _array);
		static void SaveConfigFile(const bool sm_path = false, const bool bp_list = false, const bool mod_list = false);
		static void AddArrayPath(const std::wstring& element, std::vector<std::wstring>& _array);
		static void AddRegistryPathAndSave();
	};
};