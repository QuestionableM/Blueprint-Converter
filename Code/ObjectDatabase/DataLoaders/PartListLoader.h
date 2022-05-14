#pragma once

#include <nlohmann\json.hpp>
#include <glm.hpp>

namespace SMBC
{
	namespace Texture
	{
		class Texture;
		struct TextureList;
	}

	class PartListLoader
	{
		PartListLoader()  = default;
		~PartListLoader() = default;

		static void LoadTextureList(const nlohmann::json& texList, Texture::TextureList& entry);
		static void AddSubMesh(const nlohmann::json& subMesh, Texture::Texture& tex, const std::wstring& idx);
		static bool TryLoadSubMeshList(const nlohmann::json& pLodItem, Texture::Texture& tex);
		static bool TryLoadSubMeshMap(const nlohmann::json& pLodItem, Texture::Texture& tex);
		static bool LoadSubMeshes(const nlohmann::json& pLodItem, Texture::Texture& tex);
		static bool LoadRenderable(const nlohmann::json& pRenderable, Texture::Texture& tex_data, std::wstring& mesh_path);
		static bool GetRenderableData(const nlohmann::json& part, Texture::Texture& tex_data, std::wstring& mesh_path);
		static glm::vec3 LoadPartCollision(const nlohmann::json& collision);

	public:
		static void Load(const nlohmann::json& part_list, class Mod* pMod);
	};
}