#pragma once

#include <string>

namespace SMBC
{
	class Blueprint
	{
	public:
		std::wstring Name;
		std::wstring Path;
		std::wstring Folder;
		std::wstring WorkshopId;
		std::wstring ImagePath;
		std::wstring LowerName;

		bool IsSupportedExtension(const std::wstring& _ext);
		static std::wstring FixBlueprintName(const std::wstring& name);
		void FindBlueprintImage();
		bool BlueprintExists();

		static Blueprint* CreateBlueprintFromDirectory(const std::wstring& path);

		~Blueprint() = default;
	};
}