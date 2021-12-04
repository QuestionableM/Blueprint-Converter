#include "Blueprint.h"

#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/File/FileFunc.h"
#include "Lib/Json/JsonFunc.h"
#include "Lib/String/String.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace SMBC
{
	const static std::unordered_map<std::wstring, bool> SupportedImageExtensions = { { L".jpg", 1 }, { L".png", 1 }, { L".bmp", 1 } };
	bool Blueprint::IsSupportedExtension(const std::wstring& _ext)
	{
		if (SupportedImageExtensions.find(_ext) != SupportedImageExtensions.end())
			return true;

		return false;
	}

	std::wstring Blueprint::FixBlueprintName(const std::wstring& name)
	{
		std::wstring _Output = L"";

		for (const wchar_t& _Letter : name)
		{
			if (Other::IsLetterAllowed(_Letter))
				_Output += _Letter;
		}

		return _Output;
	}

	void Blueprint::FindBlueprintImage()
	{
		if (!File::Exists(this->Folder) || !this->ImagePath.empty()) return;

		fs::directory_iterator dir_iter(this->Folder, fs::directory_options::skip_permission_denied);
		for (const auto& dir : dir_iter)
		{
			if (!dir.is_regular_file()) continue;

			const fs::path& imgPath = dir.path();

			if (imgPath.has_extension() && this->IsSupportedExtension(imgPath.extension().wstring()))
			{
				this->ImagePath = imgPath.wstring();
				return;
			}
		}
	}

	bool Blueprint::BlueprintExists()
	{
		bool _FolderExists = File::Exists(this->Folder);
		bool _PathExists = File::Exists(this->Path);

		return (_FolderExists && _PathExists);
	}

	Blueprint* Blueprint::CreateBlueprintFromDirectory(const std::wstring& path)
	{
		const std::wstring bpJson = (path + L"/description.json");
		if (!File::Exists(bpJson)) return nullptr;

		const nlohmann::json bpDesc = Json::LoadParseJson(bpJson, true);
		if (!bpDesc.is_object()) return nullptr;

		const auto& bpName = Json::Get(bpDesc, "name");
		const auto& bpType = Json::Get(bpDesc, "type");

		if (!bpName.is_string() || !bpType.is_string()) return nullptr;
		if (bpType.get<std::string>() != "Blueprint") return nullptr;

		const auto& file_id_obj = Json::Get(bpDesc, "fileId");
		long long file_id = file_id_obj.is_number() ? file_id_obj.get<long long>() : 0ll;

		Blueprint* new_bp = new Blueprint();
		new_bp->Name = String::ToWide(bpName.get<std::string>());
		new_bp->Path = bpJson;
		new_bp->Folder = path;
		new_bp->WorkshopId = (file_id > 0) ? std::to_wstring(file_id) : L"";
		new_bp->LowerName = String::ToLower(new_bp->Name);

		new_bp->FindBlueprintImage();

		return new_bp;
	}
}