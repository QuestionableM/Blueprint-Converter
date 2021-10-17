#pragma once

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Language Database/LanguageDatabase.h"
#include "Object Database/Object Data/ObjectData.h"

#include <unordered_map>
#include <string>
#include <glm.hpp>

namespace SMBC
{
	class Mod
	{
		static std::unordered_map<std::wstring, ObjectData*> AllObjects;
		static std::unordered_map<std::wstring, Mod*> Mods;
	public:
		std::unordered_map<std::wstring, ObjectData*> Objects;
		SMBC::LangDB LanguageDB;

		std::wstring Name;
		std::wstring WorkshopId;
		std::wstring Path;
		std::wstring Uuid;

		void LoadTranslations(const std::wstring& path = L"");
		void LoadObjects();

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