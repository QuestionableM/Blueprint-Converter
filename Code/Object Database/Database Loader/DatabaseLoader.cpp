#include "DatabaseLoader.h"

#include "Object Database/Mod Data/ModData.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/String/String.h"
#include "Lib/ConvData/ConvData.h"
#include "Lib/File/FileFunc.h"
#include "Lib/ProgramSettings.h"

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
	}

	void DatabaseLoader::LoadModDatabase()
	{
		if (Settings::ModFolders.empty()) return;

		for (std::wstring& _ModDirectory : Settings::ModFolders)
		{
			if (!File::Exists(_ModDirectory)) continue;

			fs::directory_iterator _DirIter(_ModDirectory, fs::directory_options::skip_permission_denied);
			for (auto& dir : _DirIter)
			{
				if (!dir.is_directory()) continue;

				const std::wstring mModDir = dir.path().wstring();

				Mod* NewMod = Mod::CreateModFromDirectory(mModDir);
				if (!NewMod) continue;

				const std::wstring mDatabasePath = mModDir + L"/Objects/Database/ShapeSets";
				if (!File::Exists(mDatabasePath)) continue;

				NewMod->LoadTranslations();

				PathReplacer::SetModData(NewMod->Path, NewMod->Uuid);
				NewMod->LoadObjectsFromDirectory(mDatabasePath);
			}
		}
	}

	void DatabaseLoader::LoadDatabase()
	{
		Mod::ClearMods();

		ConvData::SetState(State::ReadingDatabase);

		DatabaseLoader::LoadGameDatabase();
		DatabaseLoader::LoadModDatabase();
	}
}