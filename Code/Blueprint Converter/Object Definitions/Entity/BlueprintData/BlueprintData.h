#pragma once

#include "Blueprint Converter/Object Definitions/Entity/Entity.h"

#include <vector>
#include <string>

namespace SMBC
{
	class ConvertError;
	class Body;

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
		static void LoadChild(const nlohmann::json& bpChild, Body* pBody, const std::size_t& obj_idx);
		void LoadObjects(const nlohmann::json& bpJson);
		void LoadJoints(const nlohmann::json& bpJson);
		std::size_t GetAmountOfObjects() const;

		static nlohmann::json LoadAndCheckBlueprint(const std::wstring& path, ConvertError& cError);

	public:
		static BlueprintData* LoadFromFile(const std::wstring& path, ConvertError& cError);

		void WriteMtlFile(const std::wstring& path, const std::unordered_map<std::string, ObjectTextureData>& tex_data) const;
		void WriteToFile(const std::wstring& name, ConvertError& cError) const;
	};
}