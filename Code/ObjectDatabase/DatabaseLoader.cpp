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
		Mod* pVanillaMod = Mod::CreateVanillaGameMod(Settings::PathToSM);

		for (const std::wstring& path : Settings::VanillaLanguagePaths)
			pVanillaMod->LoadTranslations(path);

		for (const std::wstring& data_path : Settings::SMDirDatabase)
		{
			if (!File::Exists(data_path)) continue;

			pVanillaMod->LoadObjectsFromDirectory(data_path);
		}

		DebugOutL("Loaded: ", 0b1101_fg, pVanillaMod->m_Name, 0b1110_fg, " (Objects: ", 0b1101_fg, pVanillaMod->m_Objects.size(), 0b1110_fg, ")");
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

				pNewMod->LoadObjectDatabase();

				DebugOutL("Loaded: ", 0b1101_fg, pNewMod->m_Name, 0b1110_fg, " (Objects: ", 0b1101_fg, pNewMod->m_Objects.size(), 0b1110_fg, ")");
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