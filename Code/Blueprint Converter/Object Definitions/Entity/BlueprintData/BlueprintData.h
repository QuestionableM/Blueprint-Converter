#pragma once

#include "Blueprint Converter/Object Definitions/Entity/Entity.h"

#include <vector>
#include <string>

namespace SMBC
{
	class ConvertError;

	class BlueprintData
	{
		std::vector<SMBC::Entity*> Objects = {};
		std::size_t ObjectIndex = 0;

		BlueprintData() = default;

	public:
		BlueprintData(BlueprintData&) = delete;
		BlueprintData(BlueprintData&&) = delete;
		BlueprintData(const BlueprintData&) = delete;
		~BlueprintData();

	private:
		void LoadObjects(const nlohmann::json& bpJson);
		void LoadJoints(const nlohmann::json& bpJson);
		std::size_t GetAmountOfObjects() const;

		static nlohmann::json LoadAndCheckBlueprint(const std::wstring& path, ConvertError& cError);

	public:
		static BlueprintData* LoadFromFile(const std::wstring& path, ConvertError& cError);
		void WriteToFile(const std::wstring& name, ConvertError& cError) const;
	};
}