#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <glm.hpp>

namespace SMBC {
	namespace Other {
		std::string WideToUtf8(const std::wstring& wstr);
		std::wstring Utf8ToWide(const std::string& str);
		std::wstring ReadRegistryKey(const std::wstring& main_key, const std::wstring& sub_key);
		bool FindEQInTable(std::vector<std::wstring>& vec, const std::wstring& key);
		std::string VecToString(glm::vec3& vec3);
	}
}