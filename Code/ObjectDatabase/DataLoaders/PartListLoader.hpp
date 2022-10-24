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

		//Collision loaders

		static void LoadBoxCollision(const nlohmann::json& collision, glm::vec3& vec_out);
		static void LoadCylinderCollision(const nlohmann::json& collision, glm::vec3& vec_out);
		static void LoadSphereCollision(const nlohmann::json& collision, glm::vec3& vec_out);
		static glm::vec3 LoadPartCollision(const nlohmann::json& collision);

	public:
		static void Load(const nlohmann::json& part_list, class Mod* pMod);

	private:
		struct __CollisionLoaderStruct
		{
			std::string key;
			void (*func_ptr)(const nlohmann::json&, glm::vec3&);
		};

		static const __CollisionLoaderStruct g_collisionLoadersArray[];
	};
}