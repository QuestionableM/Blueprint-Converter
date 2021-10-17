#pragma once

#include <string>
#include <unordered_map>

namespace SMBC {
	class LangDB {
	public:
		std::unordered_map<std::wstring, std::wstring> Translations;
		std::wstring Environment;

		LangDB(const std::wstring& environment);
		LangDB() = default;

		void ClearDatabase();

		void LoadLanguageFile(const std::wstring& path);
		bool UuidExists(const std::wstring& uuid);
		void AddTranslation(const std::wstring& uuid, const std::wstring& trans);
		const std::wstring& GetTranslation(const std::wstring& uuid);
	};
}