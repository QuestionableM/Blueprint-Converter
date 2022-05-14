#pragma once

#include "Lib\Uuid.h"

#include <unordered_map>
#include <string>

namespace SMBC
{
	class LangDB
	{
	public:
		std::unordered_map<Uuid, std::wstring> m_Translations;

		LangDB() = default;

		void LoadLanguageFile(const std::wstring& path);

		std::wstring GetTranslation(const Uuid& uuid) const;
	};
}