#pragma once

#include "Blueprint Converter/Object Definitions/Entity/Body/Body.h"
#include "Blueprint Converter/Object Definitions/Entity/Entity.h"

namespace SMBC
{
	class Joint : public Entity
	{
		const PartData* pParent;
		const Body* pBody;

	public:
		Joint(const PartData* pParent, const Body* pBody, Model* pModel, const glm::vec3& pos, const AxisData& mAxis, const Color& mColor);
		Joint(const Joint&) = delete;
		Joint(Joint&&) = delete;
		Joint(Joint&) = delete;
		~Joint() = default;

		EntityType Type() const override;
		std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;

		glm::mat4 GetTransformMatrix() const override;
	};
}