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

void SMBC::LangDB::clear_database() {
	this->_Environment.clear();
	this->_translations.clear();
}

void SMBC::LangDB::LoadLanguageFile(const std::wstring& path) {
	nlohmann::json _LangFile;
	if (!SMBC::JSON::OpenParseJson(path, _LangFile) || !_LangFile.is_object()) return;

	for (auto& trans : _LangFile.items()) {
		if (!trans.value().is_object()) continue;

		auto& _Title = trans.value()["title"];
		if (!_Title.is_string()) continue;

		std::wstring _WstrTitle = SMBC::Other::Utf8ToWide(_Title.get<std::string>());
		SMBC::PathReplacer::RemoveNewLineCharacters(_WstrTitle);

		std::wstring _WstrKey = SMBC::Other::Utf8ToWide(trans.key());

		this->_translations.push_back(SMBC::LangTrans(_WstrKey, _WstrTitle));
	}
}

bool SMBC::LangDB::UuidExists(const std::wstring& uuid) {
	for (SMBC::LangTrans& transl : this->_translations)
		if (transl.uuid == uuid) return true;

	return false;
}

void SMBC::LangDB::AddTranslation(const SMBC::LangTrans& translation) {
	if (this->UuidExists(translation.uuid)) return;

	this->_translations.push_back(translation);
}

std::wstring SMBC::LangDB::GetTranslationString(const std::wstring& uuid) {
	for (SMBC::LangTrans& _trans : this->_translations)
		if (_trans.uuid == uuid) return _trans.translation;

	return L"BLOCK NOT FOUND";
}