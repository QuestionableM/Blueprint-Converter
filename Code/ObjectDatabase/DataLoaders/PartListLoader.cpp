#include "PartListLoader.h"

#include "ObjectDatabase\ModData.h"
#include "ObjectDatabase\KeywordReplacer.h"

#include "Lib\Json.h"
#include "Lib\String.h"
#include "Lib\ConvData.h"

#include "DebugCon.h"

namespace SMBC
{
	void PartListLoader::LoadTextureList(const nlohmann::json& texList, Texture::TextureList& entry)
	{
		const int arr_sz = (int)texList.size();
		const int list_sz = (arr_sz > 3 ? 3 : arr_sz);

		for (int a = 0; a < list_sz; a++)
		{
			const auto& cur_item = texList.at(a);

			if (cur_item.is_string())
			{
				std::wstring& wstr_path = entry.GetStringRef(a);

				wstr_path = String::ToWide(cur_item.get<std::string>());
				wstr_path = PathReplacer::ReplaceKey(wstr_path);
			}
		}
	}

	void PartListLoader::AddSubMesh(const nlohmann::json& subMesh, Texture::Texture& tex, const std::wstring& idx)
	{
		const auto& sTexList = Json::Get(subMesh, "textureList");
		if (!sTexList.is_array()) return;

		Texture::TextureList new_entry;
		PartListLoader::LoadTextureList(sTexList, new_entry);

		const auto& sMaterial = Json::Get(subMesh, "material");
		new_entry.material = (sMaterial.is_string() ? String::ToWide(sMaterial.get<std::string>()) : L"DifAsgNor");

		tex.AddEntry(idx, new_entry);
	}

	bool PartListLoader::TryLoadSubMeshList(const nlohmann::json& pLodItem, Texture::Texture& tex)
	{
		const auto& pMeshList = Json::Get(pLodItem, "subMeshList");
		if (!pMeshList.is_array()) return false;

		std::size_t _idx = 0;
		Texture::Texture out_tex(Texture::TextureType::SubMeshList);
		for (const auto& subMesh : pMeshList)
		{
			if (!subMesh.is_object()) continue;

			const auto& sIdx = Json::Get(subMesh, "idx");
			std::size_t cur_idx = (sIdx.is_number() ? sIdx.get<std::size_t>() : _idx);

			PartListLoader::AddSubMesh(subMesh, out_tex, std::to_wstring(cur_idx));
			_idx++;
		}

		tex = out_tex;
		return true;
	}

	bool PartListLoader::TryLoadSubMeshMap(const nlohmann::json& pLodItem, Texture::Texture& tex)
	{
		const auto& pMeshMap = Json::Get(pLodItem, "subMeshMap");
		if (!pMeshMap.is_object()) return false;

		Texture::Texture out_tex(Texture::TextureType::SubMeshMap);
		for (const auto& subMesh : pMeshMap.items())
		{
			if (!subMesh.value().is_object()) continue;

			std::wstring subMeshName = String::ToWide(subMesh.key());
			PartListLoader::AddSubMesh(subMesh.value(), out_tex, subMeshName);
		}

		tex = out_tex;
		return true;
	}

	bool PartListLoader::LoadSubMeshes(const nlohmann::json& pLodItem, Texture::Texture& tex)
	{
		if (PartListLoader::TryLoadSubMeshList(pLodItem, tex) || PartListLoader::TryLoadSubMeshMap(pLodItem, tex))
			return true;

		return false;
	}

	bool PartListLoader::LoadRenderable(const nlohmann::json& pRenderable, Texture::Texture& tex_data, std::wstring& mesh_path)
	{
		const auto& rLodList = Json::Get(pRenderable, "lodList");
		if (!rLodList.is_array() || rLodList.size() == 0) return false;

		const auto& rLodItem = Json::Get(rLodList, 0);
		if (!rLodItem.is_object()) return false;

		const auto& rMesh = Json::Get(rLodItem, "mesh");
		if (!rMesh.is_string()) return false;

		mesh_path = String::ToWide(rMesh.get<std::string>());
		mesh_path = PathReplacer::ReplaceKey(mesh_path);

		return PartListLoader::LoadSubMeshes(rLodItem, tex_data);
	}

