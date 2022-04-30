#include "DatabaseLoader.h"

#include "ObjectDatabase\KeywordReplacer.h"
#include "ObjectDatabase\ModData.h"

#include "Lib\ProgramSettings.h"
#include "Lib\ConvData.h"
#include "Lib\File.h"
#include "Lib\String.h"

#include <filesystem>

#include "DebugCon.h"

namespace fs = std::filesystem;

namespace SMBC
{
	void DatabaseLoader::LoadGameDatabase()
	{
		Mod* _VanillaParts = Mod::CreateMod(
			SMBC::Uuid("00000000-0000-0000-0000-000000000000"),
			L"VANILLA GAME",
			L"",
			Settings::PathToSM
		);

		for (std::wstring& path : Settings::VanillaLanguagePaths)
			_VanillaParts->LoadTranslations(path);

		for (std::wstring& data_path : Settings::SMDirDatabase)
		{
			if (!File::Exists(data_path)) continue;

			_VanillaParts->LoadObjectsFromDirectory(data_path);
		}

		DebugOutL("Loaded ", ConCol::YELLOW_INT, _VanillaParts->Name, ConCol::WHITE, " (Objects: ", ConCol::YELLOW_INT, _VanillaParts->Objects.size(), ConCol::WHITE, ")");
	}

	void DatabaseLoader::LoadModDatabase()
	{
		if (Settings::ModFolders.empty()) return;

		std::error_code mError;

		for (const std::wstring& _ModDirectory : Settings::ModFolders)
		{
			if (!File::Exists(_ModDirectory)) continue;

			fs::directory_iterator _DirIter(_ModDirectory, fs::directory_options::skip_permission_denied, mError);
			for (const auto& dir : _DirIter)
			{
				if (mError)
				{
					DebugErrorL("Couldn't get an item in: ", _ModDirectory);
					continue;
				}

				if (!dir.is_directory()) continue;

				const std::wstring mModDir = dir.path().wstring();

				Mod* NewMod = Mod::CreateModFromDirectory(mModDir);
				if (!NewMod) continue;

				const std::wstring mDatabasePath = mModDir + L"/Objects/Database/ShapeSets";
				if (!File::Exists(mDatabasePath)) continue;

				NewMod->LoadTranslations();

				PathReplacer::SetModData(NewMod->Path, NewMod->Uuid);
				NewMod->LoadObjectsFromDirectory(mDatabasePath);

				DebugOutL("Loaded: ", ConCol::YELLOW_INT , NewMod->Name, ConCol::WHITE, " (Objects: ", ConCol::YELLOW_INT, NewMod->Objects.size(), ConCol::WHITE, ")");
			}
		}
	}

	void DatabaseLoader::LoadDatabase()
	{
		Mod::ClearMods();

		ConvData::SetState(ConvState::DB_ReadingDatabase, 0);

		DatabaseLoader::LoadGameDatabase();
		DatabaseLoader::LoadModDatabase();
	}
}