#include "Object Database/ObjectDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/Functions/Functions.h"
#include "Lib/Json/JsonFunc.h"
#include "Lib/OtherFunc/OtherFunc.h"

typedef SMBC::LangDB _LangDB;

_LangDB::LangDB(const std::wstring& environment) {
	this->_Environment = environment;
}

void _LangDB::clear_database() {
	this->_Environment.clear();
	this->_Translations.clear();
}

void _LangDB::LoadLanguageFile(const std::wstring& path) {
	nlohmann::json _LangFile;

	if (!SMBC::Json::ParseJson(path, _LangFile) || !_LangFile.is_object())
		return;

	for (auto& trans : _LangFile.items()) {
		if (!trans.value().is_object()) continue;

		const auto& _Title = SMBC::Json::Get(trans.value(), "title");
		if (!_Title.is_string()) continue;

		std::wstring _WstrTitle = SMBC::Other::Utf8ToWide(_Title.get<std::string>());
		SMBC::PathReplacer::RemoveNewLineCharacters(_WstrTitle);

		this->AddTranslation(
			SMBC::Other::Utf8ToWide(trans.key()),
			_WstrTitle
		);
	}
}

void _LangDB::AddTranslation(const std::wstring& uuid, const std::wstring& trans) {
	if (this->_Translations.find(uuid) == this->_Translations.end())
		this->_Translations.insert(std::make_pair(uuid, trans));
}

static const std::wstring BlockNotFoundString = L"BLOCK NOT FOUND";
const std::wstring& _LangDB::GetTranslation(const std::wstring& uuid) {
	if (this->_Translations.find(uuid) != this->_Translations.end())
		return this->_Translations.at(uuid);

	return BlockNotFoundString;
}