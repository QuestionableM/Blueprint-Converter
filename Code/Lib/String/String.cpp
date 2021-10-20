#include "String.h"
#include <Windows.h>

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

		std::string HexToFloat(const std::string& hex_str)
		{
			float fr = (float)std::stoi(hex_str.substr(0, 2), nullptr, 16) / 255.0f;
			float fg = (float)std::stoi(hex_str.substr(2, 2), nullptr, 16) / 255.0f;
			float fb = (float)std::stoi(hex_str.substr(4, 2), nullptr, 16) / 255.0f;

			std::string _output;
			SMBC::String::Combine(_output, fr, " ", fg, " ", fb);

			return _output;
		}

		std::string HexToFloatW(const std::wstring& hex_wstr)
		{
			float fr = (float)std::stoi(hex_wstr.substr(0, 2), nullptr, 16) / 255.0f;
			float fg = (float)std::stoi(hex_wstr.substr(2, 2), nullptr, 16) / 255.0f;
			float fb = (float)std::stoi(hex_wstr.substr(4, 2), nullptr, 16) / 255.0f;

			std::string _output;
			SMBC::String::Combine(_output, fr, " ", fg, " ", fb);

			return _output;
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
	}
}