#include "FileFunc.h"
#include <filesystem>

namespace fs = std::filesystem;
namespace _File = SMBC::FILE;

_File::ExistsType _File::FileExistsA(const std::wstring& path) {
	try {
		if (fs::exists(path))
			return _File::ExistsType::Exists;
	}
	catch (...) {
		return _File::ExistsType::BadFile;
	}

	return _File::ExistsType::NExists;
}

bool _File::IsBadPath(const std::wstring& path) {
	if (_File::FileExistsA(path) == _File::ExistsType::BadFile)
		return true;

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