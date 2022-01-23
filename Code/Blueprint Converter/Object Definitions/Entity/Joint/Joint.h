#pragma once

#include "Blueprint Converter/Object Definitions/Entity/Body/Body.h"
#include "Blueprint Converter/Object Definitions/Entity/Entity.h"

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

		glm::mat4 GetTransformMatrix() const override;
		SMBC::Uuid GetUuid() const override;
	};
}