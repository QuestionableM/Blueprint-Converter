#include "File.h"

#include <Windows.h>
#include <ShlObj.h>

#include <filesystem>

#include "DebugCon.h"


namespace fs = std::filesystem;

namespace SMBC
{
	bool File::IsDirectory(const std::wstring& path)
	{
		std::error_code mError;
		const bool isDirectory = fs::is_directory(path, mError);

		return (!mError && isDirectory);
	}

	bool File::IsBadPath(const std::wstring& path)
	{
		std::error_code mError;
		const bool isExists = fs::exists(path, mError);

		return (bool)mError;
	}

	bool File::Exists(const std::wstring& path)
	{
		std::error_code mError;
		const bool isExists = fs::exists(path, mError);

		return (!mError && isExists);
	}

	bool File::IsEquivalent(const std::wstring& p1, const std::wstring& p2)
	{
		std::error_code mError;
		const bool isEquivalent = fs::equivalent(p1, p2, mError);

		return (!mError && isEquivalent);
	}

	bool File::SafeCreateDir(const std::wstring& path)
	{
		std::error_code mError;

		const bool isExists = fs::exists(path, mError);
		if (!mError && isExists)
		{
			const bool isDirectory = fs::is_directory(path, mError);
			if (!mError && !isDirectory)
				fs::remove(path, mError);
		}

		fs::create_directory(path, mError);

		return true;
	}

	bool File::GetCanonicalPath(std::wstring& path)
	{
		std::error_code op_error;
		const fs::path fullPath = fs::canonical(path, op_error);

		if (op_error) return false;

		path = fullPath.wstring();

		return true;
	}

	std::wstring File::GetCanonicalPathW(const std::wstring& path)
	{
		WCHAR path_buffer[MAX_PATH];
		DWORD str_len = GetFullPathNameW(path.c_str(), MAX_PATH, path_buffer, NULL);

		return std::wstring(path_buffer);
	}

	bool File::GetAppDataPath(std::wstring& mPath)
	{
		TCHAR szPath[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
		{
			mPath = std::wstring(szPath);

			return true;
		}

		DebugErrorL(__FUNCTION__, " -> Failed to get the path");
		return false;
	}
}