#if _DEBUG
#include "DebugCon.h"

namespace SMBC
{
	HANDLE Console::Handle = NULL;

	void Console::Output(const wchar_t* text)
	{
		WriteConsoleW(Console::Handle, text, (DWORD)wcslen(text), NULL, NULL);
	}

	void Console::Output(const char* text)
	{
		WriteConsoleA(Console::Handle, text, (DWORD)strlen(text), NULL, NULL);
	}

	void Console::Output(const std::wstring& text)
	{
		WriteConsoleW(Console::Handle, text.c_str(), (DWORD)text.length(), NULL, NULL);
	}

	void Console::Output(const std::string& text)
	{
		WriteConsoleA(Console::Handle, text.c_str(), (DWORD)text.length(), NULL, NULL);
	}

	void Console::Output(const unsigned long long& number)
	{
		Console::Output(std::to_string(number));
	}

	void Console::Output(const long long& number)
	{
		Console::Output(std::to_string(number));
	}

	void Console::Output(const unsigned long& number)
	{
		Console::Output(std::to_string(number));
	}

	void Console::Output(const long& number)
	{
		Console::Output(std::to_string(number));
	}

	void Console::Output(const unsigned int& number)
	{
		Console::Output(std::to_string(number));
	}

	void Console::Output(const int& number)
	{
		Console::Output(std::to_string(number));
	}

	void Console::Output(const double& number)
	{
		Console::Output(std::to_string(number));
	}

	void Console::Output(const float& number)
	{
		Console::Output(std::to_string(number));
	}

	void Console::Output(const SMBC::Color& color)
	{
		SetConsoleTextAttribute(Console::Handle, static_cast<WORD>(color));
	}

	void Console::Output(const bool& boolean)
	{
		Console::Output(boolean ? "true" : "false");
	}


	bool Console::Create(const wchar_t* title)
	{
		if (Console::Handle != NULL) return false;

		BOOL c_Created = AllocConsole();
		if (c_Created == FALSE) return false;

		SetConsoleTitleW(title);
		SetConsoleOutputCP(CP_UTF8);

		Console::Handle = GetStdHandle(STD_OUTPUT_HANDLE);

		return true;
	}

	void Console::Destroy()
	{
		if (Console::Handle == NULL) return;

		FreeConsole();
		Console::Handle = NULL;
	}

	__ConsoleOutputHandler Console::Out = __ConsoleOutputHandler();
}

#endif