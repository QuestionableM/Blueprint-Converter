#include "Blueprint.h"

#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/File/FileFunc.h"

#include <filesystem>
#include <cwctype>

namespace fs = std::filesystem;
typedef SMBC::Blueprint _Blueprint;

std::vector<std::wstring> _Blueprint::ImageExtensions = { L".jpg", L".png", L".bmp" };

bool _Blueprint::IsSupportedExtension(const std::wstring& _ext)
{
	for (std::wstring& _Ext : SMBC::Blueprint::ImageExtensions)
		if (_Ext == _ext) return true;

	return false;
}

std::wstring _Blueprint::FixBlueprintName(const std::wstring& name)
{
	std::wstring _Output = L"";

	for (const wchar_t& _Letter : name)
	{
		if (SMBC::Other::IsLetterAllowed(_Letter))
			_Output += _Letter;
	}

	return _Output;
}

std::wstring _Blueprint::FindBlueprintImage()
{
	if (!SMBC::File::Exists(this->Folder)) return L"";

	fs::directory_iterator _Iter(this->Folder, fs::directory_options::skip_permission_denied);
	for (auto& Image : _Iter)
	{
		const fs::path& _img_p = Image.path();

		if (
			Image.is_regular_file() && _img_p.has_extension() &&
			this->IsSupportedExtension(_img_p.extension().wstring())
		) return _img_p.wstring();
	}

	return L"";
}

bool _Blueprint::BlueprintExists()
{
	bool _FolderExists = SMBC::File::Exists(this->Folder);
	bool _PathExists = SMBC::File::Exists(this->Path);

	return (_FolderExists && _PathExists);
}

_Blueprint::Blueprint(
	const std::wstring& name,
	const std::wstring& path,
	const std::wstring& folder,
	const std::wstring& workshop_id
) {
	this->Name = name;
	this->Path = path;
	this->Folder = folder;
	this->WorkshopId = workshop_id;

	this->LowerName = this->Name;
	for (wchar_t& curChar : this->LowerName)
		curChar = std::towlower(curChar);
}