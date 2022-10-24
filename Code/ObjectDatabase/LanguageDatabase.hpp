#pragma once

#include "Lib\Uuid.hpp"

#include <unordered_map>
#include <string>

namespace SMBC
{
	class LangDB
	{
		using TranslationStorageMap = std::unordered_map<Uuid, std::wstring>;
	public:
		TranslationStorageMap m_Translations;

		LangDB() = default;

		void LoadLanguageFile(const std::wstring& path);

		std::wstring GetTranslation(const Uuid& uuid) const;
	};
}