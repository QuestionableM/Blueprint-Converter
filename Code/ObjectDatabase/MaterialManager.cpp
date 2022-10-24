#include "MaterialManager.hpp"

#include "Lib\ProgramSettings.hpp"
#include "Lib\String.hpp"
#include "Lib\Json.hpp"

#include "DebugCon.hpp"

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
			const std::wstring pValue = String::ToWide(pObject.value().get_ref<const std::string&>());

			if (MatStorage.find(pKey) != MatStorage.end())
				continue;

			MatStorage.insert(std::make_pair(pKey, pValue));
		}
	}

	std::wstring MaterialManager::GetMaterialW(const std::wstring& mat_name)
	{
		const MaterialStorageMap::iterator it = MatStorage.find(mat_name);
		if (it != MatStorage.end())
			return L"m" + it->second;

		DebugErrorL("Couldn't find the specifieid material: ", mat_name);

		return L"m1";
	}

	std::string MaterialManager::GetMaterialA(const std::wstring& mat_name)
	{
		return String::ToUtf8(MaterialManager::GetMaterialW(mat_name));
	}
}