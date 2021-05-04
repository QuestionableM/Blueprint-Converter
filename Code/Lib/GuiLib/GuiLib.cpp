#include "GuiLib.h"
#include <shobjidl.h>

std::wstring SMBC::GUI::OpenFileName(LPCWSTR filter, HWND owner) {
	std::wstring _Output = L"";

	HRESULT hr = CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);

	if (SUCCEEDED(hr)) {
		IFileOpenDialog* pFileOpen;

		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr)) {
			pFileOpen->SetTitle(L"Select Blueprint File");
			hr = pFileOpen->Show(NULL);

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