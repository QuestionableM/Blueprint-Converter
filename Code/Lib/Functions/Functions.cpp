#include "Functions.h"

#include <sstream>
#include <fstream>
#include <filesystem>
#include <msclr/marshal_cppstd.h>

#include "Lib/File/FileFunc.h"

namespace fs = std::filesystem;

typedef SMBC::BPConvData _BPConvData;

long _BPConvData::Stage = -1;
long _BPConvData::ProgressBarMax = 0;
long _BPConvData::ProgressBarValue = 0;

void _BPConvData::SetNewStage(const long& Stage, const long& MaxValue) {
	_BPConvData::Stage = Stage;
	_BPConvData::ProgressBarValue = 0;
	_BPConvData::ProgressBarMax = MaxValue;
}

std::vector<std::wstring> SMBC::Blueprint::ImageExtensions = { L".jpg", L".png", L".bmp" };

bool SMBC::Blueprint::IsSupportedExtension(const std::wstring& _ext) {
	for (std::wstring& _Ext : SMBC::Blueprint::ImageExtensions)
		if (_Ext == _ext) return true;

	return false;
}

#include "Lib/OtherFunc/OtherFunc.h"

std::wstring SMBC::Blueprint::FixBlueprintName(const std::wstring& name) {
	std::wstring _Output = L"";

	for (const wchar_t& _Letter : name) {
		if (SMBC::Other::IsLetterAllowed(_Letter))
			_Output += _Letter;
	}

	return _Output;
}

std::wstring SMBC::Blueprint::FindBlueprintImage() {
	if (!SMBC::FILE::FileExists(this->BlueprintFolder)) return L"";

	fs::directory_iterator _Iter(this->BlueprintFolder, fs::directory_options::skip_permission_denied);
	for (auto& Image : _Iter) {
		const fs::path& _img_p = Image.path();

		if (
			Image.is_regular_file() && _img_p.has_extension() &&
			this->IsSupportedExtension(_img_p.extension().wstring())
		) return _img_p.wstring();
	}
	
	return L"";
}

bool SMBC::Blueprint::BlueprintExists() {
	bool _FolderExists = SMBC::FILE::FileExists(this->BlueprintFolder);
	bool _PathExists = SMBC::FILE::FileExists(this->BlueprintPath);

	return (_FolderExists && _PathExists);
}

SMBC::Blueprint::Blueprint(
	const std::wstring& name,
	const std::wstring& path,
	const std::wstring& folder,
	const std::wstring& workshop_id
) {
	this->BlueprintName = name;
	this->BlueprintPath = path;
	this->BlueprintFolder = folder;
	this->WorkshopId = workshop_id;
}