#include "GuiLib.hpp"

namespace SMBC
{
	std::wstring Gui::OpenFileName(
		const std::wstring& title,
		FILEOPENDIALOGOPTIONS options,
		LPCWSTR filter,
		HWND owner)
	{
		std::wstring _Output = L"";

		HRESULT hr = CoInitializeEx(NULL, COINIT_DISABLE_OLE1DDE | COINIT_APARTMENTTHREADED);

		if (SUCCEEDED(hr))
		{
			IFileOpenDialog* pFileOpen;

			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			if (SUCCEEDED(hr))
			{
				pFileOpen->SetOptions(options);
				pFileOpen->SetTitle(title.c_str());
				hr = pFileOpen->Show(owner);

				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						if (SUCCEEDED(hr))
						{
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
}