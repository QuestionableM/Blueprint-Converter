#include "Lib\WinInclude.h"

#include "Gui/MainGUI.h"
#include "DebugCon.h"

#include <locale>

namespace SMBC
{
	using namespace BlueprintConverter;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CreateDebugConsole();

	std::setlocale(LC_CTYPE, "en_US.UTF-8");
	CoUninitialize();

	SMBC::Application::EnableVisualStyles();
	SMBC::Application::SetCompatibleTextRenderingDefault(false);
	SMBC::Application::Run(gcnew SMBC::MainGUI);

	return 0;
}