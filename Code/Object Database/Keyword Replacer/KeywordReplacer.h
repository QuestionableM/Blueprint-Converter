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
		static std::vector<SMBC::KeyReplacement> _Replacements;
		static std::wstring _ModDataReplacement;
		static std::wstring _ContentDataReplacement;
	public:
		static void AddKeyReplacement(const SMBC::KeyReplacement& k_repl);
		static void ClearAllData();

		static void SetModKey(const std::wstring& replacement);
		static void SetContentKey(const std::wstring& replacement);

		static void ReplaceAll(std::wstring& wstr, const wchar_t& to_replace, const wchar_t& replacer);
		static std::wstring ReplaceKey(const std::wstring& str);
		static void RemoveNewLineCharacters(std::wstring& str);
	};
}