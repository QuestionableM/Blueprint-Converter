#include <Windows.h>
#include <locale>

#include "Gui/MainGUI.h"
#include "Lib/ProgramSettings.h"

#include "DebugCon.h"

namespace SMBC {
	using namespace BlueprintConverter;
}

#include "Lib/File/FileFunc.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CreateDebugConsole();

	std::setlocale(LC_CTYPE, "en_US.UTF-8");
	CoUninitialize();

	SMBC::Settings::LoadSettingsFile();

	SMBC::Application::EnableVisualStyles();
	SMBC::Application::SetCompatibleTextRenderingDefault(false);
	SMBC::Application::Run(gcnew SMBC::MainGUI);

	return 0;
}