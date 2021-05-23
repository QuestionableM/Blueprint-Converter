#include "KeywordReplacer.h"

#include "Lib/Json/JsonFunc.h"
#include "Lib/OtherFunc/OtherFunc.h"

#include <msclr/marshal_cppstd.h>

SMBC::KeyReplacement::KeyReplacement(
	const std::wstring& key,
	const std::wstring& replacement
) {
	std::wstring _key_cpy = SMBC::PathReplacer::ToLowercase(key);
	std::wstring _repl_cpy = SMBC::PathReplacer::ToLowercase(replacement);

	SMBC::PathReplacer::ReplaceAll(_key_cpy, L'\\', L'/');
	SMBC::PathReplacer::ReplaceAll(_repl_cpy, L'\\', L'/');

	this->_Key = _key_cpy;
	this->_Replacement = _repl_cpy;
}

std::vector<SMBC::KeyReplacement> SMBC::PathReplacer::PathReplacements = {};
std::vector<SMBC::KeyReplacement> SMBC::PathReplacer::_Replacements = {};
std::wstring SMBC::PathReplacer::_ModDataReplacement = L"";
std::wstring SMBC::PathReplacer::_ContentDataReplacement = L"";

void SMBC::PathReplacer::ReadResourceUpgrades(const std::wstring& path) {
	nlohmann::json _json;
	if (!SMBC::JSON::OpenParseJson(path, _json)) return;
	auto& upgrades_json = _json;

	auto& upgrades = _json["upgrade"];
	if (!upgrades.is_array()) return;

	for (auto& upgrade_list : upgrades) {
		if (!upgrade_list.is_array()) continue;

		for (auto& upgrade_item : upgrade_list) {
			if (!upgrade_item.is_array()) continue;

			auto& key = upgrade_item[0];
			auto& replacement = upgrade_item[1];

			if (!key.is_string() || !replacement.is_string()) continue;

			std::wstring key_wstr = SMBC::Other::Utf8ToWide(key.get<std::string>());
			std::wstring repl_wstr = SMBC::Other::Utf8ToWide(replacement.get<std::string>());

			SMBC::PathReplacer::PathReplacements.push_back(SMBC::KeyReplacement(key_wstr, repl_wstr));
		}
	}
}

std::wstring SMBC::PathReplacer::GetResourceUpgrade(const std::wstring& path) {
	std::wstring _path_cpy = SMBC::PathReplacer::ToLowercase(path);
	SMBC::PathReplacer::ReplaceAll(_path_cpy, L'\\', L'/');

	for (SMBC::KeyReplacement& item : SMBC::PathReplacer::PathReplacements)
		if (item._Key == _path_cpy) return item._Replacement;

	return _path_cpy;
}

std::wstring SMBC::PathReplacer::ToLowercase(const std::wstring& path) {
	System::String^ _s_path = gcnew System::String(path.c_str());
	_s_path = _s_path->ToLower();

	return msclr::interop::marshal_as<std::wstring>(_s_path);
}

void SMBC::PathReplacer::Add(const SMBC::KeyReplacement& k_repl) {
	SMBC::PathReplacer::_Replacements.push_back(k_repl);
}

void SMBC::PathReplacer::ClearAllData() {
	SMBC::PathReplacer::PathReplacements.clear();
	SMBC::PathReplacer::_Replacements.clear();
	SMBC::PathReplacer::_ModDataReplacement.clear();
	SMBC::PathReplacer::_ContentDataReplacement.clear();
}

void SMBC::PathReplacer::SetModData(const std::wstring& path, const std::wstring& uuid) {
	SMBC::PathReplacer::_ModDataReplacement = path;
	SMBC::PathReplacer::_ContentDataReplacement = (L"$content_" + uuid);
}

void SMBC::PathReplacer::ReplaceAll(std::wstring& str, const wchar_t& to_replace, const wchar_t& replacer) {
	size_t _idx = 0;
	while ((_idx = str.find(to_replace)) != std::wstring::npos)
		str[_idx] = replacer;
}

std::wstring SMBC::PathReplacer::ReplaceKey(const std::wstring& str) {
	std::wstring str_copy = SMBC::PathReplacer::GetResourceUpgrade(str);

	for (SMBC::KeyReplacement& _Repl : SMBC::PathReplacer::_Replacements)
		if (str_copy.find(_Repl._Key) != std::wstring::npos)
			return (_Repl._Replacement + str_copy.substr(_Repl._Key.size()));

	std::wstring& _ContentKey = SMBC::PathReplacer::_ContentDataReplacement;
	if (str_copy.find(L"$mod_data") != std::wstring::npos)
		return (SMBC::PathReplacer::_ModDataReplacement + str_copy.substr(9));
	else if (str_copy.find(_ContentKey) != std::wstring::npos)
		return (SMBC::PathReplacer::_ModDataReplacement + str_copy.substr(_ContentKey.size()));

	return L"";
}

void SMBC::PathReplacer::RemoveNewLineCharacters(std::wstring& str) {
	SMBC::PathReplacer::ReplaceAll(str, L'\n', L' ');
	SMBC::PathReplacer::ReplaceAll(str, L'\r', L' ');
}