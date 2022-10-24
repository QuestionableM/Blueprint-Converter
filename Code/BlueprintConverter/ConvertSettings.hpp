#pragma once

namespace SMBC
{
	class ConvertSettings
	{
	public:
		inline static bool TextureList   = false;
		inline static bool ApplyTextures = true;
		inline static bool ExportUvs     = true;
		inline static bool ExportNormals = true;
		inline static bool MatByColor    = true;

		inline static int SeparationMethod = 0;
	};
}