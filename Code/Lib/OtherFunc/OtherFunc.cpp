#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/File/FileFunc.h"

#include <Windows.h>

namespace _Other = SMBC::Other;

std::string _Other::WideToUtf8(const std::wstring& wstr) {
	int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
	std::string str(count, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
	return str;
}

std::wstring _Other::Utf8ToWide(const std::string& str) {
	int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0);
	std::wstring wstr(count, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], count);
	return wstr;
}

std::wstring _Other::ReadRegistryKey(const std::wstring& main_key, const std::wstring& sub_key) {
	wchar_t _Data[255] = {};
	DWORD _BufSz = 8196;

	LSTATUS _Status = RegGetValueW(
		HKEY_CURRENT_USER,
		main_key.c_str(),
		sub_key.c_str(),
		RRF_RT_REG_SZ,
		NULL,
		(PVOID)&_Data,
		&_BufSz
	);

	if (_Status == ERROR_SUCCESS)
		return std::wstring(_Data);

	return L"";
}

bool _Other::FindEQInTable(std::vector<std::wstring>& vec, const std::wstring& key) {
	for (std::wstring& v : vec)
		if (v == key || SMBC::FILE::IsEquivalent(v, key)) return true;

	return false;
}

std::string _Other::VecToString(glm::vec3& vec3) {
	std::string _Output;
	_Output.append(std::to_string(vec3.x));
	_Output.append(" ");
	_Output.append(std::to_string(vec3.y));
	_Output.append(" ");
	_Output.append(std::to_string(vec3.z));

	return _Output;
}