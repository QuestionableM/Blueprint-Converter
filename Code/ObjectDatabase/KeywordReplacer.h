#pragma once

#include <string>
#include <unordered_map>

#include "Lib\Uuid.h"

namespace SMBC {
	class PathReplacer {
		static std::unordered_map<std::wstring, std::wstring> PathReplacements;
		static std::unordered_map<std::wstring, std::wstring> KeyReplacements;

		static bool ReplaceKeyInternal(std::wstring& path);
	public:
		static void ReadResourceUpgrades(const std::wstring& path);
		static std::wstring GetResourceUpgrade(const std::wstring& path);

		static void CreateKey(std::wstring& key, std::wstring& value);

		static void Add(const std::wstring& key, const std::wstring& value);
		static void ClearData();

		static void SetModData(const std::wstring& path, const SMBC::Uuid& uuid);

		static std::wstring ReplaceKey(const std::wstring& str);

		static void RemoveNewLineCharacters(std::wstring& str);
	};
}