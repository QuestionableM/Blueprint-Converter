#include "FileFunc.h"
#include <filesystem>

#include "DebugCon.h"

namespace fs = std::filesystem;
namespace _File = SMBC::File;

bool _File::IsDirectory(const std::wstring& path) {
	fs::directory_entry _path(path);

	return _path.is_directory();
}

bool _File::IsBadPath(const std::wstring& path) {
	try {
		bool exists = fs::exists(path);
	}
	catch (...) { return true; }

	return false;
}

bool _File::Exists(const std::wstring& path) {
	try {
		bool _Exists = fs::exists(path);
		return _Exists;
	}
	catch (...) {}

	return false;
}

bool _File::IsEquivalent(const std::wstring& p1, const std::wstring& p2) {
	try {
		if (fs::equivalent(p1, p2)) return true;
	}
	catch (...) {}

	return false;
}

bool _File::SafeCreateDir(const std::wstring& path) {
	try {
		fs::directory_entry dir_entr(path);
		if (dir_entr.exists() && !dir_entr.is_directory())
			fs::remove(dir_entr.path());

		fs::create_directory(path);
		return true;
	}
	catch (...) {}

	return false;
}

bool _File::GetCanonicalPath(std::wstring& path) {
	try {
		std::error_code op_error;
		const fs::path fullPath = fs::canonical(path, op_error);

		if (!op_error) {
			path = fullPath.wstring();
			return true;
		}
	}
	catch (...) {}

	return false;
}

std::wstring _File::GetCanonicalPathW(const std::wstring& path) {
	WCHAR path_buffer[MAX_PATH];
	DWORD str_len = GetFullPathNameW(path.c_str(), MAX_PATH, path_buffer, NULL);

	return std::wstring(path_buffer);
}