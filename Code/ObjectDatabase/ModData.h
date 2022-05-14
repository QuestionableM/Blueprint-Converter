#pragma once

#include "ObjectDatabase\LanguageDatabase.h"
#include "ObjectDatabase\TextureDatabase.h"
#include "ObjectDatabase\ObjectData.h"

#include "Lib\Uuid.h"

#include <unordered_map>
#include <string>
#include <glm.hpp>
#include <nlohmann/json.hpp>

namespace SMBC
{
	class Mod
	{
		friend class BlockListLoader;
		friend class PartListLoader;

		inline static std::unordered_map<Uuid, ObjectData*> AllObjects = {};
		inline static std::unordered_map<Uuid, Mod*> Mods              = {};
		inline static std::vector<Mod*> ModsArray                      = {};

	public:
		std::unordered_map<Uuid, ObjectData*> m_Objects;
		LangDB m_LanguageDb;

		std::wstring m_Name;
		unsigned __int64 m_WorkshopId;
		std::wstring m_Directory;
		Uuid m_Uuid;

	private:
		void LoadObjectFile(const std::wstring& path);
		bool IsShapeSetExtensionValid(const std::string& extension);

	public:
		void LoadObjectsFromDirectory(const std::wstring& dir);
		void LoadTranslations(const std::wstring& path);
		void LoadShapeSetList(const std::wstring& path);
		void LoadObjectDatabase();

		static std::size_t GetObjectCount();
		static std::size_t GetModCount();
		static void ClearMods();

		static const ObjectData* GetObject(const SMBC::Uuid& uuid);
		static const PartData* GetPart(const SMBC::Uuid& uuid);
		static const BlockData* GetBlock(const SMBC::Uuid& uuid);

		static Mod* CreateModFromDirectory(const std::wstring& dir);

		static Mod* CreateVanillaGameMod(const std::wstring& game_path);
		static Mod* CreateMod(
			const SMBC::Uuid& uuid,
			const std::wstring& name,
			const unsigned __int64& workshop_id,
			const std::wstring& dir
		);

		~Mod();
	};
}