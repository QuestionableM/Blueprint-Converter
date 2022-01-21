#include "BlueprintConverter.h"

#include "Blueprint Converter/Model Storage/ModelStorage.h"
#include "Blueprint Converter/Object Definitions/Entity/BlueprintData/BlueprintData.h"
#include "Blueprint Converter/Convert Settings/ConvertSettings.h"

#include <gtc/matrix_transform.hpp>

namespace SMBC
{
	namespace BPFunction
	{
		void ConvertBlueprintToObj(const std::wstring& bp_path, const std::wstring& bp_name, ConvertError& cError)
		{
			BlueprintData* bpData = BlueprintData::LoadFromFile(bp_path, cError);
			if (cError || !bpData) return;

			bpData->WriteToFile(bp_name, cError);

			delete bpData;

			SMBC::ModelStorage::ClearStorage();
		}
	}
}