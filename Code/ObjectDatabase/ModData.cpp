#include "ModData.h"

#include "ObjectDatabase\KeywordReplacer.h"
#include "ObjectDatabase\DatabaseLoader.h"

#include "ObjectDatabase\DataLoaders\BlockListLoader.h"
#include "ObjectDatabase\DataLoaders\PartListLoader.h"

#include "Lib\ConvData.h"
#include "Lib\String.h"
#include "Lib\File.h"
#include "Lib\Json.h"

#include "DebugCon.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace SMBC
{
	const static std::unordered_map<std::string, void (*)(const nlohmann::json&, Mod*)> m_DataLoaders =
	{
		{ "blockList", BlockListLoader::Load },
		{ "partList" , PartListLoader::Load  }
	};

	void Mod::LoadObjectFile(const std::wstring& path)
	{
		nlohmann::json mObjectFile = Json::LoadParseJson(path);
		if (!mObjectFile.is_object())
		{
			DebugErrorL("Couldn't load object database: ", path);
			return;
		}

		for (const auto& l_cur_item : mObjectFile.items())
		{
			if (!l_cur_item.value().is_array()) continue;

			const std::string l_key_str = l_cur_item.key();
			if (m_DataLoaders.find(l_key_str) != m_DataLoaders.end())
			{
				m_DataLoaders.at(l_key_str)(l_cur_item.value(), this);
			}
			else
			{
				DebugErrorL("Couldn't find the loader for \"", l_key_str, "\"");
			}
		}
	}

	bool Mod::IsShapeSetExtensionValid(const std::string& extension)
	{
		if (extension == ".json")     return true;
		if (extension == ".shapeset") return true;

		return false;
	}

	void Mod::LoadObjectsFromDirectory(const std::wstring& dir)
	{
		std::error_code mError;
		fs::directory_iterator dir_iter(dir, fs::directory_options::skip_permission_denied, mError);

		for (const auto& mCurDir : dir_iter)
		{
			if (mError)
			{
				DebugErrorL("Failed to get an item in: ", dir);
				continue;
			}

			if (!mCurDir.is_regular_file()) continue;

			const fs::path& fPath = mCurDir.path();
			if (!fPath.has_filename() || !fPath.has_extension()) continue;

			if (this->IsShapeSetExtensionValid(fPath.extension().string()))
				this->LoadObjectFile(fPath.wstring());
		}
	}

	void Mod::LoadTranslations(const std::wstring& path)
	{
		if (!File::Exists(path)) return;

		m_LanguageDb.LoadLanguageFile(path);
	}

	static const std::wstring g_ShapeSetExtensions[2] =
	{
		L".shapedb",
		L".json"
	};

	bool FindShapeSetFile(const std::wstring& mod_path, std::wstring& output_path)
	{
		const std::wstring l_DatabaseDir = mod_path + L"/Objects/Database/shapesets";

		for (__int8 a = 0; a < 2; a++)
		{
			const std::wstring l_FullPath = l_DatabaseDir + g_ShapeSetExtensions[a];
			if (File::Exists(l_FullPath))
			{
				output_path = l_FullPath;
				return true;
			}
		}

		return false;
	}

	void Mod::LoadShapeSetList(const std::wstring& path)
	{
		const nlohmann::json l_ShapeSetListFile = Json::LoadParseJson(path);
		if (!l_ShapeSetListFile.is_object()) return;

		const auto& l_ShapeSetList = Json::Get(l_ShapeSetListFile, "shapeSetList");
		if (!l_ShapeSetList.is_array()) return;

		for (const auto& path_shapedb : l_ShapeSetList)
		{
			if (!path_shapedb.is_string()) continue;

			const std::wstring l_path_wide = String::ToWide(path_shapedb.get<std::string>());
			const std::wstring l_full_path = PathReplacer::ReplaceKey(l_path_wide);

			this->LoadObjectFile(l_full_path);
		}
	}

	void Mod::LoadObjectDatabase()
	{
		this->LoadTranslations(m_Directory + L"/Gui/Language/English/inventoryDescriptions.json");
		PathReplacer::SetModData(m_Directory, m_Uuid);

		std::wstring l_ShapeSetPath;
		if (FindShapeSetFile(m_Directory, l_ShapeSetPath))
		{
			this->LoadShapeSetList(l_ShapeSetPath);
		}
		else
		{
			const std::wstring l_DbPath = m_Directory + L"/Objects/Database/ShapeSets";
			if (!File::Exists(l_DbPath)) return;

			this->LoadObjectsFromDirectory(l_DbPath);
		}
	}

	std::size_t Mod::GetObjectCount()
	{
		return AllObjects.size();
	}

	std::size_t Mod::GetModCount()
	{
		return Mods.size();
	}

	void Mod::ClearMods()
	{
		DebugOutL("Removing ", 0b1101_fg, Mod::ModsArray.size(), 0b1110_fg, " mods from memory...");

		for (std::size_t a = 0; a < Mod::ModsArray.size(); a++)
			delete Mod::ModsArray[a];

		Mod::ModsArray.clear();
		Mod::Mods.clear();
		Mod::AllObjects.clear();
	}

	const ObjectData* Mod::GetObject(const SMBC::Uuid& uuid)
	{
		if (AllObjects.find(uuid) == AllObjects.end())
			return nullptr;

		return AllObjects.at(uuid);
	}

	const PartData* Mod::GetPart(const SMBC::Uuid& uuid)
	{
		const ObjectData* current_obj = GetObject(uuid);
		if (!current_obj || current_obj->Type() != ObjectType::Part)
			return nullptr;

		return static_cast<const PartData*>(current_obj);
	}

	const BlockData* Mod::GetBlock(const SMBC::Uuid& uuid)
	{
		const ObjectData* current_obj = GetObject(uuid);
		if (!current_obj || current_obj->Type() != ObjectType::Block)
			return nullptr;

		return static_cast<const BlockData*>(current_obj);
	}

	Mod* Mod::CreateModFromDirectory(const std::wstring& dir)
	{
		const std::wstring mDescPath = dir + L"/description.json";
		if (!File::Exists(mDescPath)) return nullptr;

		const nlohmann::json mDescJson = Json::LoadParseJson(mDescPath, true);
		if (!mDescJson.is_object()) return nullptr;

		const auto& mType = Json::Get(mDescJson, "type");
		if (!mType.is_string() || mType.get<std::string>() != "Blocks and Parts")
			return nullptr;

		const auto& mUuid = Json::Get(mDescJson, "localId");
		const auto& mName = Json::Get(mDescJson, "name");

		if (!mUuid.is_string() || !mName.is_string()) return nullptr;

		const auto& lWorkshopIdStr = Json::Get(mDescJson, "fileId");
		const unsigned __int64 lWorkshopId = (lWorkshopIdStr.is_number() ? lWorkshopIdStr.get<unsigned __int64>() : 0ull);

		return Mod::CreateMod(
			SMBC::Uuid(mUuid.get<std::string>()),
			String::ToWide(mName.get<std::string>()),
			lWorkshopId,
			dir
		);
	}

	Mod* Mod::CreateVanillaGameMod(const std::wstring& game_path)
	{
		Mod* pNewMod = new Mod();
		pNewMod->m_Uuid = Uuid();
		pNewMod->m_Name = L"VANILLA GAME";
		pNewMod->m_WorkshopId = 0ull;
		pNewMod->m_Directory = game_path;

		Mod::ModsArray.push_back(pNewMod);
		return pNewMod;
	}

	Mod* Mod::CreateMod(
		const SMBC::Uuid& uuid,
		const std::wstring& name,
		const unsigned __int64& workshop_id,
		const std::wstring& dir)
	{
		if (Mod::Mods.find(uuid) != Mod::Mods.end())
		{
			DebugWarningL("Uuid conflict between 2 mods: \"", name, "\" and \"", Mod::Mods.at(uuid)->m_Name, "\" (", uuid.ToString(), ")");
			return nullptr;
		}

		Mod* pNewMod = new Mod();
		pNewMod->m_Uuid = uuid;
		pNewMod->m_Name = name;
		pNewMod->m_WorkshopId = workshop_id;
		pNewMod->m_Directory = dir;

		Mod::Mods.insert(std::make_pair(uuid, pNewMod));
		Mod::ModsArray.push_back(pNewMod);
		return pNewMod;
	}

	Mod::~Mod()
	{
		for (auto& obj : m_Objects)
			delete obj.second;
	}
}