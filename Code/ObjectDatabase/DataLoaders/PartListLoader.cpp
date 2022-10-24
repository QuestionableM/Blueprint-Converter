#include "PartListLoader.hpp"

#include "ObjectDatabase\KeywordReplacer.hpp"
#include "ObjectDatabase\ModData.hpp"

#include "Lib\ConvData.hpp"
#include "Lib\String.hpp"
#include "Lib\Json.hpp"

#include "DebugCon.hpp"

namespace SMBC
{
	const PartListLoader::__CollisionLoaderStruct PartListLoader::g_collisionLoadersArray[] =
	{
		{ "box"     , PartListLoader::LoadBoxCollision      },
		{ "hull"    , PartListLoader::LoadBoxCollision      },
		{ "cylinder", PartListLoader::LoadCylinderCollision },
		{ "sphere"  , PartListLoader::LoadSphereCollision   },
		{ "slant"   , PartListLoader::LoadBoxCollision      }
	};

	void PartListLoader::LoadTextureList(const nlohmann::json& texList, Texture::TextureData* entry)
	{
		const int arr_sz = static_cast<int>(texList.size());
		const int list_sz = (arr_sz > 3 ? 3 : arr_sz);

		for (int a = 0; a < list_sz; a++)
		{
			const auto& cur_item = texList.at(a);

			if (cur_item.is_string())
			{
				std::wstring& wstr_path = entry->GetStringRef(a);

				wstr_path = String::ToWide(cur_item.get_ref<const std::string&>());
				wstr_path = PathReplacer::ReplaceKey(wstr_path);
			}
		}
	}

	bool PartListLoader::LoadSubMeshDataEntry(const nlohmann::json& subMesh, Texture::TextureData** entry)
	{
		const auto& v_material_json = Json::Get(subMesh, "material");
		const auto& v_texList_json = Json::Get(subMesh, "textureList");

		if (v_material_json.is_string() && v_texList_json.is_array())
		{
			Texture::TextureData* v_newEntry = new Texture::TextureData();
			v_newEntry->material = String::ToWide(v_material_json.get_ref<const std::string&>());

			PartListLoader::LoadTextureList(v_texList_json, v_newEntry);

			(*entry) = v_newEntry;
			return true;
		}

		return false;
	}

	bool PartListLoader::TryLoadSubMeshList(const nlohmann::json& pLodItem, Texture::SubMeshDataBase** tex)
	{
		const auto& v_subMeshListJson = Json::Get(pLodItem, "subMeshList");
		if (!v_subMeshListJson.is_array())
			return false;

		std::size_t v_idx = 0;
		Texture::SubMeshList* v_subMeshlist = new Texture::SubMeshList();

		for (const auto& v_subMesh : v_subMeshListJson)
		{
			if (!v_subMesh.is_object()) continue;

			const auto& v_new_idx = Json::Get(v_subMesh, "idx");
			const std::size_t v_cur_idx = (v_new_idx.is_number() ? v_new_idx.get<std::size_t>() : v_idx);

			Texture::TextureData* v_newTexData;
			if (PartListLoader::LoadSubMeshDataEntry(v_subMesh, &v_newTexData))
				v_subMeshlist->AddEntry(L"", v_cur_idx, v_newTexData);

			v_idx++;
		}

		(*tex) = v_subMeshlist;
		return true;
	}

	bool PartListLoader::TryLoadSubMeshMap(const nlohmann::json& pLodItem, Texture::SubMeshDataBase** tex)
	{
		const auto& v_subMeshMapJson = Json::Get(pLodItem, "subMeshMap");
		if (!v_subMeshMapJson.is_object()) 
			return false;

		Texture::SubMeshMap* v_newSubMeshMap = new Texture::SubMeshMap();

		for (const auto& v_subMesh : v_subMeshMapJson.items())
		{
			if (!v_subMesh.value().is_object())
				continue;

			std::wstring v_subMeshName = String::ToWide(v_subMesh.key());
			
			Texture::TextureData* v_newTexData;
			if (PartListLoader::LoadSubMeshDataEntry(v_subMesh.value(), &v_newTexData))
				v_newSubMeshMap->AddEntry(String::ToWide(v_subMesh.key()), 0, v_newTexData);
		}

		(*tex) = v_newSubMeshMap;
		return true;
	}

	bool PartListLoader::LoadSubMeshes(const nlohmann::json& pLodItem, Texture::SubMeshDataBase** tex)
	{
		if (PartListLoader::TryLoadSubMeshList(pLodItem, tex))
			return true;

		if (PartListLoader::TryLoadSubMeshMap(pLodItem, tex))
			return true;

		return false;
	}

	bool PartListLoader::LoadRenderable(const nlohmann::json& pRenderable, Texture::SubMeshDataBase** tex_data, std::wstring& mesh_path)
	{
		const auto& rLodList = Json::Get(pRenderable, "lodList");
		if (!rLodList.is_array() || rLodList.size() == 0) return false;

		const auto& rLodItem = Json::Get(rLodList, 0);
		if (!rLodItem.is_object()) return false;

		const auto& rMesh = Json::Get(rLodItem, "mesh");
		if (!rMesh.is_string()) return false;

		mesh_path = String::ToWide(rMesh.get_ref<const std::string&>());
		mesh_path = PathReplacer::ReplaceKey(mesh_path);

		return PartListLoader::LoadSubMeshes(rLodItem, tex_data);
	}

