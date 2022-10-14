#pragma once

#include "BlueprintConverter\ObjectDefinitions\Entity.h"

#include <vector>

namespace SMBC
{
	class Body : public Entity
	{
		friend class BlueprintData;

		std::vector<Entity*> mObjects = {};
		std::string mBodyName;

	public:
		inline Body(const std::string& mBodyName)
		{
			this->mBodyName = mBodyName;
		}

		Body(Body&) = delete;
		Body(Body&&) = delete;
		Body(const Body&) = delete;

		inline ~Body()
		{
			for (Entity* pEntity : mObjects)
				delete pEntity;
		}

		inline EntityType Type() const override { return EntityType::Body; }
		inline std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override { return "NOT_IMPLEMENTED_FOR_BODIES"; }
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;
		void FillTextureJson(nlohmann::json& mJson) const override;
		inline std::wstring GetName() const override { return L"Body_" + std::to_wstring(this->mIndex); }
		glm::vec3 GetCenterPoint() const override;

		std::size_t GetAmountOfObjects() const override;
		void WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const override;

		inline void Add(Entity* mEntity)
		{
			mObjects.push_back(mEntity);
		}
	};
}