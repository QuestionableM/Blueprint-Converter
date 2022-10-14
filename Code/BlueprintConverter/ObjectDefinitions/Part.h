#pragma once

#include "BlueprintConverter/ObjectDefinitions/Entity.h"
#include "BlueprintConverter\ObjectDefinitions\Body.h"

#include "ObjectDatabase\ObjectData.h"

#include "Lib\Color.h"

namespace SMBC
{
	class Part : public Entity
	{
		const PartData* pParent;

	public:
		inline Part(const PartData* pParent, Model* pModel, const glm::vec3& position, const AxisData& mAxisData, const Color& mColor, const std::size_t& obj_idx)
		{
			this->pParent = pParent;
			this->pModel = pModel;
			this->mPosition = position;
			this->mAxis = mAxisData;
			this->mColor = mColor;
			this->mIndex = obj_idx;
		}

		Part(Part&) = delete;
		Part(Part&&) = delete;
		Part(const Part&) = delete;
		~Part() = default;

		inline EntityType Type() const override { return EntityType::Part; }
		std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;
		void FillTextureJson(nlohmann::json& mJson) const override;
		glm::vec3 GetCenterPoint() const override;

		glm::mat4 GetTransformMatrix() const override;
		inline SMBC::Uuid GetUuid() const override { return pParent->Uuid; }
		inline std::wstring GetName() const override
		{
			return pParent->Name + L" (" + pParent->ModPtr->m_Name + L": " + pParent->Uuid.ToWstring() + L")";
		}
	};
}