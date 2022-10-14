#pragma once

#include <cwctype>
#include <string>

namespace SMBC
{
	namespace String
	{
		std::string ToUtf8(const std::wstring& wstr);
		std::wstring ToWide(const std::string& str);

		template<class StringType>
		inline void ToLowerR(StringType& r_str)
		{
			static_assert(std::_Is_any_of_v<StringType, std::string, std::wstring>, "ToLowerR can only be used with the following types: std::string, std::wstring");

			if constexpr (std::is_same_v<StringType, std::string>) {
				for (char& u_char : r_str)
					u_char = std::tolower(u_char);
			} else {
				for (wchar_t& w_char : r_str)
					w_char = std::towlower(w_char);
			}
		}

		template<class StringType>
		inline StringType ToLower(const StringType& r_str)
		{
			static_assert(std::_Is_any_of_v<StringType, std::string, std::wstring>, "ToLower can only be used with the following types: std::string, std::wstring");

			if constexpr (std::is_same_v<StringType, std::string>) {
				std::string v_output = r_str;

				for (char& u_char : v_output)
					u_char = std::tolower(u_char);

				return v_output;
			} else {
				std::wstring v_output = r_str;

				for (wchar_t& w_char : v_output)
					w_char = std::towlower(w_char);

				return v_output;
			}
		}

		template<class StringType>
		inline void ToUpperR(StringType& r_str)
		{
			static_assert(std::_Is_any_of_v<StringType, std::string, std::wstring>, "ToUpperR can only be used with the following types: std::string, std::wstring");

			if constexpr (std::is_same_v<StringType, std::string>) {
				for (char& u_char : r_str)
					u_char = std::toupper(u_char);
			} else {
				for (wchar_t& w_char : r_str)
					w_char = std::towupper(w_char);
			}
		}

		template<class StringType>
		inline StringType ToUpper(const StringType& r_str)
		{
			static_assert(std::_Is_any_of_v<StringType, std::string, std::wstring>, "ToUpper can only be used with the following types: std::string, std::wstring");

			if constexpr (std::is_same_v<StringType, std::string>) {
				std::string v_output = r_str;

				for (char& u_char : v_output)
					u_char = std::toupper(u_char);

				return v_output;
			} else {
				std::wstring v_output = r_str;

				for (wchar_t& w_char : v_output)
					w_char = std::towupper(w_char);

				return v_output;
			}
		}

		void ReplaceR(std::wstring& orig_str, const wchar_t& to_replace, const wchar_t& replacer);
		std::wstring Replace(const std::wstring& orig_str, const wchar_t& to_replace, const wchar_t& replacer);

		std::string FloatVecToString(const float* f, const std::size_t& amount, const std::string& separator = " ");
		bool IsLetterAllowed(const wchar_t& ch);

		std::wstring ReadRegistryKey(const std::wstring& main_key, const std::wstring& sub_key);

		inline void Combine(std::string& mainStr, const std::wstring& curArg) {
			mainStr.append(SMBC::String::ToUtf8(curArg));
		}

		inline void Combine(std::string& mainStr, const char* curArg) {
			mainStr.append(curArg);
		}

		inline void Combine(std::wstring& mainStr, const std::string& curArg) {
			mainStr.append(SMBC::String::ToWide(curArg));
		}

		inline void Combine(std::wstring& mainStr, const wchar_t* curArg) {
			mainStr.append(curArg);
		}

		template<typename StrType, typename ArgType>
		inline constexpr void Combine(StrType& mainStr, const ArgType& curArg)
		{
			static_assert(std::is_arithmetic_v<ArgType> || std::is_same_v<StrType, ArgType>, "Combine: Argument should be arithmetic or a string!");

			//Parse integer
			if constexpr (std::is_arithmetic_v<ArgType>) {
				if constexpr (std::is_same_v<StrType, std::wstring>) {
					mainStr.append(std::to_wstring(curArg));
				} else {
					mainStr.append(std::to_string(curArg));
				}
			}
			else if constexpr (std::is_same_v<StrType, ArgType>) {
				mainStr.append(curArg);
			}
		}


		template<typename StrType, typename CurArg, typename ...ArgList>
		inline void Combine(StrType& mainStr, const CurArg& curArg, const ArgList& ...argList)
		{
			static_assert(std::_Is_any_of_v<StrType, std::string, std::wstring>, "Combine only works with the following types: std::string, std::wstring");

			SMBC::String::Combine(mainStr, curArg);
			SMBC::String::Combine(mainStr, argList...);
		}
	}
}