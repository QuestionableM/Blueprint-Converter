#pragma once

#include "Blueprint Converter/Object Definitions/Entity/Entity.h"
#include "Blueprint Converter/Object Definitions/Entity/Body/Body.h"
#include "Object Database/Object Data/ObjectData.h"

namespace SMBC
{
	class Block : public Entity
	{
		const SMBC::BlockData* pParent;
		glm::vec3 mScale;

	public:
		Block(
			const BlockData* pParent,
			const glm::vec3& position,
			const glm::vec3& size,
			const AxisData& mAxisData,
			const Color& mColor,
			const std::size_t& obj_idx
		);
		Block(Block&) = delete;
		Block(Block&&) = delete;
		Block(const Block&) = delete;
		~Block() = default;

		EntityType Type() const override;
		std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;
		void FillTextureJson(nlohmann::json& mJson) const override;
		std::wstring GetName() const override;

		glm::mat4 GetTransformMatrix() const override;
		void WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const override;
		SMBC::Uuid GetUuid() const override;
	};
}