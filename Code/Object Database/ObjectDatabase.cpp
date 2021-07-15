#include <filesystem>
#include <fstream>

#include "ObjectDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"
#include "Database Loader/DatabaseLoader.h"

#include "Lib/Functions/Functions.h"
#include "Lib/File/FileFunc.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/Json/JsonFunc.h"
#include "Lib/ProgramSettings.h"

namespace fs = std::filesystem;
typedef SMBC::ObjectDatabase _ObjectDatabase;

std::vector<SMBC::ModData*> SMBC::ObjectDatabase::ModDB = {};

SMBC::ObjectData* _ObjectDatabase::GetPart(const std::wstring& uuid) {
	for (SMBC::ModData*& mod : _ObjectDatabase::ModDB) {
		for (SMBC::ObjectData*& part : mod->ObjectDB) {
			if (part->_obj_uuid != uuid) continue;

			return part;
		}
	}

	return nullptr;
}

SMBC::BlockData* _ObjectDatabase::GetBlock(const std::wstring& uuid) {
	for (SMBC::ModData*& mod : _ObjectDatabase::ModDB) {
		for (SMBC::BlockData*& block : mod->BlockDB) {
			if (block->_obj_uuid != uuid) continue;

			return block;
		}
	}

	return nullptr;
}

long long _ObjectDatabase::CountLoadedObjects() {
	long long output = 0;

	for (SMBC::ModData*& mod : _ObjectDatabase::ModDB) {
		output += (long long)mod->ObjectDB.size();
		output += (long long)mod->BlockDB.size();
	}

	return output;
}

void _ObjectDatabase::ClearDatabase() {
	for (SMBC::ModData*& mod_ptr : _ObjectDatabase::ModDB)
		delete mod_ptr;

	_ObjectDatabase::ModDB.clear();
}

void _ObjectDatabase::ReloadDatabase() {
	SMBC::BPConvData::SetNewStage(SMBC::Stat_ReadingDatabase);

	_ObjectDatabase::ClearDatabase();

	SMBC::DatabaseLoader::LoadGameDatabase();
	SMBC::DatabaseLoader::LoadModDatabase();
}