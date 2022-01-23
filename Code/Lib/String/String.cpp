#include "String.h"

#include <Windows.h>
#include <vector>
#include <locale>
#include <cwctype>

namespace SMBC
{
	namespace String
	{
		std::string ToUtf8(const std::wstring& wstr)
		{
			int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
			std::string str(count, 0);
			WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
			return str;
		}

		std::wstring ToWide(const std::string& str)
		{
			int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
			std::wstring wstr(count, 0);
			MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], count);
			return wstr;
		}

		void ToLowerR(std::wstring& wstr)
		{
			for (wchar_t& w_char : wstr)
				w_char = std::towlower(w_char);
		}

		std::wstring ToLower(const std::wstring& wstr)
		{
			std::wstring output_wstr = wstr;
			String::ToLowerR(output_wstr);

			return output_wstr;
		}

		void ToUpperR(std::wstring& wstr)
		{
			for (wchar_t& w_char : wstr)
				w_char = std::towupper(w_char);
		}

		std::wstring ToUpper(const std::wstring& wstr)
		{
			std::wstring output_wstr = wstr;
			String::ToUpperR(output_wstr);

			return output_wstr;
		}

		void ReplaceR(std::wstring& orig_str, const wchar_t& to_replace, const wchar_t& replacer)
		{
			std::size_t cIdx = 0;
			while ((cIdx = orig_str.find(to_replace)) != std::wstring::npos)
				orig_str[cIdx] = replacer;
		}

		std::wstring Replace(const std::wstring& orig_str, const wchar_t& to_replace, const wchar_t& replacer)
		{
			std::wstring output_wstr = orig_str;
			String::ReplaceR(output_wstr, to_replace, replacer);

			return output_wstr;
		}

		std::string FloatVecToString(const float* f, const std::size_t& amount, const std::string& separator)
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
		bool IsLetterAllowed(const wchar_t& ch)
		{
			if (System::Char::IsLetterOrDigit(ch)) return true;

			for (const wchar_t& _c : _allowed_letters)
				if (ch == _c) return true;

			return false;
		}

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

		void Combine(std::string& mainStr, const std::string& curArg)
		{
			mainStr.append(curArg);
		}

		void Combine(std::string& mainStr, const char* curArg)
		{
			mainStr.append(curArg);
		}

		void Combine(std::string& mainStr, const std::wstring& curArg)
		{
			mainStr.append(SMBC::String::ToUtf8(curArg));
		}

		void Combine(std::string& mainStr, const float& curArg)
		{
			mainStr.append(std::to_string(curArg));
		}

		void Combine(std::string& mainStr, const double& curArg)
		{
			mainStr.append(std::to_string(curArg));
		}

		void Combine(std::string& mainStr, const unsigned long long& curArg)
		{
			mainStr.append(std::to_string(curArg));
		}

		void Combine(std::string& mainStr, const long long& curArg)
		{
			mainStr.append(std::to_string(curArg));
		}

		void Combine(std::string& mainStr, const unsigned long& curArg)
		{
			mainStr.append(std::to_string(curArg));
		}

		void Combine(std::string& mainStr, const long& curArg)
		{
			mainStr.append(std::to_string(curArg));
		}

		void Combine(std::string& mainStr, const unsigned int& curArg)
		{
			mainStr.append(std::to_string(curArg));
		}

		void Combine(std::string& mainStr, const int& curArg)
		{
			mainStr.append(std::to_string(curArg));
		}

		void Combine(std::string& mainStr, const unsigned char& curArg)
		{
			mainStr.append(std::to_string(curArg));
		}


		void Combine(std::wstring& mainStr, const std::wstring& curArg)
		{
			mainStr.append(curArg);
		}

		void Combine(std::wstring& mainStr, const wchar_t* curArg)
		{
			mainStr.append(curArg);
		}

		void Combine(std::wstring& mainStr, const std::string& curArg)
		{
			mainStr.append(SMBC::String::ToWide(curArg));
		}

		void Combine(std::wstring& mainStr, const float& curArg)
		{
			mainStr.append(std::to_wstring(curArg));
		}

		void Combine(std::wstring& mainStr, const double& curArg)
		{
			mainStr.append(std::to_wstring(curArg));
		}

		void Combine(std::wstring& mainStr, const unsigned long long& curArg)
		{
			mainStr.append(std::to_wstring(curArg));
		}

		void Combine(std::wstring& mainStr, const long long& curArg)
		{
			mainStr.append(std::to_wstring(curArg));
		}

		void Combine(std::wstring& mainStr, const unsigned long& curArg)
		{
			mainStr.append(std::to_wstring(curArg));
		}

		void Combine(std::wstring& mainStr, const long& curArg)
		{
			mainStr.append(std::to_wstring(curArg));
		}

		void Combine(std::wstring& mainStr, const unsigned int& curArg)
		{
			mainStr.append(std::to_wstring(curArg));
		}

		void Combine(std::wstring& mainStr, const int& curArg)
		{
			mainStr.append(std::to_wstring(curArg));
		}

		void Combine(std::wstring& mainStr, const unsigned char& curArg)
		{
			mainStr.append(std::to_wstring(curArg));
		}
	}
}