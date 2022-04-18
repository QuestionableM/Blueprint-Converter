#include "LanguageDatabase.h"

#include "ObjectDatabase\KeywordReplacer.h"

#include "Lib\JsonFunc.h"
#include "Lib\String.h"

namespace SMBC
{
	void LangDB::LoadLanguageFile(const std::wstring& path)
	{
		nlohmann::json _LangFile = Json::LoadParseJson(path);
		if (!_LangFile.is_object()) return;

		for (auto& trans : _LangFile.items())
		{
			if (!trans.value().is_object()) continue;

			const auto& _Title = Json::Get(trans.value(), "title");
			if (!_Title.is_string()) continue;

			std::wstring mWstrTitle = String::ToWide(_Title.get<std::string>());

			{
				//Remove the new line characters

				String::ReplaceR(mWstrTitle, L'\n', L' ');
				String::ReplaceR(mWstrTitle, L'\r', L' ');
			}

			this->AddTranslation(Uuid(trans.key()), mWstrTitle);
		}
	}

	void LangDB::AddTranslation(const Uuid& uuid, const std::wstring& trans)
	{
		if (this->Translations.find(uuid) == this->Translations.end())
			this->Translations.insert(std::make_pair(uuid, trans));
	}

	static const std::wstring BlockNotFoundString = L"BLOCK NOT FOUND";
	const std::wstring& LangDB::GetTranslation(const Uuid& uuid) const
	{
		if (this->Translations.find(uuid) != this->Translations.end())
			return this->Translations.at(uuid);

		return BlockNotFoundString;
	}
}