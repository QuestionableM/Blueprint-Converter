#pragma once

#include "Blueprint Converter/Object Definitions/Entity/Body/Body.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace SMBC
{
	class ConvertError;
	class Body;

	class BlueprintData
	{
		std::size_t ObjectIndex = 0;
		std::size_t BodyIndex   = 0;

		std::unordered_map<std::size_t, Body*> mIndexMap = {};
		std::unordered_map<std::string, Body*> mCollectionsMap = {};
		std::vector<Body*> mCollections = {};

		void (*CollectionBindFunction)(BlueprintData*, Entity*) = nullptr;

		static void Bind_NoSeparation        (BlueprintData* bpData, Entity* pEntity);
		static void Bind_SeparateAll         (BlueprintData* bpData, Entity* pEntity);
		static void Bind_SeparateJoints      (BlueprintData* bpData, Entity* pEntity);
		static void Bind_SeparateUuid        (BlueprintData* bpData, Entity* pEntity);
		static void Bind_SeparateColor       (BlueprintData* bpData, Entity* pEntity);
		static void Bind_SeparateUuidAndColor(BlueprintData* bpData, Entity* pEntity);

		void CreateAndAddObjToCollection(const std::string& cName, Entity* pEntity);
		void SelectSeparationMethod();

		BlueprintData() = default;

	public:
		BlueprintData(BlueprintData&) = delete;
		BlueprintData(BlueprintData&&) = delete;
		BlueprintData(const BlueprintData&) = delete;
		~BlueprintData();

	private:
		static glm::vec3 JsonToVec(const nlohmann::json& jObj);

		void LoadChild(const nlohmann::json& bpChild);
		void LoadObjects(const nlohmann::json& bpJson);

		void LoadJoint(const nlohmann::json& bpJoint);
		void LoadJoints(const nlohmann::json& bpJson);
		std::size_t GetAmountOfObjects() const;

		static nlohmann::json LoadAndCheckBlueprint(const std::wstring& path, ConvertError& cError);

	public:
		static BlueprintData* LoadFromFile(const std::wstring& path, ConvertError& cError);

		void WriteMtlFile(const std::wstring& path) const;
		void WriteTexPaths(const std::wstring& path) const;

		std::wstring PrepareForWrite(const std::wstring& name, ConvertError& cError) const;
		void WriteObjects(std::ofstream& mOutput) const;
		void WriteToFile(const std::wstring& name, ConvertError& cError) const;
	};
}