	bool PartListLoader::GetRenderableData(const nlohmann::json& part, Texture::Texture& tex_data, std::wstring& mesh_path)
	{
		const auto& pRenderable = Json::Get(part, "renderable");

		nlohmann::json rend_data;
		switch (pRenderable.type())
		{
		case nlohmann::detail::value_t::string:
			{
				const std::wstring p_rend_wide = String::ToWide(pRenderable.get<std::string>());
				const std::wstring p_rend_repl = PathReplacer::ReplaceKey(p_rend_wide);

				rend_data = Json::LoadParseJson(p_rend_repl);
				if (!rend_data.is_object())
					return false;

				break;
			}
		case nlohmann::detail::value_t::object:
			rend_data = pRenderable;
			break;
		default:
			return false;
		}

		return PartListLoader::LoadRenderable(rend_data, tex_data, mesh_path);
	}

	glm::vec3 PartListLoader::LoadPartCollision(const nlohmann::json& collision)
	{
		const bool isBoxCol = collision.contains("box");
		const bool isHullCol = collision.contains("hull");
		if (isBoxCol || isHullCol)
		{
			const auto& b_Col = collision.at(isBoxCol ? "box" : "hull");

			if (b_Col.is_object())
			{
				const auto& xPos = Json::Get(b_Col, "x");
				const auto& yPos = Json::Get(b_Col, "y");
				const auto& zPos = Json::Get(b_Col, "z");

				if (xPos.is_number() && yPos.is_number() && zPos.is_number())
					return { xPos.get<float>(), yPos.get<float>(), zPos.get<float>() };
			}
		}
		else
		{
			const auto& cyl_col = Json::Get(collision, "cylinder");
			if (cyl_col.is_object())
			{
				const auto& c_Diameter = Json::Get(cyl_col, "diameter");
				const auto& c_Depth = Json::Get(cyl_col, "depth");

				if (c_Diameter.is_number() && c_Depth.is_number())
				{
					const float f_Diameter = c_Diameter.get<float>();
					const float f_Depth = c_Depth.get<float>();

					const auto& c_Axis = Json::Get(cyl_col, "axis");
					const std::string c_AxisStr = (c_Axis.is_string() ? c_Axis.get<std::string>() : "z");

					switch (c_AxisStr[0])
					{
					case 'x': case 'X':
						return { f_Depth, f_Diameter, f_Diameter };
					case 'y': case 'Y':
						return { f_Diameter, f_Depth, f_Diameter };
					case 'z': case 'Z':
						return { f_Diameter, f_Diameter, f_Depth };
					}
				}
			}
			else
			{
				const auto& sphere_col = Json::Get(collision, "sphere");
				if (sphere_col.is_object())
				{
					const auto& s_Diameter = Json::Get(sphere_col, "diameter");
					if (s_Diameter.is_number())
						return glm::vec3(s_Diameter.get<float>());
				}
			}
		}

		return glm::vec3(1.0f);
	}

	void PartListLoader::Load(const nlohmann::json& part_list, Mod* pMod)
	{
		ConvData::ProgressMax += part_list.size();
		for (const auto& l_part : part_list)
		{
			if (!l_part.is_object()) continue;

			const auto& p_uuid = Json::Get(l_part, "uuid");
			if (!p_uuid.is_string()) continue;

			SMBC::Uuid uuid_obj(p_uuid.get<std::string>());

			if (Mod::AllObjects.find(uuid_obj) != Mod::AllObjects.end())
			{
				DebugWarningL("An object with this uuid already exists! (", uuid_obj.ToString(), ")");
				continue;
			}

			std::wstring mesh_path;
			Texture::Texture tex_data;
			if (!PartListLoader::GetRenderableData(l_part, tex_data, mesh_path))
				continue;

			const std::wstring l_NameWstr = pMod->m_LanguageDb.GetTranslation(uuid_obj);
			const glm::vec3 p_bounds = PartListLoader::LoadPartCollision(l_part);

			PartData* p_new_part = new PartData(uuid_obj, mesh_path, l_NameWstr, tex_data, p_bounds, pMod);

			const auto new_pair = std::make_pair(p_new_part->Uuid, p_new_part);
			Mod::AllObjects.insert(new_pair);
			pMod->m_Objects.insert(new_pair);

			ConvData::ProgressValue++;
		}
	}
}