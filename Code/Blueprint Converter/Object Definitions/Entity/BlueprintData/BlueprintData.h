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
		std::unordered_map<std::string, SMBC::Entity*> mEntityMap = {};
		std::vector<SMBC::Body*> Objects = {};
		std::size_t ObjectIndex = 0;

		void (*CollectionBindFunction)(SMBC::Entity*, OffsetData&) = nullptr;

		static void Bind_NoSeparation        (SMBC::Entity* pEntity, OffsetData& mOffset);
		static void Bind_SeparateJoints      (SMBC::Entity* pEntity, OffsetData& mOffset);
		static void Bind_SeparateUuid        (SMBC::Entity* pEntity, OffsetData& mOffset);
		static void Bind_SeparateColor       (SMBC::Entity* pEntity, OffsetData& mOffset);
		static void Bind_SeparateUuidAndColor(SMBC::Entity* pEntity, OffsetData& mOffset);

		BlueprintData() = default;

	public:
		BlueprintData(BlueprintData&) = delete;
		BlueprintData(BlueprintData&&) = delete;
		BlueprintData(const BlueprintData&) = delete;
		~BlueprintData();

	private:
		static glm::vec3 JsonToVec(const nlohmann::json& jObj);

		static void LoadChild(const nlohmann::json& bpChild, Body* pBody, const std::size_t& obj_idx);
		void LoadObjects(const nlohmann::json& bpJson);

		static void LoadJoint(const nlohmann::json& bpJoint, Body* pBody);
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