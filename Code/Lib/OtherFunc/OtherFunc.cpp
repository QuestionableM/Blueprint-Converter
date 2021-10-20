#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/String/String.h"

#include <Windows.h>

namespace SMBC
{
	namespace Other
	{
		std::wstring ReadRegistryKey(const std::wstring& main_key, const std::wstring& sub_key) {
			wchar_t _Data[255] = {};
			DWORD _BufSz = 8196;

			LSTATUS _Status = RegGetValueW(
				HKEY_CURRENT_USER,
				main_key.c_str(),
				sub_key.c_str(),
				RRF_RT_REG_SZ,
				NULL,
				(PVOID)&_Data,
				&_BufSz
			);

			if (_Status == ERROR_SUCCESS)
				return std::wstring(_Data);

			return L"";
		}

		std::string VecToString(glm::vec3& vec3) {
			std::string _Output;
			SMBC::String::Combine(_Output, vec3.x, " ", vec3.y, " ", vec3.z);

			return _Output;
		}

		bool IsLetterAllowed(const wchar_t& ch) {
			if (System::Char::IsLetterOrDigit(ch)) return true;

			for (const wchar_t& _c : _allowed_letters)
				if (ch == _c) return true;

			return false;
		}
	}
}