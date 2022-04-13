#pragma once

#include <string>
#include <unordered_map>

#include "Lib\Uuid.h"

namespace SMBC {
	class LangDB {
	public:
		std::unordered_map<Uuid, std::wstring> Translations;

		LangDB() = default;

		void LoadLanguageFile(const std::wstring& path);
		void AddTranslation(const Uuid& uuid, const std::wstring& trans);
		const std::wstring& GetTranslation(const Uuid& uuid) const;
	};
}