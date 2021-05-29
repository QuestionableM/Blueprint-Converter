#pragma once

#include <string>
#include <vector>

namespace SMBC {
	class LangTrans {
	public:
		std::wstring uuid;
		std::wstring translation;

		LangTrans(const std::wstring& uuid, const std::wstring& translation);
	};

	class LangDB {
	public:
		std::vector<SMBC::LangTrans> _translations;
		std::wstring _Environment;

		LangDB(const std::wstring& environment);
		LangDB() = default;

		void clear_database();

		void LoadLanguageFile(const std::wstring& path);
		bool UuidExists(const std::wstring& uuid);
		void AddTranslation(const SMBC::LangTrans& translation);
		std::wstring GetTranslationString(const std::wstring& uuid);
	};
}