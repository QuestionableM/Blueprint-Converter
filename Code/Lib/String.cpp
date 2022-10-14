#include "String.h"

#include "Lib\WinInclude.h"

#include <vector>
#include <locale>

namespace SMBC
{
	std::string String::ToUtf8(const std::wstring& wstr)
	{
		int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), NULL, 0, NULL, NULL);
		std::string str(count, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
		return str;
	}

	std::wstring String::ToWide(const std::string& str)
	{
		int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0);
		std::wstring wstr(count, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], count);
		return wstr;
	}

	void String::ReplaceR(std::wstring& orig_str, const wchar_t& to_replace, const wchar_t& replacer)
	{
		std::size_t cIdx = 0;
		while ((cIdx = orig_str.find(to_replace)) != std::wstring::npos)
			orig_str[cIdx] = replacer;
	}

	std::wstring String::Replace(const std::wstring& orig_str, const wchar_t& to_replace, const wchar_t& replacer)
	{
		std::wstring output_wstr = orig_str;
		String::ReplaceR(output_wstr, to_replace, replacer);

		return output_wstr;
	}

	std::string String::FloatVecToString(const float* f, const std::size_t& amount, const std::string& separator)
	{
		std::string out;

		for (std::size_t a = 0; a < amount; a++)
		{
			if (a > 0) out.append(separator);

			out.append(std::to_string(f[a]));
		}

		return out;
	}

	const static std::vector<wchar_t> _allowed_letters = { L'(', L')', L'.', L' ', L'_', 0x32, L'[', L']', L'-' };
	bool String::IsLetterAllowed(const wchar_t& ch)
	{
		if (System::Char::IsLetterOrDigit(ch)) return true;

		for (const wchar_t& _c : _allowed_letters)
			if (ch == _c) return true;

		return false;
	}

	std::wstring String::ReadRegistryKey(const std::wstring& main_key, const std::wstring& sub_key)
	{
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
}