#pragma once

#include "Blueprint Converter/Object Definitions/Entity/Entity.h"
#include <vector>

namespace SMBC
{
	class Body : public Entity
	{
		friend class BlueprintData;

		std::vector<Entity*> mObjects = {};
		std::string mBodyName;

	public:
		Body(const std::string& mBodyName);
		Body(Body&) = delete;
		Body(Body&&) = delete;
		Body(const Body&) = delete;
		~Body();

		EntityType Type() const override;
		std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;
		void FillTextureJson(nlohmann::json& mJson) const override;
		std::wstring GetName() const override;
		glm::vec3 GetCenterPoint() const override;

		std::size_t GetAmountOfObjects() const override;
		void WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const override;

		void Add(Entity* mEntity);
	};
}