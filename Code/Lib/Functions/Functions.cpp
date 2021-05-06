#include "Functions.h"

#include <sstream>
#include <fstream>
#include <filesystem>
#include <msclr/marshal_cppstd.h>

#include "Lib/File/FileFunc.h"

namespace fs = std::filesystem;

long SMBC::BlueprintConversionData::Stage = -1;
long SMBC::BlueprintConversionData::ProgressBarMax = 0;
long SMBC::BlueprintConversionData::ProgressBarValue = 0;

void SMBC::BlueprintConversionData::SetNewStage(const long& Stage, const long& MaxValue) {
	SMBC::BlueprintConversionData::Stage = Stage;
	SMBC::BlueprintConversionData::ProgressBarValue = 0;
	SMBC::BlueprintConversionData::ProgressBarMax = MaxValue;
}

std::vector<std::wstring> SMBC::Blueprint::ImageExtensions = { L".jpg", L".png", L".bmp" };
std::vector<std::wstring> SMBC::Blueprint::BlueprintPaths = {};

bool SMBC::Blueprint::IsSupportedExtension(const std::wstring& _ext) {
	for (std::wstring& _Ext : SMBC::Blueprint::ImageExtensions)
		if (_Ext == _ext) return true;

	return false;
}

#include "Lib/OtherFunc/OtherFunc.h"

std::wstring SMBC::Blueprint::FixBlueprintName(const std::wstring& name) {
	std::wstring _Output = L"";

	for (unsigned int a = 0; a < name.size(); a++) {
		const wchar_t& _Letter = name[a];

		if (SMBC::Other::IsLetterAllowed(_Letter))
			_Output += _Letter;
	}

	return _Output;
}

std::wstring SMBC::Blueprint::FindBlueprintImage() {
	std::wstring _Output = L"";
	if (!SMBC::FILE::FileExists(this->BlueprintFolder)) goto skip_function;

	for (auto& Image : fs::directory_iterator(this->BlueprintFolder, fs::directory_options::skip_permission_denied)) {
		if (Image.is_regular_file() && Image.path().has_extension() && this->IsSupportedExtension(Image.path().extension().wstring())) {
			_Output = Image.path().wstring();
			break;
		}
	}

skip_function:
	return _Output;
}
SMBC::Blueprint::Blueprint(const std::wstring& name, const std::wstring& path, const std::wstring& folder) {
	this->BlueprintName = name;
	this->BlueprintPath = path;
	this->BlueprintFolder = folder;
}