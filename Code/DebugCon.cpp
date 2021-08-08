#if _DEBUG
#include "DebugCon.h"

typedef SMBC::Console _Console;

HANDLE _Console::Handle = NULL;

void _Console::Output(const wchar_t* text) {
	WriteConsoleW(_Console::Handle, text, (DWORD)wcslen(text), NULL, NULL);
}

void _Console::Output(const char* text) {
	WriteConsoleA(_Console::Handle, text, (DWORD)strlen(text), NULL, NULL);
}

void _Console::Output(const std::wstring& text) {
	WriteConsoleW(_Console::Handle, text.c_str(), (DWORD)text.length(), NULL, NULL);
}

void _Console::Output(const std::string& text) {
	WriteConsoleA(_Console::Handle, text.c_str(), (DWORD)text.length(), NULL, NULL);
}

void _Console::Output(const unsigned long long& number) {
	_Console::Output(std::to_string(number));
}

void _Console::Output(const long long& number) {
	_Console::Output(std::to_string(number));
}

void _Console::Output(const unsigned long& number) {
	_Console::Output(std::to_string(number));
}

void _Console::Output(const long& number) {
	_Console::Output(std::to_string(number));
}

void _Console::Output(const unsigned int& number) {
	_Console::Output(std::to_string(number));
}

void _Console::Output(const int& number) {
	_Console::Output(std::to_string(number));
}

void _Console::Output(const double& number) {
	_Console::Output(std::to_string(number));
}

void _Console::Output(const float& number) {
	_Console::Output(std::to_string(number));
}

void _Console::Output(const SMBC::Color& color) {
	SetConsoleTextAttribute(_Console::Handle, static_cast<WORD>(color));
}

void _Console::Output(const bool& boolean) {
	_Console::Output(boolean ? "true" : "false");
}



bool _Console::Create(const wchar_t* title) {
	if (_Console::Handle != NULL) return false;

	BOOL c_Created = AllocConsole();
	if (c_Created == FALSE) return false;

	SetConsoleTitleW(title);
	SetConsoleOutputCP(CP_UTF8);

	_Console::Handle = GetStdHandle(STD_OUTPUT_HANDLE);

	return true;
}

void _Console::Destroy() {
	if (_Console::Handle == NULL) return;

	FreeConsole();
	_Console::Handle = NULL;
}

SMBC::__ConsoleOutputHandler _Console::Out = SMBC::__ConsoleOutputHandler();

#endif