	bool PartListLoader::GetRenderableData(const nlohmann::json& part, Texture::SubMeshDataBase** tex_data, std::wstring& mesh_path)
	{
		const auto& pRenderable = Json::Get(part, "renderable");

		nlohmann::json rend_data;
		switch (pRenderable.type())
		{
		case nlohmann::detail::value_t::string:
			{
				const std::wstring p_rend_wide = String::ToWide(pRenderable.get_ref<const std::string&>());
				const std::wstring p_rend_repl = PathReplacer::ReplaceKey(p_rend_wide);

				rend_data = Json::LoadParseJson(p_rend_repl);
				if (!rend_data.is_object())
					return false;

				return PartListLoader::LoadRenderable(rend_data, tex_data, mesh_path);
			}
		case nlohmann::detail::value_t::object:
			return PartListLoader::LoadRenderable(pRenderable, tex_data, mesh_path);
		default:
			return false;
		}
	}

	void PartListLoader::LoadBoxCollision(const nlohmann::json& collision, glm::vec3& vec_out)
	{
		const auto& v_box_x = Json::Get(collision, "x");
		const auto& v_box_y = Json::Get(collision, "y");
		const auto& v_box_z = Json::Get(collision, "z");

		if (v_box_x.is_number() && v_box_y.is_number() && v_box_z.is_number())
			vec_out = glm::vec3(v_box_x.get<float>(), v_box_y.get<float>(), v_box_z.get<float>());
	}

	void PartListLoader::LoadCylinderCollision(const nlohmann::json& collision, glm::vec3& vec_out)
	{
		const auto& v_diameter = Json::Get(collision, "diameter");
		const auto& v_depth = Json::Get(collision, "depth");

		if (!v_diameter.is_number() || !v_depth.is_number())
			return;

		const auto& v_axis = Json::Get(collision, "axis");
		const std::string v_axis_str = (v_axis.is_string() ? v_axis.get_ref<const std::string&>() : "z");
		if (v_axis_str.empty())
			return;

		const float v_diameter_f = v_diameter.get<float>();
		const float v_depth_f = v_depth.get<float>();

		const char v_axis_char = std::tolower(v_axis_str[0]);
		switch (v_axis_char)
		{
		case 'x': vec_out = glm::vec3(v_depth_f   , v_diameter_f, v_diameter_f); break;
		case 'y': vec_out = glm::vec3(v_diameter_f, v_depth_f   , v_diameter_f); break;
		case 'z': vec_out = glm::vec3(v_diameter_f, v_diameter_f, v_depth_f   ); break;
		}
	}

	void PartListLoader::LoadSphereCollision(const nlohmann::json& collision, glm::vec3& vec_out)
	{
		const auto& v_diameter = Json::Get(collision, "diameter");
		if (v_diameter.is_number())
			vec_out = glm::vec3(v_diameter.get<float>());
	}

	glm::vec3 PartListLoader::LoadPartCollision(const nlohmann::json& collision)
	{
		glm::vec3 v_output_vec(1.0f);

		constexpr unsigned char v_collisionLoadersArraySz = sizeof(g_collisionLoadersArray) / sizeof(__CollisionLoaderStruct);
		for (unsigned char a = 0; a < v_collisionLoadersArraySz; a++)
		{
			const __CollisionLoaderStruct& v_curLoader = g_collisionLoadersArray[a];

			const auto& v_collisionJson = Json::Get(collision, v_curLoader.key);
			if (!v_collisionJson.is_object())
				continue;

			v_curLoader.func_ptr(v_collisionJson, v_output_vec);
			break;
		}

		return v_output_vec;
	}

	void PartListLoader::Load(const nlohmann::json& part_list, Mod* pMod)
	{
		ConvData::ProgressMax += part_list.size();
		for (const auto& l_part : part_list)
		{
			if (!l_part.is_object()) continue;

			const auto& p_uuid = Json::Get(l_part, "uuid");
			if (!p_uuid.is_string()) continue;

			const SMBC::Uuid uuid_obj(p_uuid.get_ref<const std::string&>());

			if (Mod::AllObjects.find(uuid_obj) != Mod::AllObjects.end())
			{
				DebugWarningL("An object with this uuid already exists! (", uuid_obj.ToString(), ")");
				continue;
			}

			std::wstring mesh_path;
			Texture::SubMeshDataBase* tex_data;
			if (!PartListLoader::GetRenderableData(l_part, &tex_data, mesh_path))
				continue;

			const std::wstring l_NameWstr = pMod->m_LanguageDb.GetTranslation(uuid_obj);
			const glm::vec3 p_bounds = PartListLoader::LoadPartCollision(l_part);

			PartData* p_new_part = new PartData(uuid_obj, mesh_path, l_NameWstr, tex_data, p_bounds, pMod);

			const auto new_pair = std::make_pair(p_new_part->m_uuid, p_new_part);
			Mod::AllObjects.insert(new_pair);
			pMod->m_Objects.insert(new_pair);

			ConvData::ProgressValue++;
		}
	}
}