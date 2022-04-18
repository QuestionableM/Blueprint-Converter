#include "MaterialManager.h"

#include "Lib\ProgramSettings.h"
#include "Lib\String.h"
#include "Lib\Json.h"

#include "DebugCon.h"

namespace SMBC
{
	void MaterialManager::Initialize()
	{
		nlohmann::json pMatMap = Json::LoadParseJson(Settings::MaterialMapPath.data());
		if (!pMatMap.is_object()) return;

		for (const auto& pObject : pMatMap.items())
		{
			if (!pObject.value().is_string()) continue;

			const std::wstring pKey = String::ToWide(pObject.key());
			const std::wstring pValue = String::ToWide(pObject.value().get<std::string>());

			if (MatStorage.find(pKey) != MatStorage.end())
				continue;

			MatStorage.insert(std::make_pair(pKey, pValue));
		}
	}

	std::wstring MaterialManager::GetMaterialW(const std::wstring& mat_name)
	{
		if (MatStorage.find(mat_name) != MatStorage.end())
			return L"m" + MatStorage.at(mat_name);

		DebugErrorL("Couldn't find the specifieid material: ", mat_name);

		return L"m1";
	}

	std::string MaterialManager::GetMaterialA(const std::wstring& mat_name)
	{
		const std::wstring wMaterial = MaterialManager::GetMaterialW(mat_name);

		return String::ToUtf8(wMaterial);
	}
}