#pragma once

#include <string>
#include <unordered_map>

namespace SMBC {
	class LangDB {
	public:
		std::unordered_map<std::wstring, std::wstring> _Translations;
		std::wstring _Environment;

		LangDB(const std::wstring& environment);
		LangDB() = default;

		void clear_database();

		void LoadLanguageFile(const std::wstring& path);
		bool UuidExists(const std::wstring& uuid);
		void AddTranslation(const std::wstring& uuid, const std::wstring& trans);
		const std::wstring& GetTranslation(const std::wstring& uuid);
	};
}