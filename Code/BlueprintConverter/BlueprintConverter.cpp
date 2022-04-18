#include "BlueprintConverter.h"

#include "BlueprintConverter\ObjectDefinitions\BlueprintData.h"
#include "BlueprintConverter\ConvertSettings.h"
#include "BlueprintConverter\ModelStorage.h"

#include <gtc/matrix_transform.hpp>

namespace SMBC
{
	void BPFunction::ConvertBlueprintToObj(const std::wstring& bp_path, const std::wstring& bp_name, ConvertError& cError)
	{
		BlueprintData* bpData = BlueprintData::LoadFromFile(bp_path, cError);
		if (cError || !bpData) return;

		bpData->WriteToFile(bp_name, cError);

		delete bpData;

		SMBC::ModelStorage::ClearStorage();
	}
}