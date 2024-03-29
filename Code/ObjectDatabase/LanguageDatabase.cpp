#include "LanguageDatabase.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"

#include "Lib\String.hpp"
#include "Lib\Json.hpp"

namespace SMBC
{
	void LangDB::LoadLanguageFile(const std::wstring& path)
	{
		const nlohmann::json lang_file = Json::LoadParseJson(path);
		if (!lang_file.is_object()) return;

		for (const auto& l_translation : lang_file.items())
		{
			if (!l_translation.value().is_object()) continue;

			const auto& l_title = Json::Get(l_translation.value(), "title");
			if (!l_title.is_string()) continue;

			std::wstring l_wide_title = String::ToWide(l_title.get_ref<const std::string&>());

			{
				//Remove the new line characters

				String::ReplaceR(l_wide_title, L'\n', L' ');
				String::ReplaceR(l_wide_title, L'\r', L' ');
			}

			const Uuid l_key_uuid(l_translation.key());
			if (m_Translations.find(l_key_uuid) == m_Translations.end())
				m_Translations.insert(std::make_pair(l_key_uuid, l_wide_title));
		}
	}
}