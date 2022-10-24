#pragma once

#include "ObjectDatabase\ModData.hpp"
#include "Lib\Color.hpp"

namespace SMBC
{
	struct OffsetData;
	struct Model;

	struct ObjectTextureData
	{
		SMBC::Texture::TextureData* mTextures;
		SMBC::Color mColor;
	};

	struct AxisData
	{
		short x;
		short z;
	};

	enum class EntityType
	{
		Body,
		Block,
		Part,
		Joint
	};

	class Entity
	{
	protected:
		glm::vec3 mPosition = glm::vec3(0.0f);
		AxisData mAxis;

		SMBC::Color mColor;
		SMBC::Model* pModel;
		std::size_t mIndex = 0;

	public:
		Entity() = default;
		Entity(Entity&) = delete;
		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
		virtual ~Entity() = default;

		virtual EntityType Type() const = 0;
		virtual std::string GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const = 0;
		virtual void FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const = 0;
		virtual void FillTextureJson(nlohmann::json& mJson) const = 0;
		virtual std::wstring GetName() const = 0;
		virtual glm::vec3 GetCenterPoint() const = 0;

		inline virtual glm::mat4 GetTransformMatrix() const { return glm::mat4(1.0f); }
		inline virtual std::size_t GetAmountOfObjects() const { return 1; }

		virtual void WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const;

		inline std::size_t GetIndex() const { return this->mIndex; }
		inline SMBC::Color GetColor() const { return this->mColor; }
		inline virtual SMBC::Uuid GetUuid() const { return SMBC::Uuid(); }
	};
}