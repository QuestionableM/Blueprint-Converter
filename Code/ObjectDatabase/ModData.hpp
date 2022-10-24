#pragma once

#include "ObjectDatabase\LanguageDatabase.hpp"
#include "ObjectDatabase\TextureDatabase.hpp"
#include "ObjectDatabase\ObjectData.hpp"

#include "Lib\Uuid.hpp"

#include <nlohmann\json.hpp>
#include <glm.hpp>

#include <unordered_map>
#include <string>

namespace SMBC
{
	class Mod
	{
		friend class BlockListLoader;
		friend class PartListLoader;

		using ObjectDataMap = std::unordered_map<Uuid, ObjectData*>;
		using ModDataMap    = std::unordered_map<Uuid, Mod*>;

		inline static std::vector<Mod*> ModsArray = {};
		inline static ObjectDataMap AllObjects    = {};
		inline static ModDataMap ModsMap          = {};

	public:
		std::unordered_map<Uuid, ObjectData*> m_Objects;
		LangDB m_LanguageDb;

		std::wstring m_Name;
		unsigned __int64 m_WorkshopId;
		std::wstring m_Directory;
		Uuid m_Uuid;
		bool m_isLocal;

	private:
		void LoadObjectFile(const std::wstring& path);
		bool IsShapeSetExtensionValid(const std::string& extension);

	public:
		void LoadObjectsFromDirectory(const std::wstring& dir);
		void LoadTranslations(const std::wstring& path);
		void LoadShapeSetList(const std::wstring& path);
		void LoadObjectDatabase();

		inline static std::size_t GetObjectCount() { return Mod::AllObjects.size(); }
		inline static std::size_t GetModCount() { return Mod::ModsArray.size(); }
		static void ClearMods();

		inline static const ObjectData* GetObject(const SMBC::Uuid& uuid)
		{
			const ObjectDataMap::const_iterator v_iter = Mod::AllObjects.find(uuid);
			if (v_iter == Mod::AllObjects.end())
				return nullptr;

			return v_iter->second;
		}

		inline static const PartData* GetPart(const SMBC::Uuid& uuid)
		{
			const ObjectDataMap::const_iterator v_iter = Mod::AllObjects.find(uuid);
			if (v_iter == Mod::AllObjects.end())
				return nullptr;

			const ObjectData* v_curObj = v_iter->second;
			if (v_curObj->Type() != ObjectType::Part)
				return nullptr;

			return static_cast<const PartData*>(v_curObj);
		}

		inline static const BlockData* GetBlock(const SMBC::Uuid& uuid)
		{
			const ObjectDataMap::const_iterator v_iter = Mod::AllObjects.find(uuid);
			if (v_iter == Mod::AllObjects.end())
				return nullptr;

			const ObjectData* v_curObj = v_iter->second;
			if (v_curObj->Type() != ObjectType::Block)
				return nullptr;

			return static_cast<const BlockData*>(v_curObj);
		}

		static Mod* CreateModFromDirectory(const std::wstring& dir, const bool& is_local);
		static Mod* CreateVanillaGameMod(const std::wstring& game_path);

		~Mod();
	};
}