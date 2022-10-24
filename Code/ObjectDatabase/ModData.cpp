#include "ModData.hpp"

#include "ObjectDatabase\DataLoaders\BlockListLoader.hpp"
#include "ObjectDatabase\DataLoaders\PartListLoader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\DatabaseLoader.hpp"

#include "Lib\ConvData.hpp"
#include "Lib\String.hpp"
#include "Lib\File.hpp"
#include "Lib\Json.hpp"

#include "DebugCon.hpp"

#include <filesystem>
namespace fs = std::filesystem;

namespace SMBC
{
	using DataLoaderMap = std::unordered_map<std::string, void (*)(const nlohmann::json&, Mod*)>;
	const static DataLoaderMap m_DataLoaders =
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
			const DataLoaderMap::const_iterator it = m_DataLoaders.find(l_key_str);
			if (it != m_DataLoaders.end())
			{
				it->second(l_cur_item.value(), this);
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

	inline bool FindShapeSetFile(const std::wstring& mod_path, std::wstring& output_path)
	{
		const std::wstring v_dbDir = mod_path + L"/Objects/Database/shapesets";

		const std::wstring v_shapedbPath = v_dbDir + L".shapedb";
		if (File::Exists(v_shapedbPath))
		{
			output_path = v_shapedbPath;
			return true;
		}

		const std::wstring v_jsonPath = v_dbDir + L".json";
		if (File::Exists(v_jsonPath))
		{
			output_path = v_jsonPath;
			return true;
		}

		return false;
	}

	void Mod::LoadShapeSetList(const std::wstring& path)
	{
		const nlohmann::json v_shapeSetListJson = Json::LoadParseJson(path);
		if (!v_shapeSetListJson.is_object()) return;

		const auto& v_shapeSetArray = Json::Get(v_shapeSetListJson, "shapeSetList");
		if (!v_shapeSetArray.is_array()) return;

		for (const auto& v_curShapedb : v_shapeSetArray)
		{
			if (!v_curShapedb.is_string()) continue;

			const std::wstring v_pathWide = String::ToWide(v_curShapedb.get_ref<const std::string&>());
			const std::wstring v_fullPath = PathReplacer::ReplaceKey(v_pathWide);

			this->LoadObjectFile(v_fullPath);
		}
	}

	void Mod::LoadObjectDatabase()
	{
		this->LoadTranslations(m_Directory + L"/Gui/Language/English/inventoryDescriptions.json");
		PathReplacer::SetModData(m_Directory, m_Uuid);

		std::wstring v_shapeSetPath;
		if (FindShapeSetFile(m_Directory, v_shapeSetPath))
		{
			this->LoadShapeSetList(v_shapeSetPath);
		}
		else
		{
			const std::wstring v_dbPath = m_Directory + L"/Objects/Database/ShapeSets";
			if (!File::Exists(v_dbPath)) return;

			this->LoadObjectsFromDirectory(v_dbPath);
		}
	}

	void Mod::ClearMods()
	{
		DebugOutL("Removing ", 0b1101_fg, Mod::ModsArray.size(), 0b1110_fg, " mods from memory...");

		for (std::size_t a = 0; a < Mod::ModsArray.size(); a++)
			delete Mod::ModsArray[a];

		Mod::AllObjects.clear();
		Mod::ModsArray.clear();
		Mod::ModsMap.clear();
	}

	const ObjectData* Mod::GetObject(const SMBC::Uuid& uuid)
	{
		const ObjectDataMap::const_iterator it = Mod::AllObjects.find(uuid);
		if (it == AllObjects.end())
			return nullptr;

		return it->second;
	}

	const PartData* Mod::GetPart(const SMBC::Uuid& uuid)
	{
		const ObjectDataMap::const_iterator it = Mod::AllObjects.find(uuid);
		if (it == Mod::AllObjects.end())
			return nullptr;

		const ObjectData* p_cur_obj = it->second;
		if (p_cur_obj->Type() != ObjectType::Part)
			return nullptr;

		return static_cast<const PartData*>(p_cur_obj);
	}

	const BlockData* Mod::GetBlock(const SMBC::Uuid& uuid)
	{
		const ObjectDataMap::const_iterator it = Mod::AllObjects.find(uuid);
		if (it == Mod::AllObjects.end())
			return nullptr;

		const ObjectData* p_cur_obj = it->second;
		if (p_cur_obj->Type() != ObjectType::Block)
			return nullptr;

		return static_cast<const BlockData*>(p_cur_obj);
	}

	Mod* Mod::CreateModFromDirectory(const std::wstring& dir, const bool& is_local)
	{
		const std::wstring v_descriptionPath = dir + L"/description.json";
		if (!File::Exists(v_descriptionPath)) return nullptr;

		const nlohmann::json v_descJson = Json::LoadParseJson(v_descriptionPath, true);
		if (!v_descJson.is_object()) return nullptr;

		const auto& v_modType = Json::Get(v_descJson, "type");
		if (!v_modType.is_string() || v_modType.get_ref<const std::string&>() != "Blocks and Parts")
			return nullptr;

		const auto& v_modName = Json::Get(v_descJson, "name");
		const auto& v_modUuidStr = Json::Get(v_descJson, "localId");

		if (!v_modName.is_string() || !v_modUuidStr.is_string())
			return nullptr;

		const auto& v_workshopIdStr = Json::Get(v_descJson, "fileId");
		const Uuid v_modUuid(v_modUuidStr.get_ref<const std::string&>());

		const ModDataMap::const_iterator v_iter = Mod::ModsMap.find(v_modUuid);
		if (v_iter != Mod::ModsMap.end())
		{
			if (v_iter->second->m_isLocal && !is_local)
			{
				DebugOutL("A local version of this mod has already been loaded! (Uuid: ", v_modUuid.ToString(), ", Name: ", v_modName.get_ref<const std::string&>(), ")");
				return nullptr;
			}

			if (v_iter->second->m_isLocal == is_local)
			{
				DebugErrorL("Uuid conflict between 2 mods: \"", v_modName.get_ref<const std::string&>(), "\" and \"", v_iter->second->m_Name, "\" (Uuid: ", v_modUuid.ToString(), ")");
				return nullptr;
			}
		}

		Mod* v_newMod = new Mod();
		v_newMod->m_Uuid = v_modUuid;
		v_newMod->m_Name = String::ToWide(v_modName.get_ref<const std::string&>());
		v_newMod->m_WorkshopId = (v_workshopIdStr.is_number() ? v_workshopIdStr.get<unsigned __int64>() : 0ull);
		v_newMod->m_Directory = dir;
		v_newMod->m_isLocal = is_local;

		Mod::ModsArray.push_back(v_newMod);
		Mod::ModsMap.insert(std::make_pair(v_newMod->m_Uuid, v_newMod));

		return v_newMod;
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

	Mod::~Mod()
	{
		for (auto& obj : m_Objects)
			delete obj.second;
	}
}