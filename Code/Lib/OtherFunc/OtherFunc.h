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
		std::string VecToString(glm::vec3& vec3);

		const static std::vector<wchar_t> _allowed_letters = {
			L'(', L')', L'.', L' ', L'_', 0x32, L'[', L']', L'-'
		};
		bool IsLetterAllowed(const wchar_t& ch);
	}
}