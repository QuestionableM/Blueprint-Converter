#pragma once

#include <vector>
#include <string>

namespace SMBC
{
	class Blueprint
	{
	public:
		static std::vector<std::wstring> ImageExtensions;
		std::wstring Name;
		std::wstring Path;
		std::wstring Folder;
		std::wstring WorkshopId;
		std::wstring LowerName;

		bool IsSupportedExtension(const std::wstring& _ext);
		static std::wstring FixBlueprintName(const std::wstring& name);
		std::wstring FindBlueprintImage();
		bool BlueprintExists();

		Blueprint(
			const std::wstring& name,
			const std::wstring& path,
			const std::wstring& folder,
			const std::wstring& workshop_id
		);

		Blueprint() = default;
	};
}