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

		DebugOutL("Loaded: ", 0b1101_fg, _VanillaParts->Name, 0b1110_fg, " (Objects: ", 0b1101_fg, _VanillaParts->Objects.size(), 0b1110_fg, ")");
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

				Mod* pNewMod = Mod::CreateModFromDirectory(mModDir);
				if (!pNewMod) continue;

				const std::wstring mDatabasePath = mModDir + L"/Objects/Database/ShapeSets";
				if (!File::Exists(mDatabasePath)) continue;

				pNewMod->LoadTranslations();

				PathReplacer::SetModData(pNewMod->Path, pNewMod->Uuid);
				pNewMod->LoadObjectsFromDirectory(mDatabasePath);

				DebugOutL("Loaded: ", 0b1101_fg, pNewMod->Name, 0b1110_fg, " (Objects: ", 0b1101_fg, pNewMod->Objects.size(), 0b1110_fg, ")");
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