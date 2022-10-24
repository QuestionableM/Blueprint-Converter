#pragma once

#include "BuildConfig.hpp"

#ifdef SMBC_DEBUG_CONSOLE_ENABLED

#include "ConsoleColors.hpp"
#include <string>

namespace SMBC
{
	class __ConsoleOutputHandler;
	class Console
	{
		friend __ConsoleOutputHandler;

		static HANDLE Handle;

		static void Output(const wchar_t* text);
		static void Output(const char* text);
		static void Output(const std::wstring& text);
		static void Output(const std::string& text);

		static void Output(const unsigned long long& number);
		static void Output(const long long& number);
		static void Output(const unsigned long& number);
		static void Output(const long& number);
		static void Output(const unsigned int& number);
		static void Output(const int& number);
		static void Output(const double& number);
		static void Output(const float& number);

		static void Output(const ConsoleColor& color);

		static void Output(const bool& boolean);
	public:
		static bool Create(const wchar_t* title);
		static void Destroy();

		static __ConsoleOutputHandler Out;
	};

	class __ConsoleOutputHandler
	{
		friend Console;

		__ConsoleOutputHandler() = default;

		template<typename T>
		inline void variadic_func(const T& arg)
		{
			Console::Output(arg);
		}

		template<typename CurArg, typename ...ConArgs>
		inline void variadic_func(const CurArg& curArg, const ConArgs& ...argList)
		{
			this->variadic_func(curArg);
			this->variadic_func(argList...);
		}

	public:
		template<typename ...ConArgs>
		inline void operator()(const ConArgs& ...argList)
		{
			this->variadic_func(argList...);
		}
	};
}

#define DebugOut(...)        SMBC::Console::Out(__VA_ARGS__)
#define DebugOutL(...)       SMBC::Console::Out(__VA_ARGS__, 0b1110_fg, "\n")

#define DebugWarningL(...)   SMBC::Console::Out(0b1101_fg, "WARNING: ", __FUNCTION__, " (", __LINE__, ") -> ", __VA_ARGS__, 0b1110_fg, "\n")
#define DebugErrorL(...)     SMBC::Console::Out(0b1001_fg, "ERROR: "  , __FUNCTION__, " (", __LINE__, ") -> ", __VA_ARGS__, 0b1110_fg, "\n")

#define CreateDebugConsole() SMBC::Console::Create(L"SMBC Debug Console")

#else
#define DebugOut(...) ((void*)0)
#define DebugOutL(...) ((void*)0)
#define DebugWarningL(...) ((void*)0)
#define DebugErrorL(...) ((void*)0)
#define CreateDebugConsole() ((void*)0)
#endif