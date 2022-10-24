#pragma once

#include "BlueprintConverter\ObjectDefinitions\Entity.hpp"
#include "BlueprintConverter\ObjectDefinitions\Body.hpp"

namespace SMBC
{
	class Joint : public Entity
	{
		const PartData* pParent;

	public:
		inline Joint(const PartData* pParent, Model* pModel, const glm::vec3& pos, const AxisData& mAxis, const Color& mColor, const std::size_t& mChildIdx)
		{
			this->pParent = pParent;
			this->pModel = pModel;
			this->mPosition = pos;
			this->mAxis = mAxis;
			this->mColor = mColor;
			this->mIndex = mChildIdx;
		}

		Joint(const Joint&) = delete;
		Joint(Joint&&) = delete;
		Joint(Joint&) = delete;
		~Joint() = default;

		inline EntityType Type() const override { return EntityType::Joint; }
		std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;
		void FillTextureJson(nlohmann::json& mJson) const override;
		glm::vec3 GetCenterPoint() const override;

		glm::mat4 GetTransformMatrix() const override;
		inline SMBC::Uuid GetUuid() const override { return pParent->m_uuid; }
		inline std::wstring GetName() const override
		{
			return pParent->m_name + L" (" + pParent->m_modPtr->m_Name + L": " + pParent->m_uuid.ToWstring() + L")";
		}
	};
}