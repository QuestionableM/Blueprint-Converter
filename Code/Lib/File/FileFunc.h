#pragma once

#include <string>

namespace SMBC {
	namespace FILE {
		bool IsDirectory(const std::wstring& path);
		bool IsBadPath(const std::wstring& path);

		bool FileExists(const std::wstring& path);
		bool IsEquivalent(const std::wstring& p1, const std::wstring& p2);
		bool SafeCreateDir(const std::wstring& path);
	}
}