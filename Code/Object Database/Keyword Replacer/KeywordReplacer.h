#pragma once

#include <string>
#include <unordered_map>

namespace SMBC {
	class PathReplacer {
		static std::unordered_map<std::wstring, std::wstring> PathReplacements;
		static std::unordered_map<std::wstring, std::wstring> KeyReplacements;
		static std::wstring _ModDataReplacement;
		static std::wstring _ContentDataReplacement;

		static bool ReplaceKeyInternal(std::wstring& path);
	public:
		static void ReadResourceUpgrades(const std::wstring& path);
		static std::wstring GetResourceUpgrade(const std::wstring& path);
		static std::wstring ToLowercase(const std::wstring& path);
		static void ToLowercaseR(std::wstring& path);

		static void CreateKey(std::wstring& key, std::wstring& value);

		static void Add(const std::wstring& key, const std::wstring& value);
		static void ClearAllData();

		static void SetModData(const std::wstring& path, const std::wstring& uuid);

		static void ReplaceAll(std::wstring& wstr, const wchar_t& to_replace, const wchar_t& replacer);

		static std::wstring ReplaceKey(const std::wstring& str);

		static void RemoveNewLineCharacters(std::wstring& str);
	};
}