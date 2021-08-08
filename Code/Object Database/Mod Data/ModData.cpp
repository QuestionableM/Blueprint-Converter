#include "ModData.h"
#include "Lib/File/FileFunc.h"
#include "Object Database/Database Loader/DatabaseLoader.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include <filesystem>
namespace fs = std::filesystem;

SMBC::ObjectData::ObjectData(
	const std::wstring& obj_uuid,
	const std::wstring& obj_path,
	const std::wstring& obj_name,
	SMBC::Texture::Texture& obj_textures,
	const glm::vec3& obj_bounds
) {
	this->_obj_path = obj_path;
	this->_obj_uuid = obj_uuid;
	this->_obj_name = obj_name;
	this->obj_textures = obj_textures;
	this->_obj_bounds = obj_bounds;
}

SMBC::BlockData::BlockData(
	const SMBC::Texture::TextureList& tex_list,
	const std::wstring& uuid,
	const std::wstring& name,
	const int& tiling
) {
	this->_block_tex = tex_list;
	this->_obj_uuid = uuid;
	this->_obj_name = name;
	this->_tiling = tiling;
}

void SMBC::ModData::LoadTranslations(const std::wstring& path) {
	std::wstring _Path = (path.empty() ? (this->path + L"/Gui/Language/English") : path);

	std::wstring _LangFile = _Path + L"/inventoryDescriptions.json";
	std::wstring _OtherLangFile = _Path + L"/InventoryItemDescriptions.json";

	std::wstring _FinalPath = (SMBC::File::FileExists(_LangFile) ? _LangFile : SMBC::File::FileExists(_OtherLangFile) ? _OtherLangFile : L"");

	this->LanguageDB.LoadLanguageFile(_FinalPath);
}

void SMBC::ModData::LoadObjects() {
	std::wstring _ObjDirectory = this->path + L"/Objects/Database/ShapeSets";

	if (!SMBC::File::FileExists(_ObjDirectory)) return;

	fs::recursive_directory_iterator RecDirIter(_ObjDirectory, fs::directory_options::skip_permission_denied);
	for (auto& m_dir : RecDirIter) {
		if (!m_dir.is_regular_file() || !m_dir.path().has_extension() || m_dir.path().extension() != ".json") continue;

		SMBC::DatabaseLoader::LoadObjectFile(this, m_dir.path().wstring(), this->LanguageDB);
	}
}

bool SMBC::ModData::UuidExists(const std::wstring& uuid) {
	if (this->BlockDB.find(uuid) != this->BlockDB.end()) return true;
	if (this->ObjectDB.find(uuid) != this->ObjectDB.end()) return true;

	return false;
}

void SMBC::ModData::AddBlockToDatabase(SMBC::BlockData*& block) {
	if (this->BlockDB.find(block->_obj_uuid) == this->BlockDB.end())
		this->BlockDB.insert(std::make_pair(block->_obj_uuid, block));
}

void SMBC::ModData::AddPartToDatabase(SMBC::ObjectData*& part) {
	if (this->ObjectDB.find(part->_obj_uuid) == this->ObjectDB.end())
		this->ObjectDB.insert(std::make_pair(part->_obj_uuid, part));
}

SMBC::ModData::ModData(
	const std::wstring& name,
	const std::wstring& workshop_id,
	const std::wstring& path
) {
	this->name = name;
	this->workshop_id = workshop_id;
	this->path = path;
	this->LanguageDB._Environment = name;
}

SMBC::ModData::~ModData() {
	for (auto& obj_Itr : this->ObjectDB)
		delete obj_Itr.second;

	for (auto& blk_Itr : this->BlockDB)
		delete blk_Itr.second;
}