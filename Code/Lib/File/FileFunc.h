#pragma once

#include <string>

namespace SMBC {
	namespace FILE {
		public enum ExistsType {
			Exists = 0,
			NExists = 1,
			BadFile = 2
		};

		ExistsType FileExistsA(const std::wstring& path);
		bool IsBadPath(const std::wstring& path);

		bool FileExists(const std::wstring& path);
		bool IsEquivalent(const std::wstring& p1, const std::wstring& p2);
	}
}