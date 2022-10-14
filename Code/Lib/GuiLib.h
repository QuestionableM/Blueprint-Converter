#pragma once

#include "Lib\WinInclude.h"
#include <shobjidl.h>

#include <string>

namespace WForms = System::Windows::Forms;

namespace SMBC
{
	namespace Gui
	{
		std::wstring OpenFileName(
			const std::wstring& title,
			FILEOPENDIALOGOPTIONS options = 0,
			LPCWSTR filter = L"All Files (*.*)\0*.*\0",
			HWND owner = NULL
		);

		inline void OpenFolderInExplorer(const std::wstring& path)
		{
			System::Diagnostics::Process::Start("explorer.exe", gcnew System::String(path.c_str()));
		}

		inline WForms::DialogResult Message(
			System::String^ title,
			System::String^ description,
			WForms::MessageBoxButtons buttons = WForms::MessageBoxButtons::OK,
			WForms::MessageBoxIcon icon = WForms::MessageBoxIcon::None)
		{
			return WForms::MessageBox::Show(description, title, buttons, icon);
		}

		inline WForms::DialogResult Error(System::String^ title, System::String^ description)
		{
			return Gui::Message(
				title, description,
				WForms::MessageBoxButtons::OK,
				WForms::MessageBoxIcon::Error
			);
		}

		inline WForms::DialogResult Warning(System::String^ title, System::String^ description)
		{
			return Gui::Message(
				title, description,
				WForms::MessageBoxButtons::OK,
				WForms::MessageBoxIcon::Warning
			);
		}

		inline WForms::DialogResult Question(System::String^ title, System::String^ description)
		{
			return Gui::Message(
				title, description,
				WForms::MessageBoxButtons::YesNo,
				WForms::MessageBoxIcon::Question
			);
		}
	};
}