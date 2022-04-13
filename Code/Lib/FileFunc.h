#pragma once

#include <string>

namespace SMBC {
	namespace File {
		bool IsDirectory(const std::wstring& path);
		bool IsBadPath(const std::wstring& path);

		bool Exists(const std::wstring& path);
		bool IsEquivalent(const std::wstring& p1, const std::wstring& p2);
		bool SafeCreateDir(const std::wstring& path);
		bool GetCanonicalPath(std::wstring& path);
		std::wstring GetCanonicalPathW(const std::wstring& path);
	}
}