#include "ConvertSettings.h"

namespace SMBC
{
	bool ConvertSettings::TextureList	= false;
	bool ConvertSettings::ApplyTextures = true;
	bool ConvertSettings::ExportUvs		= true;
	bool ConvertSettings::ExportNormals = true;
	bool ConvertSettings::MatByColor	= true;

	int  ConvertSettings::SeparationMethod = 0;
}