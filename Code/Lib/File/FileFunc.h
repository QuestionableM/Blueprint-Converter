#pragma once

#include <string>

namespace SMBC {
	namespace FILE {
		bool FileExists(const std::wstring& path);
		bool IsEquivalent(const std::wstring& p1, const std::wstring& p2);
	}
}