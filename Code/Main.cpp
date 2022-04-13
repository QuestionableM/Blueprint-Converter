#include <Windows.h>

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.h"

#include "Gui/MainGUI.h"
#include "Lib/ProgramSettings.h"
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

	SMBC::Settings::ReadConfig();
	SMBC::MaterialManager::Initialize();
	SMBC::Rotations::InitializeRotations();

	SMBC::Application::EnableVisualStyles();
	SMBC::Application::SetCompatibleTextRenderingDefault(false);
	SMBC::Application::Run(gcnew SMBC::MainGUI);

	return 0;
}