#include "Object Database/ObjectDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/Functions/Functions.h"
#include "Lib/Json/JsonFunc.h"
#include "Lib/OtherFunc/OtherFunc.h"

SMBC::LangTrans::LangTrans(
	const std::wstring& uuid,
	const std::wstring& translation
) {
	this->uuid = uuid;
	this->translation = translation;
}

SMBC::LangDB::LangDB(const std::wstring& environment) {
	this->_Environment = environment;
}

void SMBC::LangDB::LoadLanguageFile(const std::wstring& path) {
	nlohmann::json* _LangFile = SMBC::JSON::OpenParseJson(path);

	if (!(_LangFile && _LangFile->is_object())) goto skip_other_actions;

	for (auto& trans : _LangFile->items()) {
		if (!trans.value().is_object()) continue;

		auto& _Title = trans.value()["title"];
		if (!_Title.is_string()) continue;

		std::wstring _WstrTitle = SMBC::Other::Utf8ToWide(_Title.get<std::string>());
		SMBC::PathReplacer::RemoveNewLineCharacters(_WstrTitle);

		std::wstring _WstrKey = SMBC::Other::Utf8ToWide(trans.key());

		this->_translations.push_back(SMBC::LangTrans(_WstrKey, _WstrTitle));
	}

skip_other_actions:
	delete _LangFile;
}

bool SMBC::LangDB::UuidExists(const std::wstring& uuid) {
	for (unsigned int a = 0; a < this->_translations.size(); a++)
		if (this->_translations[a].uuid == uuid) return true;

	return false;
}
void SMBC::LangDB::AddTranslation(const SMBC::LangTrans& translation) {
	if (this->UuidExists(translation.uuid)) return;

	this->_translations.push_back(translation);
}
SMBC::LangTrans* SMBC::LangDB::GetTranslation(const std::wstring& uuid) {
	for (unsigned int a = 0; a < this->_translations.size(); a++) {
		SMBC::LangTrans& _translation = this->_translations[a];
		if (_translation.uuid == uuid) return &_translation;
	}

	return nullptr;
}

std::wstring SMBC::LangDB::GetTranslationString(const std::wstring& uuid) {
	for (unsigned int a = 0; a < this->_translations.size(); a++) {
		SMBC::LangTrans& _Translation = this->_translations[a];
		if (_Translation.uuid == uuid) return _Translation.translation;
	}

	return L"";
}