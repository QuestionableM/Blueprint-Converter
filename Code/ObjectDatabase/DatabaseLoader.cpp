#include "DatabaseLoader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ModData.hpp"

#include "Lib\ProgramSettings.hpp"
#include "Lib\ConvData.hpp"
#include "Lib\String.hpp"
#include "Lib\File.hpp"

#include "DebugCon.hpp"

#include <filesystem>

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

	void DatabaseLoader::LoadModsFromStrVector(const std::vector<std::wstring>& r_wstr_vec, const bool& is_local)
	{
		if (r_wstr_vec.empty())
			return;

		std::error_code v_errorCode;

		for (const std::wstring& v_modStorageDir : r_wstr_vec)
		{
			if (!File::Exists(v_modStorageDir))
				continue;

			fs::directory_iterator v_dirIter(v_modStorageDir, fs::directory_options::skip_permission_denied, v_errorCode);
			for (const auto& v_curDir : v_dirIter)
			{
				if (v_errorCode)
				{
					DebugErrorL("Caught an error while iterating through ", v_modStorageDir);
					continue;
				}

				if (!v_curDir.is_directory())
					continue;

				const std::wstring v_modDir = v_curDir.path().wstring();

				Mod* v_newMod = Mod::CreateModFromDirectory(v_modDir, is_local);
				if (!v_newMod) continue;

				v_newMod->LoadObjectDatabase();

				DebugOutL("Loaded: ", 0b1101_fg, v_newMod->m_Name, 0b1110_fg, " (Objects: ", 0b1101_fg, v_newMod->m_Objects.size(), 0b1110_fg, ", Is Local: ", v_newMod->m_isLocal, ")");
			}
		}
	}

	void DatabaseLoader::LoadModDatabase()
	{
		DebugOutL("Loading mods...");

		DatabaseLoader::LoadModsFromStrVector(Settings::LocalModFolders, true);
		DatabaseLoader::LoadModsFromStrVector(Settings::ModFolders, false);
	}

	void DatabaseLoader::LoadDatabase()
	{
		Mod::ClearMods();

		ConvData::SetState(ConvState::DB_ReadingDatabase, 0);

		DatabaseLoader::LoadGameDatabase();
		DatabaseLoader::LoadModDatabase();
	}
}