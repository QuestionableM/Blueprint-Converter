#pragma once

#include "BlueprintConverter\ObjectDefinitions\Entity.hpp"
#include "BlueprintConverter\ObjectDefinitions\Body.hpp"

#include "ObjectDatabase\ObjectData.hpp"

namespace SMBC
{
	class Block : public Entity
	{
		const SMBC::BlockData* pParent;
		glm::vec3 mScale;

	public:
		inline Block(const BlockData* pParent, const glm::vec3& position, const glm::vec3& size, const AxisData& mAxisData, const Color& mColor, const std::size_t& obj_idx)
		{
			this->pParent = pParent;
			this->mPosition = position;
			this->mScale = size;
			this->mAxis = mAxisData;
			this->mColor = mColor;
			this->mIndex = obj_idx;
		}

		Block(Block&) = delete;
		Block(Block&&) = delete;
		Block(const Block&) = delete;
		~Block() = default;

		inline EntityType Type() const override { return EntityType::Block; }
		std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;
		void FillTextureJson(nlohmann::json& mJson) const override;
		glm::vec3 GetCenterPoint() const override;

		glm::mat4 GetTransformMatrix() const override;
		void WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const override;
		inline SMBC::Uuid GetUuid() const override { return pParent->m_uuid; }
		inline std::wstring GetName() const override
		{
			return pParent->m_name + L" (" + pParent->m_modPtr->m_Name + L": " + pParent->m_uuid.ToWstring() + L")";
		}
	};
}