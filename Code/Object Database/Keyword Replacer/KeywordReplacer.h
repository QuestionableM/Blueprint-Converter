#pragma once

#include <string>
#include <vector>

namespace SMBC {
	class KeyReplacement {
	public:
		std::wstring _Key;
		std::wstring _Replacement;

		KeyReplacement(const std::wstring& key, const std::wstring& replacement);
	};

	class PathReplacer {
		static std::vector<SMBC::KeyReplacement> PathReplacements;
		static std::vector<SMBC::KeyReplacement> _Replacements;
		static std::wstring _ModDataReplacement;
		static std::wstring _ContentDataReplacement;
	public:
		static void ReadResourceUpgrades(const std::wstring& path);
		static std::wstring GetResourceUpgrade(const std::wstring& path);
		static std::wstring ToLowercase(const std::wstring& path);

		static void Add(const SMBC::KeyReplacement& k_repl);
		static void ClearAllData();

		static void SetModData(const std::wstring& path, const std::wstring& uuid);

		static void ReplaceAll(std::wstring& wstr, const wchar_t& to_replace, const wchar_t& replacer);
		static std::wstring ReplaceKey(const std::wstring& str);
		static void RemoveNewLineCharacters(std::wstring& str);
	};
}