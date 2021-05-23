#include "FileFunc.h"
#include <filesystem>

namespace fs = std::filesystem;
namespace _File = SMBC::FILE;

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

bool _File::FileExists(const std::wstring& path) {
	try {
		bool _Exists = fs::exists(path);
		return _Exists;
	}
	catch (fs::filesystem_error& _err) { fs::filesystem_error& a = _err; }

	return false;
}

bool _File::IsEquivalent(const std::wstring& p1, const std::wstring& p2) {
	try {
		if (fs::equivalent(p1, p2)) return true;
	}
	catch (fs::filesystem_error& fe) { fs::filesystem_error& a = fe; }

	return false;
}