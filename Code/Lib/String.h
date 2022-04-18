#pragma once

#include <string>

namespace SMBC
{
	namespace String
	{
		std::string ToUtf8(const std::wstring& wstr);
		std::wstring ToWide(const std::string& str);

		void ToLowerR(std::wstring& wstr);
		std::wstring ToLower(const std::wstring& wstr);

		void ToUpperR(std::wstring& wstr);
		std::wstring ToUpper(const std::wstring& wstr);

		void ReplaceR(std::wstring& orig_str, const wchar_t& to_replace, const wchar_t& replacer);
		std::wstring Replace(const std::wstring& orig_str, const wchar_t& to_replace, const wchar_t& replacer);

		std::string FloatVecToString(const float* f, const std::size_t& amount, const std::string& separator = " ");
		bool IsLetterAllowed(const wchar_t& ch);

		std::wstring ReadRegistryKey(const std::wstring& main_key, const std::wstring& sub_key);

		void Combine(std::string& mainStr, const std::string& curArg);
		void Combine(std::string& mainStr, const char* curArg);
		void Combine(std::string& mainStr, const std::wstring& curArg);
		void Combine(std::string& mainStr, const float& curArg);
		void Combine(std::string& mainStr, const double& curArg);
		void Combine(std::string& mainStr, const unsigned long long& curArg);
		void Combine(std::string& mainStr, const long long& curArg);
		void Combine(std::string& mainStr, const unsigned long& curArg);
		void Combine(std::string& mainStr, const long& curArg);
		void Combine(std::string& mainStr, const unsigned int& curArg);
		void Combine(std::string& mainStr, const int& curArg);
		void Combine(std::string& mainStr, const unsigned char& curArg);

		void Combine(std::wstring& mainStr, const std::wstring& curArg);
		void Combine(std::wstring& mainStr, const wchar_t* curArg);
		void Combine(std::wstring& mainStr, const std::string& curArg);
		void Combine(std::wstring& mainStr, const float& curArg);
		void Combine(std::wstring& mainStr, const double& curArg);
		void Combine(std::wstring& mainStr, const unsigned long long& curArg);
		void Combine(std::wstring& mainStr, const long long& curArg);
		void Combine(std::wstring& mainStr, const unsigned long& curArg);
		void Combine(std::wstring& mainStr, const long& curArg);
		void Combine(std::wstring& mainStr, const unsigned int& curArg);
		void Combine(std::wstring& mainStr, const int& curArg);
		void Combine(std::wstring& mainStr, const unsigned char& curArg);


		template<typename StrType, typename CurArg, typename ...ArgList>
		inline void Combine(StrType& mainStr, const CurArg& curArg, const ArgList& ...argList)
		{
			SMBC::String::Combine(mainStr, curArg);
			SMBC::String::Combine(mainStr, argList...);
		}
	}
}