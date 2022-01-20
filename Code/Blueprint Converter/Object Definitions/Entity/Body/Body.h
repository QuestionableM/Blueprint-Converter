#pragma once

#include "Blueprint Converter/Object Definitions/Entity/Entity.h"
#include <vector>

namespace SMBC
{
	class Body : public Entity
	{
		std::vector<Entity*> mObjects = {};
		std::size_t mIndex;

	public:
		Body(const std::size_t& mIndex);
		Body(Body&) = delete;
		Body(Body&&) = delete;
		Body(const Body&) = delete;
		~Body();

		EntityType Type() const override;
		std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;
		void FillTextureJson(nlohmann::json& mJson) const override;
		std::wstring GetName() const override;

		std::size_t GetAmountOfObjects() const override;
		void WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const override;

		void Add(Entity* mEntity);
	};
}