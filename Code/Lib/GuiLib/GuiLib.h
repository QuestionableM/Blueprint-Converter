#pragma once

#include <Windows.h>
#include <string>

namespace SMBC {
	namespace GUI {
		std::wstring OpenFileName(LPCWSTR filter = L"All Files (*.*)\0*.*\0", HWND owner = NULL);
		void OpenFolderInExplorer(const std::wstring& path);
	};
}