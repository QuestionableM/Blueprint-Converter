#include "GuiLib.h"

std::wstring SMBC::GUI::OpenFileName(
	const std::wstring& title,
	FILEOPENDIALOGOPTIONS options,
	LPCWSTR filter,
	HWND owner
) {
	std::wstring _Output = L"";

	HRESULT hr = CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);

	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			pFileOpen->SetOptions(options);
			pFileOpen->SetTitle(title.c_str());
			hr = pFileOpen->Show(owner);

			if (SUCCEEDED(hr)) {
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					if (SUCCEEDED(hr)) {
						_Output = std::wstring(pszFilePath);
						CoTaskMemFree(pszFilePath);
					}

					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

	return _Output;
}

void SMBC::GUI::OpenFolderInExplorer(const std::wstring& path) {
	System::Diagnostics::Process::Start("explorer.exe", gcnew System::String(path.c_str()));
}

WForms::DialogResult SMBC::GUI::Message(
	System::String^ title,
	System::String^ description,
	WForms::MessageBoxButtons buttons,
	WForms::MessageBoxIcon icon
) {
	WForms::DialogResult _result = WForms::MessageBox::Show(
		description, title, buttons, icon
	);

	return _result;
}

WForms::DialogResult SMBC::GUI::Error(
	System::String^ title,
	System::String^ description
) {
	WForms::DialogResult _result = SMBC::GUI::Message(
		title, description,
		WForms::MessageBoxButtons::OK,
		WForms::MessageBoxIcon::Error
	);

	return _result;
}

WForms::DialogResult SMBC::GUI::Warning(
	System::String^ title,
	System::String^ description
) {
	WForms::DialogResult _result = SMBC::GUI::Message(
		title, description,
		WForms::MessageBoxButtons::OK,
		WForms::MessageBoxIcon::Warning
	);

	return _result;
}

WForms::DialogResult SMBC::GUI::Question(
	System::String^ title,
	System::String^ description
) {
	WForms::DialogResult _result = SMBC::GUI::Message(
		title, description,
		WForms::MessageBoxButtons::YesNo,
		WForms::MessageBoxIcon::Question
	);

	return _result;
}