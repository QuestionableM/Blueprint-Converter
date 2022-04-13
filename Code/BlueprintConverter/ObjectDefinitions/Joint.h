#pragma once

#include "BlueprintConverter\ObjectDefinitions\Body.h"
#include "BlueprintConverter\ObjectDefinitions\Entity.h"

namespace SMBC
{
	class Joint : public Entity
	{
		const PartData* pParent;

	public:
		Joint(const PartData* pParent, Model* pModel, const glm::vec3& pos, const AxisData& mAxis, const Color& mColor, const std::size_t& mChildIdx);
		Joint(const Joint&) = delete;
		Joint(Joint&&) = delete;
		Joint(Joint&) = delete;
		~Joint() = default;

		EntityType Type() const override;
		std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;
		void FillTextureJson(nlohmann::json& mJson) const override;
		std::wstring GetName() const override;
		glm::vec3 GetCenterPoint() const override;

		glm::mat4 GetTransformMatrix() const override;
		SMBC::Uuid GetUuid() const override;
	};
}