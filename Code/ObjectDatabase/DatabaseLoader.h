#pragma once

namespace SMBC
{
	class DatabaseLoader
	{
		static void LoadGameDatabase();
		static void LoadModDatabase();

	public:
		static void LoadDatabase();
	};
}