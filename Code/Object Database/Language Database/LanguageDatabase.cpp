#include "LanguageDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/Json/JsonFunc.h"
#include "Lib/String/String.h"

namespace SMBC
{
	LangDB::LangDB(const std::wstring& environment)
	{
		this->Environment = environment;
	}

	void LangDB::ClearDatabase()
	{
		this->Environment.clear();
		this->Translations.clear();
	}

	void LangDB::LoadLanguageFile(const std::wstring& path)
	{
		nlohmann::json _LangFile = Json::LoadParseJson(path);
		if (!_LangFile.is_object()) return;

		for (auto& trans : _LangFile.items())
		{
			if (!trans.value().is_object()) continue;

			const auto& _Title = Json::Get(trans.value(), "title");
			if (!_Title.is_string()) continue;

			std::wstring _WstrTitle = String::ToWide(_Title.get<std::string>());
			PathReplacer::RemoveNewLineCharacters(_WstrTitle);

			this->AddTranslation(
				Uuid(trans.key()),
				_WstrTitle
			);
		}
	}

	void LangDB::AddTranslation(const Uuid& uuid, const std::wstring& trans)
	{
		if (this->Translations.find(uuid) == this->Translations.end())
			this->Translations.insert(std::make_pair(uuid, trans));
	}

	static const std::wstring BlockNotFoundString = L"BLOCK NOT FOUND";
	const std::wstring& LangDB::GetTranslation(const Uuid& uuid)
	{
		if (this->Translations.find(uuid) != this->Translations.end())
			return this->Translations.at(uuid);

		return BlockNotFoundString;
	}
}