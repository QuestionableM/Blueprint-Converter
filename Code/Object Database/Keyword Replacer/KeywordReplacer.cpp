#include "KeywordReplacer.h"

#include "Lib/Json/JsonFunc.h"
#include "Lib/String/String.h"
#include "Lib/File/FileFunc.h"

#include <cwctype>

#include "DebugCon.h"

namespace SMBC
{
	std::unordered_map<std::wstring, std::wstring> PathReplacer::PathReplacements = {};
	std::unordered_map<std::wstring, std::wstring> PathReplacer::KeyReplacements  = {};

	bool PathReplacer::ReplaceKeyInternal(std::wstring& path)
	{
		for (auto& k_ReplData : PathReplacer::KeyReplacements)
			if (path.find(k_ReplData.first) != std::wstring::npos)
			{
				path = (k_ReplData.second + path.substr(k_ReplData.first.size()));
				return true;
			}

		return false;
	}

	void PathReplacer::ReadResourceUpgrades(const std::wstring& path)
	{
		nlohmann::json _json = Json::LoadParseJson(path);
		if (!_json.is_object()) return;

		const auto& upgrades = Json::Get(_json, "upgrade");
		if (!upgrades.is_array()) return;

		for (auto& upgrade_list : upgrades)
		{
			if (!upgrade_list.is_array()) continue;

			for (auto& upgrade_item : upgrade_list)
			{
				if (!upgrade_item.is_array()) continue;

				const auto& key = Json::Get(upgrade_item, 0);
				const auto& replacement = Json::Get(upgrade_item, 1);

				if (!key.is_string() || !replacement.is_string()) continue;

				std::wstring key_wstr  = String::ToWide(key.get<std::string>());
				std::wstring repl_wstr = String::ToWide(replacement.get<std::string>());

				PathReplacer::CreateKey(key_wstr, repl_wstr);
				PathReplacer::PathReplacements.insert(std::make_pair(key_wstr, repl_wstr));
			}
		}
	}

	std::wstring PathReplacer::GetResourceUpgrade(const std::wstring& path)
	{
		std::wstring _path_cpy = PathReplacer::ToLowercase(path);
		PathReplacer::ReplaceAll(_path_cpy, L'\\', L'/');

		if (PathReplacements.find(_path_cpy) != PathReplacements.end())
			return PathReplacements.at(_path_cpy);

		return _path_cpy;
	}

	std::wstring PathReplacer::ToLowercase(const std::wstring& path)
	{
		std::wstring _Output = path;

		for (wchar_t& curChar : _Output)
			curChar = std::towlower(curChar);

		return _Output;
	}

	void PathReplacer::ToLowercaseR(std::wstring& path)
	{
		for (wchar_t& curChar : path)
			curChar = std::towlower(curChar);
	}

	void PathReplacer::CreateKey(std::wstring& key, std::wstring& value)
	{
		key = PathReplacer::ToLowercase(key);
		value = PathReplacer::ToLowercase(value);

		PathReplacer::ReplaceAll(key, L'\\', L'/');
		PathReplacer::ReplaceAll(value, L'\\', L'/');
	}

	void PathReplacer::Add(const std::wstring& key, const std::wstring& value)
	{
		std::wstring key_cpy = key;
		std::wstring val_cpy = value;

		PathReplacer::CreateKey(key_cpy, val_cpy);

		if (KeyReplacements.find(key_cpy) == KeyReplacements.end())
			KeyReplacements.insert(std::make_pair(key_cpy, val_cpy));
	}

	void PathReplacer::ClearData()
	{
		PathReplacer::PathReplacements.clear();
		PathReplacer::KeyReplacements.clear();
	}

	void PathReplacer::SetModData(const std::wstring& path, const SMBC::Uuid& uuid)
	{
		if (KeyReplacements.find(L"$mod_data") != KeyReplacements.end())
		{
			KeyReplacements.at(L"$mod_data") = path;
		}
		else
		{
			KeyReplacements.insert(std::make_pair(L"$mod_data", path));
		}

		const std::wstring content_key = (L"$content_" + uuid.ToWstring());
		if (KeyReplacements.find(content_key) == KeyReplacements.end())
		{
			DebugOutL("Adding a new key: ", content_key);
			KeyReplacements.insert(std::make_pair(content_key, path));
		}
		else
		{
			DebugWarningL("SetModData: \"", content_key, "\" already exists!");
		}
	}

	void PathReplacer::ReplaceAll(std::wstring& str, const wchar_t& to_replace, const wchar_t& replacer)
	{
		size_t _idx = 0;
		while ((_idx = str.find(to_replace)) != std::wstring::npos)
			str[_idx] = replacer;
	}

	std::wstring PathReplacer::ReplaceKey(const std::wstring& str)
	{
		std::wstring final_path = PathReplacer::GetResourceUpgrade(str);

		if (PathReplacer::ReplaceKeyInternal(final_path))
		{
			std::wstring CanonicalPath = File::GetCanonicalPathW(final_path);
			PathReplacer::ToLowercaseR(CanonicalPath);

			return CanonicalPath;
		}

		return final_path;
	}

	void PathReplacer::RemoveNewLineCharacters(std::wstring& str)
	{
		PathReplacer::ReplaceAll(str, L'\n', L' ');
		PathReplacer::ReplaceAll(str, L'\r', L' ');
	}
}