#include "KeywordReplacer.h"

#include "Lib/Json/JsonFunc.h"
#include "Lib/String/String.h"
#include "Lib/File/FileFunc.h"

#include <cwctype>

#include "DebugCon.h"

typedef SMBC::PathReplacer _PathReplacer;

std::unordered_map<std::wstring, std::wstring> _PathReplacer::PathReplacements = {};
std::unordered_map<std::wstring, std::wstring> _PathReplacer::KeyReplacements = {};
std::wstring _PathReplacer::_ModDataReplacement = L"";
std::wstring _PathReplacer::_ContentDataReplacement = L"";

bool _PathReplacer::ReplaceKeyInternal(std::wstring& path)
{
	for (auto& k_ReplData : _PathReplacer::KeyReplacements)
		if (path.find(k_ReplData.first) != std::wstring::npos)
		{
			path = (k_ReplData.second + path.substr(k_ReplData.first.size()));
			return true;
		}

	const std::wstring& _ContentKey = _PathReplacer::_ContentDataReplacement;
	if (path.find(L"$mod_data") != std::wstring::npos)
	{
		path = (_PathReplacer::_ModDataReplacement + path.substr(9));
		return true;
	}
	else if (path.find(_ContentKey) != std::wstring::npos)
	{
		path = (_PathReplacer::_ModDataReplacement + path.substr(_ContentKey.size()));
		return true;
	}

	return false;
}

void _PathReplacer::ReadResourceUpgrades(const std::wstring& path)
{
	nlohmann::json _json = SMBC::Json::LoadParseJson(path);
	if (!_json.is_object()) return;

	const auto& upgrades = SMBC::Json::Get(_json, "upgrade");
	if (!upgrades.is_array()) return;

	for (auto& upgrade_list : upgrades)
	{
		if (!upgrade_list.is_array()) continue;

		for (auto& upgrade_item : upgrade_list)
		{
			if (!upgrade_item.is_array()) continue;

			const auto& key = SMBC::Json::Get(upgrade_item, 0);
			const auto& replacement = SMBC::Json::Get(upgrade_item, 1);

			if (!key.is_string() || !replacement.is_string()) continue;

			std::wstring key_wstr = SMBC::String::ToWide(key.get<std::string>());
			std::wstring repl_wstr = SMBC::String::ToWide(replacement.get<std::string>());

			_PathReplacer::CreateKey(key_wstr, repl_wstr);
			_PathReplacer::PathReplacements.insert(std::make_pair(key_wstr, repl_wstr));
		}
	}
}

std::wstring _PathReplacer::GetResourceUpgrade(const std::wstring& path)
{
	std::wstring _path_cpy = _PathReplacer::ToLowercase(path);
	_PathReplacer::ReplaceAll(_path_cpy, L'\\', L'/');

	if (_PathReplacer::PathReplacements.find(_path_cpy) != _PathReplacer::PathReplacements.end())
		return _PathReplacer::PathReplacements.at(_path_cpy);

	return _path_cpy;
}

std::wstring _PathReplacer::ToLowercase(const std::wstring& path)
{
	std::wstring _Output = path;

	for (wchar_t& curChar : _Output)
		curChar = std::towlower(curChar);

	return _Output;
}

void _PathReplacer::ToLowercaseR(std::wstring& path)
{
	for (wchar_t& curChar : path)
		curChar = std::towlower(curChar);
}

void _PathReplacer::CreateKey(std::wstring& key, std::wstring& value)
{
	key = _PathReplacer::ToLowercase(key);
	value = _PathReplacer::ToLowercase(value);

	_PathReplacer::ReplaceAll(key, L'\\', L'/');
	_PathReplacer::ReplaceAll(value, L'\\', L'/');
}

void _PathReplacer::Add(const std::wstring& key, const std::wstring& value)
{
	std::wstring key_cpy = key;
	std::wstring val_cpy = value;

	_PathReplacer::CreateKey(key_cpy, val_cpy);

	if (_PathReplacer::KeyReplacements.find(key_cpy) == _PathReplacer::KeyReplacements.end())
		_PathReplacer::KeyReplacements.insert(std::make_pair(key_cpy, val_cpy));
}

void _PathReplacer::ClearAllData()
{
	_PathReplacer::PathReplacements.clear();
	_PathReplacer::KeyReplacements.clear();
	_PathReplacer::_ModDataReplacement.clear();
	_PathReplacer::_ContentDataReplacement.clear();
}

void _PathReplacer::SetModData(const std::wstring& path, const std::wstring& uuid)
{
	_PathReplacer::_ModDataReplacement = path;
	_PathReplacer::_ContentDataReplacement = (L"$content_" + uuid);
}

void _PathReplacer::ReplaceAll(std::wstring& str, const wchar_t& to_replace, const wchar_t& replacer)
{
	size_t _idx = 0;
	while ((_idx = str.find(to_replace)) != std::wstring::npos)
		str[_idx] = replacer;
}

std::wstring _PathReplacer::ReplaceKey(const std::wstring& str)
{
	std::wstring final_path = _PathReplacer::GetResourceUpgrade(str);

	if (_PathReplacer::ReplaceKeyInternal(final_path))
	{
		std::wstring CanonicalPath = SMBC::File::GetCanonicalPathW(final_path);
		_PathReplacer::ToLowercaseR(CanonicalPath);

		return CanonicalPath;
	}

	return L"";
}

void _PathReplacer::RemoveNewLineCharacters(std::wstring& str)
{
	_PathReplacer::ReplaceAll(str, L'\n', L' ');
	_PathReplacer::ReplaceAll(str, L'\r', L' ');
}