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
		Part(const PartData* pParent, Model* pModel, const glm::vec3& position, const AxisData& mAxisData, const Color& mColor, const std::size_t& obj_idx);
		Part(Part&) = delete;
		Part(Part&&) = delete;
		Part(const Part&) = delete;
		~Part() = default;

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