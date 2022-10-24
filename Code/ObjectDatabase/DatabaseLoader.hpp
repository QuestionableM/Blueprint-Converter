#pragma once

#include <vector>
#include <string>

namespace SMBC
{
	class DatabaseLoader
	{
		static void LoadGameDatabase();

		static void LoadModsFromStrVector(const std::vector<std::wstring>& r_wstr_vec, const bool& is_local);
		static void LoadModDatabase();

	public:
		static void LoadDatabase();
	};
}