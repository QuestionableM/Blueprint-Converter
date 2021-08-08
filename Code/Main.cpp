#include <Windows.h>
#include <locale>

#include "Gui/MainGUI.h"
#include "Lib/ProgramSettings.h"
#include "Lib/Functions/Functions.h"

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/GuiLib/GuiLib.h"

#include "DebugCon.h"

namespace SMBC {
	using namespace BlueprintConverter;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#if _DEBUG
	if (!SMBC::Console::Create(L"Blueprint Converter Debug Console")) {
		System::Windows::Forms::MessageBox::Show(
			"Couldn't start the console!",
			"Fatal Error",
			System::Windows::Forms::MessageBoxButtons::OK,
			System::Windows::Forms::MessageBoxIcon::Error
		);

		return -1;
	}
#endif

	std::setlocale(LC_CTYPE, "en_US.UTF-8");
	CoUninitialize();

	SMBC::Settings::LoadSettingsFile();

	SMBC::Application::EnableVisualStyles();
	SMBC::Application::SetCompatibleTextRenderingDefault(false);
	SMBC::Application::Run(gcnew SMBC::MainGUI);

	return 0;
}