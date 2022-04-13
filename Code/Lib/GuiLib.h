#pragma once

#include <Windows.h>
#include <shobjidl.h>
#include <string>

namespace WForms = System::Windows::Forms;

namespace SMBC {
	namespace Gui {
		std::wstring OpenFileName(
			const std::wstring& title,
			FILEOPENDIALOGOPTIONS options = 0,
			LPCWSTR filter = L"All Files (*.*)\0*.*\0",
			HWND owner = NULL
		);
		void OpenFolderInExplorer(const std::wstring& path);
		
		WForms::DialogResult Message(
			System::String^ title,
			System::String^ description,
			WForms::MessageBoxButtons buttons = WForms::MessageBoxButtons::OK,
			WForms::MessageBoxIcon icon = WForms::MessageBoxIcon::None
		);

		WForms::DialogResult Error(
			System::String^ title,
			System::String^ description
		);

		WForms::DialogResult Warning(
			System::String^ title,
			System::String^ description
		);

		WForms::DialogResult Question(
			System::String^ title,
			System::String^ description
		);
	};
}