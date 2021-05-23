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

	class ModData {
	public:
		std::vector<ObjectData> ObjectDB;
		std::vector<BlockData> BlockDB;

		std::wstring uuid;
		std::wstring name;
		std::wstring workshop_id;
		std::wstring path;

		SMBC::LangDB LoadTranslations();
		void LoadObjects(SMBC::LangDB& translations);
		void LoadModData();

		void AddBlockToDatabase(SMBC::BlockData& block);
		void AddPartToDatabase(SMBC::ObjectData& part);

		bool GetPart(const std::wstring& uuid, SMBC::ObjectData& part);
		bool GetBlock(const std::wstring& uuid, SMBC::BlockData& block);

		ModData(
			const std::wstring& uuid,
			const std::wstring& name,
			const std::wstring& workshop_id,
			const std::wstring& path
		);

		ModData() = default;
	};

	class ObjectDatabase {
	public:
		static std::vector<ModData> ModDB;
		static SMBC::LangDB GameTranslations;

		static void LoadLanguageFile(const std::wstring& path);
		static glm::vec3 LoadBlockCollision(const nlohmann::json& collision);

		static bool GetPart(const std::wstring& uuid, SMBC::ObjectData& object);
		static bool GetBlock(const std::wstring& uuid, SMBC::BlockData& block);

		static long long CountLoadedObjects();

		static void GetRenderableData(
			const nlohmann::json& renderable,
			SMBC::Texture::Texture& texture,
			std::wstring& model_path
		);

		static void LoadObjectFile(
			SMBC::ModData& mod,
			const std::wstring& path,
			SMBC::LangDB& translations
		);
		static void LoadGameDatabase();
		static void LoadModDatabase();
	};
};