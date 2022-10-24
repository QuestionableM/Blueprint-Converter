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

		inline std::wstring GetTranslation(const Uuid& uuid) const
		{
			const TranslationStorageMap::const_iterator v_iter = m_Translations.find(uuid);
			if (v_iter != m_Translations.end())
				return v_iter->second;

			return L"BLOCK_NOT_FOUND";
		}
	};
}