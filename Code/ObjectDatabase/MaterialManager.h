#pragma once

#include <unordered_map>
#include <string>

namespace SMBC
{
	class MaterialManager
	{
		inline static std::unordered_map<std::wstring, std::wstring> MatStorage = {};

	public:
		static void Initialize();

		static std::wstring GetMaterialW(const std::wstring& mat_name);
		static std::string GetMaterialA(const std::wstring& mat_name);
	};
}