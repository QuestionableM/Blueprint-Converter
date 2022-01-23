#include <Windows.h>
#include <locale>

#include "Gui/MainGUI.h"
#include "Lib/ProgramSettings.h"

#include "Object Database/Rotations/ObjectRotations.hpp"
#include "DebugCon.h"

namespace SMBC
{
	using namespace BlueprintConverter;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CreateDebugConsole();

	std::setlocale(LC_CTYPE, "en_US.UTF-8");
	CoUninitialize();

	SMBC::Settings::ReadConfig();
	SMBC::Rotations::InitializeRotations();

	SMBC::Application::EnableVisualStyles();
	SMBC::Application::SetCompatibleTextRenderingDefault(false);
	SMBC::Application::Run(gcnew SMBC::MainGUI);

	return 0;
}