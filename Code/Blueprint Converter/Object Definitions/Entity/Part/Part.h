#pragma once

#include "Blueprint Converter/Object Definitions/Entity/Entity.h"
#include "Blueprint Converter/Object Definitions/Entity/Body/Body.h"
#include "Object Database/Object Data/ObjectData.h"
#include "Lib/Color/Color.h"

namespace SMBC
{
	class Part : public Entity
	{
		const PartData* pParent;
		const Body* pBody;

	public:
		Part(const PartData* pParent, Model* pModel, Body* pBody, const glm::vec3& position, const AxisData& mAxisData, const Color& mColor, const std::size_t& obj_idx);
		Part(Part&) = delete;
		Part(Part&&) = delete;
		Part(const Part&) = delete;
		~Part() = default;

		EntityType Type() const override;
		std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const override;
		void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const override;
		void FillTextureJson(nlohmann::json& mJson) const override;
		std::wstring GetName() const override;

		glm::mat4 GetTransformMatrix() const override;
	};
}