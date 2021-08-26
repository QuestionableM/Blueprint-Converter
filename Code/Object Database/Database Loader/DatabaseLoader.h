#pragma once

#include "Object Database/Texture Database/TextureDatabase.h"
#include "Object Database/Mod Data/ModData.h"
#include "Lib/Json/JsonFunc.h"
#include <glm.hpp>

namespace SMBC {
	class DatabaseLoader {
		static glm::vec3 LoadBlockCollision(const nlohmann::json& collision);

		static void GetTextureArray(
			const nlohmann::json& textureList,
			std::wstring& dif,
			std::wstring& asg,
			std::wstring& nor
		);

		static void LoadSubMeshTextures(
			const nlohmann::json& subMesh,
			const std::wstring& texKey,
			SMBC::Texture::Texture& texture
		);

		static bool TryLoadSubMeshList(const nlohmann::json& renderable, SMBC::Texture::Texture& lod);
		static bool TryLoadSubMeshMap(const nlohmann::json& renderable, SMBC::Texture::Texture& lod);

		static void GetRenderableData(
			const nlohmann::json& renderable,
			SMBC::Texture::Texture& texture,
			std::wstring& model_path
		);

		static void LoadRenderableObject(
			SMBC::ModData*& mod,
			const nlohmann::json& r_Object,
			const std::wstring& uuid,
			const std::wstring& name,
			const glm::vec3& bounds
		);

		static void LoadBlockList(const nlohmann::json& f_Json, SMBC::ModData*& mod);
		static void LoadPartList(const nlohmann::json& f_Json, SMBC::ModData*& mod);

	public:
		static void LoadObjectFile(SMBC::ModData* mod, const std::wstring& path);
		static void LoadGameDatabase();
		static void LoadModDatabase();
	};
}