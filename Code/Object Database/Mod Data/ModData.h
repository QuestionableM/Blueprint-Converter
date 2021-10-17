#pragma once

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Language Database/LanguageDatabase.h"
#include "Object Database/Object Data/ObjectData.h"

#include <unordered_map>
#include <string>
#include <glm.hpp>
#include <nlohmann/json.hpp>

namespace SMBC
{
	class Mod
	{
		static std::unordered_map<std::wstring, ObjectData*> AllObjects;
		static std::unordered_map<std::wstring, Mod*> Mods;
	public:
		std::unordered_map<std::wstring, ObjectData*> Objects;
		LangDB LanguageDB;

		std::wstring Name;
		std::wstring WorkshopId;
		std::wstring Path;
		std::wstring Uuid;

	private:
		static bool GetBlockTextures(const nlohmann::json& block, Texture::TextureList& tex);
		void LoadBlocks(const nlohmann::json& fJson);

		static void LoadTextureList(const nlohmann::json& texList, Texture::TextureList& entry);
		static void AddSubMesh(const nlohmann::json& subMesh, Texture::Texture& tex, const std::wstring& idx);
		static bool LoadSubMeshes(const nlohmann::json& pLodItem, Texture::Texture& tex);
		static bool LoadRenderable(const nlohmann::json& pRenderable, Texture::Texture& tex_data, std::wstring& mesh_path);
		static bool GetRenderableData(const nlohmann::json& part, Texture::Texture& tex_data, std::wstring& mesh_path);
		static glm::vec3 LoadPartCollision(const nlohmann::json& collision);
		void LoadParts(const nlohmann::json& fJson);

		void LoadObjectFile(const std::wstring& path);

	public:
		void LoadObjectsFromDirectory(const std::wstring& dir);
		void LoadTranslations(const std::wstring& path = L"");

		bool UuidExists(const std::wstring& uuid);
		void AddObject(ObjectData* object);

		static std::size_t GetObjectCount();
		static std::size_t GetModCount();
		static void ClearMods();

		static const ObjectData* GetObject(const std::wstring& uuid);
		static const PartData* GetPart(const std::wstring& uuid);
		static const BlockData* GetBlock(const std::wstring& uuid);

		static Mod* CreateModFromDirectory(const std::wstring& dir);
		static Mod* CreateMod(
			const std::wstring& uuid,
			const std::wstring& name,
			const std::wstring& workshop_id,
			const std::wstring& dir
		);

		Mod() = default;
		~Mod();
	};
}