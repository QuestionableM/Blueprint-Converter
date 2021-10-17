#include "ModData.h"
#include "Lib/File/FileFunc.h"
#include "Object Database/Database Loader/DatabaseLoader.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"
#include "Lib/String/String.h"
#include "DebugCon.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace SMBC
{
	std::unordered_map<std::wstring, ObjectData*> Mod::AllObjects = {};
	std::unordered_map<std::wstring, Mod*>        Mod::Mods       = {};

	void Mod::LoadTranslations(const std::wstring& path)
	{
		std::wstring _Path = (path.empty() ? (this->Path + L"/Gui/Language/English") : path);

		std::wstring _LangFile = _Path + L"/inventoryDescriptions.json";
		std::wstring _OtherLangFile = _Path + L"/InventoryItemDescriptions.json";

		std::wstring _FinalPath = (SMBC::File::Exists(_LangFile) ? _LangFile : SMBC::File::Exists(_OtherLangFile) ? _OtherLangFile : L"");

		this->LanguageDB.LoadLanguageFile(_FinalPath);
	}

	void Mod::LoadObjects()
	{
		std::wstring _ObjDirectory = this->Path + L"/Objects/Database/ShapeSets";

		if (!SMBC::File::Exists(_ObjDirectory)) return;

		fs::recursive_directory_iterator RecDirIter(_ObjDirectory, fs::directory_options::skip_permission_denied);
		for (auto& m_dir : RecDirIter)
		{
			if (!m_dir.is_regular_file() || !m_dir.path().has_extension() || m_dir.path().extension() != ".json") continue;

			SMBC::DatabaseLoader::LoadObjectFile(this, m_dir.path().wstring());
		}
	}

	bool Mod::UuidExists(const std::wstring& uuid)
	{
		return (AllObjects.find(uuid) != AllObjects.end());
	}

	void Mod::AddObject(ObjectData* object)
	{
		if (AllObjects.find(object->Uuid) != AllObjects.end())
			return;

		const auto new_pair = std::make_pair(object->Uuid, object);

		Objects.insert(new_pair);
		AllObjects.insert(new_pair);
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
		for (auto& mod : Mods)
			delete mod.second;

		Mods.clear();
	}

	const ObjectData* Mod::GetObject(const std::wstring& uuid)
	{
		if (AllObjects.find(uuid) == AllObjects.end())
			return nullptr;

		return AllObjects.at(uuid);
	}

	const PartData* Mod::GetPart(const std::wstring& uuid)
	{
		const ObjectData* current_obj = GetObject(uuid);

		return (current_obj != nullptr) ? current_obj->ToPart() : nullptr;
	}

	const BlockData* Mod::GetBlock(const std::wstring& uuid)
	{
		const ObjectData* current_obj = GetObject(uuid);
		
		return (current_obj != nullptr) ? current_obj->ToBlock() : nullptr;
	}

	Mod* Mod::CreateModFromDirectory(const std::wstring& dir)
	{
		const std::wstring mDescPath = dir + L"/description.json";

		nlohmann::json mDescJson = Json::LoadParseJson(mDescPath, true);
		if (!mDescJson.is_object()) return nullptr;

		const auto& mType = Json::Get(mDescJson, "type");
		if (!mType.is_string() || mType.get<std::string>() != "Blocks and Parts")
			return nullptr;

		const auto& mUuid = Json::Get(mDescJson, "localId");
		const auto& mName = Json::Get(mDescJson, "name");

		if (!mUuid.is_string() || !mName.is_string()) return nullptr;

		const auto& mFileIdStr = Json::Get(mDescJson, "fileId");
		long long mFileId = mFileIdStr.is_number() ? mFileIdStr.get<long long>() : 0ll;

		return Mod::CreateMod(
			String::ToWide(mUuid.get<std::string>()),
			String::ToWide(mName.get<std::string>()),
			(mFileId > 0) ? std::to_wstring(mFileId) : L"",
			dir
		);
	}

	Mod* Mod::CreateMod(
		const std::wstring& uuid,
		const std::wstring& name,
		const std::wstring& workshop_id,
		const std::wstring& dir
	) {
		if (Mods.find(uuid) != Mods.end())
		{
			DebugErrorL("The specified uuid is already used by the other mod: ", Mods.at(uuid)->Name, " (", uuid, ")");
			return nullptr;
		}

		Mod* NewMod = new Mod();
		NewMod->Uuid = uuid;
		NewMod->Name = name;
		NewMod->WorkshopId = workshop_id;
		NewMod->Path = dir;

		DebugOutL("Loaded a new mod: ", Color::YELLOW_INT, NewMod->Name);

		Mods.insert(std::make_pair(uuid, NewMod));
		return NewMod;
	}

	Mod::~Mod()
	{
		for (auto& obj : Objects)
			delete obj.second;
	}
}