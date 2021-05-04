#include <Windows.h>

#include "Gui/MainGUI.h"
#include "Object Database/ObjectDatabase.h"
#include "Lib/Functions/Functions.h"

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/GuiLib/GuiLib.h"

namespace SMBC {
	using namespace BlueprintConverter;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	CoUninitialize();

	SMBC::ObjectDatabase::LoadConfig(L"./Resources/Config.json");

	SMBC::Application::EnableVisualStyles();
	SMBC::Application::SetCompatibleTextRenderingDefault(false);
	SMBC::Application::Run(gcnew SMBC::MainGUI);

	SMBC::ObjectDatabase::ClearDatabase();

	return 0;
}