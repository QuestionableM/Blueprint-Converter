#include "KeywordReplacer.h"

SMBC::KeyReplacement::KeyReplacement(
	const std::wstring& key,
	const std::wstring& replacement
) {
	this->_Key = key;
	this->_Replacement = replacement;
}

std::vector<SMBC::KeyReplacement> SMBC::PathReplacer::_Replacements = {};
std::wstring SMBC::PathReplacer::_ModDataReplacement = L"";
std::wstring SMBC::PathReplacer::_ContentDataReplacement = L"";

void SMBC::PathReplacer::AddKeyReplacement(const SMBC::KeyReplacement& k_repl) {
	for (int a = 0; a < (int)SMBC::PathReplacer::_Replacements.size(); a++) {
		SMBC::KeyReplacement& _Repl = SMBC::PathReplacer::_Replacements[a];
		if (_Repl._Key == k_repl._Key) return;
	}

	SMBC::PathReplacer::_Replacements.push_back(k_repl);
}
void SMBC::PathReplacer::ClearAllData() {
	SMBC::PathReplacer::_Replacements.clear();
	SMBC::PathReplacer::_ModDataReplacement.clear();
	SMBC::PathReplacer::_ContentDataReplacement.clear();
}

void SMBC::PathReplacer::SetModKey(const std::wstring& replacement) {
	SMBC::PathReplacer::_ModDataReplacement = replacement;
}
void SMBC::PathReplacer::SetContentKey(const std::wstring& replacement) {
	SMBC::PathReplacer::_ContentDataReplacement = (L"$CONTENT_" + replacement);
}

void SMBC::PathReplacer::ReplaceAll(std::wstring& str, const wchar_t& to_replace, const wchar_t& replacer) {
	size_t _idx = 0;
	while ((_idx = str.find(to_replace)) != std::wstring::npos)
		str[_idx] = replacer;
}

std::wstring SMBC::PathReplacer::ReplaceKey(const std::wstring& str) {
	for (int a = 0; a < (int)SMBC::PathReplacer::_Replacements.size(); a++) {
		SMBC::KeyReplacement& _Repl = SMBC::PathReplacer::_Replacements[a];
		if (str.find(_Repl._Key) != std::wstring::npos)
			return (_Repl._Replacement + str.substr(_Repl._Key.size()));
	}

	std::wstring& _ContentKey = SMBC::PathReplacer::_ContentDataReplacement;
	if (str.find(L"$MOD_DATA") != std::wstring::npos)
		return (SMBC::PathReplacer::_ModDataReplacement + str.substr(9));
	else if (str.find(_ContentKey) != std::wstring::npos)
		return (SMBC::PathReplacer::_ModDataReplacement + str.substr(_ContentKey.size()));

	return L"";
}

void SMBC::PathReplacer::RemoveNewLineCharacters(std::wstring& str) {
	SMBC::PathReplacer::ReplaceAll(str, L'\n', L' ');
	SMBC::PathReplacer::ReplaceAll(str, L'\r', L' ');
}