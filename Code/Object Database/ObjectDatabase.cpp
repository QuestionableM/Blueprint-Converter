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

std::vector<SMBC::ModData> SMBC::ObjectDatabase::ModDB = {};

bool SMBC::ObjectDatabase::GetPart(const std::wstring& uuid, SMBC::ObjectData& object) {
	for (SMBC::ModData& mod : SMBC::ObjectDatabase::ModDB) {
		for (SMBC::ObjectData& part : mod.ObjectDB) {
			if (part._obj_uuid != uuid) continue;

			object = part;
			return true;
		}
	}

	return false;
}

bool SMBC::ObjectDatabase::GetBlock(const std::wstring& uuid, SMBC::BlockData& block) {
	for (SMBC::ModData& mod : SMBC::ObjectDatabase::ModDB) {
		for (SMBC::BlockData& blk : mod.BlockDB) {
			if (blk._obj_uuid != uuid) continue;

			block = blk;
			return true;
		}
	}

	return false;
}

long long SMBC::ObjectDatabase::CountLoadedObjects() {
	long long output = 0;

	for (SMBC::ModData& mod : SMBC::ObjectDatabase::ModDB) {
		output += (long long)mod.ObjectDB.size();
		output += (long long)mod.BlockDB.size();
	}

	return output;
}

void SMBC::ObjectDatabase::ReloadDatabase() {
	SMBC::BlueprintConversionData::SetNewStage(SMBC_CONV_READING_DATABASE, 0);
	SMBC::ObjectDatabase::ModDB.clear();
	SMBC::DatabaseLoader::LoadGameDatabase();
	SMBC::DatabaseLoader::LoadModDatabase();
}