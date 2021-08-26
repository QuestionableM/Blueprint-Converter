#include <filesystem>
#include <fstream>

#include "ObjectDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"
#include "Database Loader/DatabaseLoader.h"

#include "Lib/ConvData/ConvData.h"
#include "Lib/File/FileFunc.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/Json/JsonFunc.h"
#include "Lib/ProgramSettings.h"

namespace fs = std::filesystem;
typedef SMBC::ObjectDatabase _ObjectDatabase;

std::unordered_map<std::wstring, SMBC::ModData*> _ObjectDatabase::ModDB = {};

SMBC::ObjectData* _ObjectDatabase::GetPart(const std::wstring& uuid) {
	for (const auto& mod_data : _ObjectDatabase::ModDB) {
		SMBC::ModData* mod = mod_data.second;

		if (mod->ObjectDB.find(uuid) != mod->ObjectDB.end())
			return mod->ObjectDB.at(uuid);
	}

	return nullptr;
}

SMBC::BlockData* _ObjectDatabase::GetBlock(const std::wstring& uuid) {
	for (const auto& mod_data : _ObjectDatabase::ModDB) {
		SMBC::ModData* mod = mod_data.second;

		if (mod->BlockDB.find(uuid) != mod->BlockDB.end())
			return mod->BlockDB.at(uuid);
	}

	return nullptr;
}

std::size_t _ObjectDatabase::CountLoadedObjects() {
	std::size_t output = 0;

	for (const auto& mod_data : _ObjectDatabase::ModDB) {
		SMBC::ModData* mod = mod_data.second;

		output += mod->ObjectDB.size();
		output += mod->BlockDB.size();
	}

	return output;
}

void _ObjectDatabase::ClearDatabase() {
	for (auto& mod_data : _ObjectDatabase::ModDB)
		delete mod_data.second;

	_ObjectDatabase::ModDB.clear();
}

void _ObjectDatabase::ReloadDatabase() {
	SMBC::ConvData::SetState(SMBC::State::ReadingDatabase);

	_ObjectDatabase::ClearDatabase();

	SMBC::DatabaseLoader::LoadGameDatabase();
	SMBC::DatabaseLoader::LoadModDatabase();
}