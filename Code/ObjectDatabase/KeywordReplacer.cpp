#include "KeywordReplacer.h"

#include "Lib\JsonFunc.h"
#include "Lib\String.h"
#include "Lib\FileFunc.h"

namespace SMBC
{
	std::unordered_map<std::wstring, std::wstring> PathReplacer::PathReplacements = {};
	std::unordered_map<std::wstring, std::wstring> PathReplacer::KeyReplacements  = {};

	bool PathReplacer::ReplaceKeyInternal(std::wstring& path)
	{
		for (const auto& k_ReplData : PathReplacer::KeyReplacements)
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
		std::wstring _path_cpy = String::ToLower(path);
		String::ReplaceR(_path_cpy, L'\\', L'/');

		if (PathReplacements.find(_path_cpy) != PathReplacements.end())
			return PathReplacements.at(_path_cpy);

		return _path_cpy;
	}

	void PathReplacer::CreateKey(std::wstring& key, std::wstring& value)
	{
		String::ToLowerR(key);
		String::ToLowerR(value);

		String::ReplaceR(key, L'\\', L'/');
		String::ReplaceR(value, L'\\', L'/');
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
			KeyReplacements.insert(std::make_pair(content_key, path));
	}

	std::wstring PathReplacer::ReplaceKey(const std::wstring& str)
	{
		std::wstring final_path = PathReplacer::GetResourceUpgrade(str);

		if (PathReplacer::ReplaceKeyInternal(final_path))
		{
			std::wstring CanonicalPath = File::GetCanonicalPathW(final_path);
			String::ToLowerR(CanonicalPath);

			return CanonicalPath;
		}

		return final_path;
	}

	void PathReplacer::RemoveNewLineCharacters(std::wstring& str)
	{
		String::ReplaceR(str, L'\n', L' ');
		String::ReplaceR(str, L'\r', L' ');
	}
}