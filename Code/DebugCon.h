#pragma once

#include <Windows.h>

#if _DEBUG
#include <string>
#endif

namespace SMBC {
	enum class Color : WORD {
		RED = FOREGROUND_RED,
		GREEN = FOREGROUND_GREEN,
		BLUE = FOREGROUND_BLUE,
		YELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
		PINK = FOREGROUND_RED | FOREGROUND_BLUE,
		CYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
		WHITE = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,

		RED_INT = FOREGROUND_RED | FOREGROUND_INTENSITY,
		GREEN_INT = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		BLUE_INT = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		YELLOW_INT = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		PINK_INT = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		CYAN_INT = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		WHITE_INT = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,

		RED_BG = BACKGROUND_RED,
		GREEN_BG = BACKGROUND_GREEN,
		BLUE_BG = BACKGROUND_BLUE,
		YELLOW_BG = BACKGROUND_RED | BACKGROUND_GREEN,
		PINK_BG = BACKGROUND_RED | BACKGROUND_BLUE,
		CYAN_BG = BACKGROUND_GREEN | BACKGROUND_BLUE,
		WHITE_BG = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,

		RED_BG_INT = BACKGROUND_RED | BACKGROUND_INTENSITY,
		GREEN_BG_INT = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		BLUE_BG_INT = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		YELLOW_BG_INT = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		PINK_BG_INT = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		CYAN_BG_INT = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		WHITE_BG_INT = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY
	};

#if _DEBUG
	class __ConsoleOutputHandler;
	class Console {
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

		static void Output(const Color& color);

		static void Output(const bool& boolean);
	public:
		static bool Create(const wchar_t* title);
		static void Destroy();

		static __ConsoleOutputHandler Out;
	};

	class __ConsoleOutputHandler {
		friend Console;

		__ConsoleOutputHandler() = default;

		template<typename T>
		inline void variadic_func(const T& arg) {
			Console::Output(arg);
		}

		template<typename CurArg, typename ...ConArgs>
		inline void variadic_func(const CurArg& curArg, const ConArgs& ...argList) {
			this->variadic_func(curArg);
			this->variadic_func(argList...);
		}

	public:
		template<typename ...ConArgs>
		inline void operator()(const ConArgs& ...argList) {
			this->variadic_func(argList...);
		}
	};

#define DebugOut(...) SMBC::Console::Out(__VA_ARGS__)
#define DebugOutL(...) SMBC::Console::Out(__VA_ARGS__, SMBC::Color::WHITE, "\n")
#else
#define DebugOut(...)
#define DebugOutL(...)
#endif
}