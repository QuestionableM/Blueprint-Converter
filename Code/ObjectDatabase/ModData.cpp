#include "ModData.h"

#include "ObjectDatabase\KeywordReplacer.h"
#include "ObjectDatabase\DatabaseLoader.h"

#include "Lib\ConvData.h"
#include "Lib\String.h"
#include "Lib\File.h"
#include "Lib\Json.h"

#include "DebugCon.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace SMBC
{
	void Mod::GetBlockMaterials(const nlohmann::json& block, Texture::TextureList& tex)
	{
		const auto& bGlass = Json::Get(block, "glass");
		const auto& bAlpha = Json::Get(block, "alpha");

		if (bGlass.is_boolean() && bGlass.get<bool>())
		{
			tex.material = L"BlockGlass";
		}
		else
		{
			tex.material = L"BlockDifAsgNor";

			if (bAlpha.is_boolean() && bAlpha.get<bool>())
				tex.material.append(L"Alpha");
		}
	}

	static const std::string blkTexNames[3] = { "dif", "asg", "nor" };
	bool Mod::GetBlockTextures(const nlohmann::json& block, Texture::TextureList& tex)
	{
		for (int a = 0; a < 3; a++)
		{
			const auto& bTexture = Json::Get(block, blkTexNames[a]);

			if (bTexture.is_string())
			{
				std::wstring& strRef = tex.GetStringRef(a);

				strRef = String::ToWide(bTexture.get<std::string>());
				strRef = PathReplacer::ReplaceKey(strRef);
			}
		}

		return tex.HasTextures();
	}

	void Mod::LoadBlocks(const nlohmann::json& fJson)
	{
		const auto& bList = Json::Get(fJson, "blockList");
		if (!bList.is_array()) return;

		ConvData::ProgressMax += bList.size();
		for (const auto& lBlock : bList)
		{
			const auto& bUuid   = Json::Get(lBlock, "uuid");
			const auto& bTiling = Json::Get(lBlock, "tiling");
			const auto& bColor  = Json::Get(lBlock, "color");

			if (!bUuid.is_string()) continue;

			SMBC::Uuid uuid_obj(bUuid.get<std::string>());

			if (AllObjects.find(uuid_obj) != AllObjects.end())
			{
				DebugWarningL("An object with this uuid already exists! (", uuid_obj.ToString(), ")");
				continue;
			}

			Texture::TextureList tex_list;
			if (!Mod::GetBlockTextures(lBlock, tex_list)) continue;
			Mod::GetBlockMaterials(lBlock, tex_list);

			int tiling_value = (bTiling.is_number() ? bTiling.get<int>() : 4);
			if (tiling_value > 16 || tiling_value <= 0) tiling_value = 4;

			const std::wstring l_NameWstr = m_LanguageDb.GetTranslation(uuid_obj);

			BlockData* new_blk = new BlockData(uuid_obj, l_NameWstr, tex_list, tiling_value);
			new_blk->ModPtr = this;

			const auto new_pair = std::make_pair(new_blk->Uuid, new_blk);
			Mod::AllObjects.insert(new_pair);
			m_Objects.insert(new_pair);

			ConvData::ProgressValue++;
		}
	}

	void Mod::LoadTextureList(const nlohmann::json& texList, Texture::TextureList& entry)
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

	void Mod::AddSubMesh(const nlohmann::json& subMesh, Texture::Texture& tex, const std::wstring& idx)
	{
		const auto& sTexList = Json::Get(subMesh, "textureList");
		if (!sTexList.is_array()) return;

		Texture::TextureList new_entry;
		Mod::LoadTextureList(sTexList, new_entry);

		const auto& sMaterial = Json::Get(subMesh, "material");
		new_entry.material = (sMaterial.is_string() ? String::ToWide(sMaterial.get<std::string>()) : L"DifAsgNor");

		tex.AddEntry(idx, new_entry);
	}

	bool Mod::TryLoadSubMeshList(const nlohmann::json& pLodItem, Texture::Texture& tex)
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

			Mod::AddSubMesh(subMesh, out_tex, std::to_wstring(cur_idx));
			_idx++;
		}

		tex = out_tex;
		return true;
	}

	bool Mod::TryLoadSubMeshMap(const nlohmann::json& pLodItem, Texture::Texture& tex)
	{
		const auto& pMeshMap = Json::Get(pLodItem, "subMeshMap");
		if (!pMeshMap.is_object()) return false;

		Texture::Texture out_tex(Texture::TextureType::SubMeshMap);
		for (const auto& subMesh : pMeshMap.items())
		{
			if (!subMesh.value().is_object()) continue;

			std::wstring subMeshName = String::ToWide(subMesh.key());
			Mod::AddSubMesh(subMesh.value(), out_tex, subMeshName);
		}

		tex = out_tex;
		return true;
	}

	bool Mod::LoadSubMeshes(const nlohmann::json& pLodItem, Texture::Texture& tex)
	{
		if (Mod::TryLoadSubMeshList(pLodItem, tex) || Mod::TryLoadSubMeshMap(pLodItem, tex))
			return true;

		return false;
	}

	bool Mod::LoadRenderable(const nlohmann::json& pRenderable, Texture::Texture& tex_data, std::wstring& mesh_path)
	{
		const auto& rLodList = Json::Get(pRenderable, "lodList");
		if (!rLodList.is_array() || rLodList.size() == 0) return false;

		const auto& rLodItem = Json::Get(rLodList, 0);
		if (!rLodItem.is_object()) return false;

		const auto& rMesh = Json::Get(rLodItem, "mesh");
		if (!rMesh.is_string()) return false;

		mesh_path = String::ToWide(rMesh.get<std::string>());
		mesh_path = PathReplacer::ReplaceKey(mesh_path);

		return Mod::LoadSubMeshes(rLodItem, tex_data);
	}

	bool Mod::GetRenderableData(const nlohmann::json& part, Texture::Texture& tex_data, std::wstring& mesh_path)
	{
		const auto& pRenderable = Json::Get(part, "renderable");

		nlohmann::json rend_data;
		if (pRenderable.is_string())
		{
			std::wstring pRendWstr = String::ToWide(pRenderable.get<std::string>());
			pRendWstr = PathReplacer::ReplaceKey(pRendWstr);

			rend_data = Json::LoadParseJson(pRendWstr);
			if (!rend_data.is_object()) return false;
		}
		else if (pRenderable.is_object())
		{
			rend_data = pRenderable;
		}
		else
		{
			return false;
		}

		return Mod::LoadRenderable(rend_data, tex_data, mesh_path);
	}

	glm::vec3 Mod::LoadPartCollision(const nlohmann::json& collision)
	{
		glm::vec3 out_coll(1.0f);

		bool isBoxCol = collision.contains("box");
		bool isHullCol = collision.contains("hull");
		if (isBoxCol || isHullCol)
		{
			const auto& b_Col = collision.at(isBoxCol ? "box" : "hull");

			if (b_Col.is_object())
			{
				const auto& xPos = Json::Get(b_Col, "x");
				const auto& yPos = Json::Get(b_Col, "y");
				const auto& zPos = Json::Get(b_Col, "z");

				if (xPos.is_number() && yPos.is_number() && zPos.is_number())
					out_coll = glm::vec3(xPos.get<float>(), yPos.get<float>(), zPos.get<float>());
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
					float f_Diameter = c_Diameter.get<float>();
					float f_Depth = c_Depth.get<float>();

					const auto& c_Axis = Json::Get(cyl_col, "axis");
					std::string c_AxisStr = (c_Axis.is_string() ? c_Axis.get<std::string>() : "z");

					if (c_AxisStr == "x" || c_AxisStr == "X")
						out_coll = glm::vec3(f_Depth, f_Diameter, f_Diameter);
					else if (c_AxisStr == "y" || c_AxisStr == "Y")
						out_coll = glm::vec3(f_Diameter, f_Depth, f_Diameter);
					else if (c_AxisStr == "z" || c_AxisStr == "Z")
						out_coll = glm::vec3(f_Diameter, f_Diameter, f_Depth);
				}
			}
			else
			{
				const auto& sphere_col = Json::Get(collision, "sphere");
				if (sphere_col.is_object())
				{
					const auto& s_Diameter = Json::Get(sphere_col, "diameter");
					if (s_Diameter.is_number())
						out_coll = glm::vec3(s_Diameter.get<float>());
				}
			}
		}

		return out_coll;
	}

	void Mod::LoadParts(const nlohmann::json& fJson)
	{
		const auto& pList = Json::Get(fJson, "partList");
		if (!pList.is_array()) return;

		ConvData::ProgressMax += pList.size();
		for (const auto& lPart : pList)
		{
			const auto& pUuid = Json::Get(lPart, "uuid");
			const auto& pColor = Json::Get(lPart, "color");
			const auto& pRenderable = Json::Get(lPart, "renderable");

			if (!pUuid.is_string()) continue;

			SMBC::Uuid uuid_obj(pUuid.get<std::string>());

			if (AllObjects.find(uuid_obj) != AllObjects.end())
			{
				DebugWarningL("An object with this uuid already exists! (", uuid_obj.ToString(), ")");
				continue;
			}

			std::wstring mesh_path;
			Texture::Texture tex_data;
			if (!Mod::GetRenderableData(lPart, tex_data, mesh_path))
				continue;

			const std::wstring l_NameWstr = m_LanguageDb.GetTranslation(uuid_obj);
			const glm::vec3 pBounds = Mod::LoadPartCollision(lPart);

			PartData* new_part = new PartData(uuid_obj, mesh_path, l_NameWstr, tex_data, pBounds);
			new_part->ModPtr = this;

			const auto new_pair = std::make_pair(new_part->Uuid, new_part);
			Mod::AllObjects.insert(new_pair);
			m_Objects.insert(new_pair);

			ConvData::ProgressValue++;
		}
	}

	void Mod::LoadObjectFile(const std::wstring& path)
	{
		nlohmann::json mObjectFile = Json::LoadParseJson(path);
		if (!mObjectFile.is_object())
		{
			DebugErrorL("Couldn't load object database: ", path);
			return;
		}

		this->LoadBlocks(mObjectFile);
		this->LoadParts(mObjectFile);
	}

	void Mod::LoadObjectsFromDirectory(const std::wstring& dir)
	{
		std::error_code mError;
		fs::directory_iterator dir_iter(dir, fs::directory_options::skip_permission_denied, mError);

		for (const auto& mCurDir : dir_iter)
		{
			if (mError)
			{
				DebugErrorL("Failed to get an item in: ", dir);
				continue;
			}

			if (!mCurDir.is_regular_file()) continue;

			const fs::path& fPath = mCurDir.path();

			if (fPath.has_filename() && fPath.has_extension() && fPath.extension() == ".json")
				this->LoadObjectFile(fPath.wstring());
		}
	}

	void Mod::LoadTranslations(const std::wstring& path)
	{
		const std::wstring _Path = (path.empty() ? (m_Directory + L"/Gui/Language/English") : path);

		const std::wstring _LangFile = _Path + L"/inventoryDescriptions.json";
		const std::wstring _OtherLangFile = _Path + L"/InventoryItemDescriptions.json";

		const std::wstring _FinalPath = (File::Exists(_LangFile) ? _LangFile : File::Exists(_OtherLangFile) ? _OtherLangFile : L"");

		m_LanguageDb.LoadLanguageFile(_FinalPath);
	}

	static std::wstring g_ShapeSetExtensions[2] =
	{
		L".shapedb",
		L".json"
	};

	bool FindShapeSetFile(const std::wstring& mod_path, std::wstring& output_path)
	{
		const std::wstring l_DatabaseDir = mod_path + L"/Objects/Database/shapesets";

		for (__int8 a = 0; a < 2; a++)
		{
			const std::wstring l_FullPath = l_DatabaseDir + g_ShapeSetExtensions[a];
			if (File::Exists(l_FullPath))
			{
				output_path = l_FullPath;
				return true;
			}
		}

		return false;
	}

	void Mod::LoadShapeSetList(const std::wstring& path)
	{
		const nlohmann::json l_ShapeSetListFile = Json::LoadParseJson(path);
		if (!l_ShapeSetListFile.is_object()) return;

		const auto& l_ShapeSetList = Json::Get(l_ShapeSetListFile, "shapeSetList");
		if (!l_ShapeSetList.is_array()) return;

		for (const auto& path_shapedb : l_ShapeSetList)
		{
			if (!path_shapedb.is_string()) continue;

			const std::wstring l_path_wide = String::ToWide(path_shapedb.get<std::string>());
			const std::wstring l_full_path = PathReplacer::ReplaceKey(l_path_wide);

			this->LoadObjectFile(l_full_path);
		}
	}

	void Mod::LoadObjectDatabase()
	{
		this->LoadTranslations();
		PathReplacer::SetModData(m_Directory, m_Uuid);

		std::wstring l_ShapeSetPath;
		if (FindShapeSetFile(m_Directory, l_ShapeSetPath))
		{
			this->LoadShapeSetList(l_ShapeSetPath);
		}
		else
		{
			const std::wstring l_DbPath = m_Directory + L"/Objects/Database/ShapeSets";
			if (!File::Exists(l_DbPath)) return;

			this->LoadObjectsFromDirectory(l_DbPath);
		}
		//const std::wstring lShapeDbFile = this->Path + L"/Objects/Database/shapesets.shapedb";
		//const std::wstring lShapeDbFile2 = this->Path + L"/Objects/Database/shapesets.json";
		//check is the shapesets.json/.shapedb file exists
		//const std::wstring lDatabasePath = this->Path + L"/Objects/Database";
		/*
		const std::wstring mDatabasePath = mModDir + L"/Objects/Database/ShapeSets";
		if (!File::Exists(mDatabasePath)) continue;

		pNewMod->LoadTranslations();

		PathReplacer::SetModData(pNewMod->Path, pNewMod->Uuid);
		pNewMod->LoadObjectsFromDirectory(mDatabasePath);
		*/
	}

	std::size_t Mod::GetObjectCount()
	{
		return AllObjects.size();
	}

	std::size_t Mod::GetModCount()
	{
		return Mods.size();
	}

	void Mod::ClearMods()
	{
		DebugOutL("Removing ", 0b1101_fg, Mods.size(), 0b1110_fg, " mods from memory...");

		for (auto& mod : Mod::Mods)
			delete mod.second;

		Mod::Mods.clear();
		Mod::AllObjects.clear();
	}

	const ObjectData* Mod::GetObject(const SMBC::Uuid& uuid)
	{
		if (AllObjects.find(uuid) == AllObjects.end())
			return nullptr;

		return AllObjects.at(uuid);
	}

	const PartData* Mod::GetPart(const SMBC::Uuid& uuid)
	{
		const ObjectData* current_obj = GetObject(uuid);
		if (!current_obj || current_obj->Type() != ObjectType::Part)
			return nullptr;

		return static_cast<const PartData*>(current_obj);
	}

	const BlockData* Mod::GetBlock(const SMBC::Uuid& uuid)
	{
		const ObjectData* current_obj = GetObject(uuid);
		if (!current_obj || current_obj->Type() != ObjectType::Block)
			return nullptr;

		return static_cast<const BlockData*>(current_obj);
	}

	Mod* Mod::CreateModFromDirectory(const std::wstring& dir)
	{
		const std::wstring mDescPath = dir + L"/description.json";
		if (!File::Exists(mDescPath)) return nullptr;

		const nlohmann::json mDescJson = Json::LoadParseJson(mDescPath, true);
		if (!mDescJson.is_object()) return nullptr;

		const auto& mType = Json::Get(mDescJson, "type");
		if (!mType.is_string() || mType.get<std::string>() != "Blocks and Parts")
			return nullptr;

		const auto& mUuid = Json::Get(mDescJson, "localId");
		const auto& mName = Json::Get(mDescJson, "name");

		if (!mUuid.is_string() || !mName.is_string()) return nullptr;

		const auto& lWorkshopIdStr = Json::Get(mDescJson, "fileId");
		const unsigned __int64 lWorkshopId = (lWorkshopIdStr.is_number() ? lWorkshopIdStr.get<unsigned __int64>() : 0ull);

		return Mod::CreateMod(
			SMBC::Uuid(mUuid.get<std::string>()),
			String::ToWide(mName.get<std::string>()),
			lWorkshopId,
			dir
		);
	}

	Mod* Mod::CreateMod(
		const SMBC::Uuid& uuid,
		const std::wstring& name,
		const unsigned __int64& workshop_id,
		const std::wstring& dir)
	{
		if (Mod::Mods.find(uuid) != Mod::Mods.end())
		{
			DebugWarningL("Uuid conflict between 2 mods: \"", name, "\" and \"", Mod::Mods.at(uuid)->m_Name, "\" (", uuid.ToString(), ")");
			return nullptr;
		}

		Mod* pNewMod = new Mod();
		pNewMod->m_Uuid = uuid;
		pNewMod->m_Name = name;
		pNewMod->m_WorkshopId = workshop_id;
		pNewMod->m_Directory = dir;

		Mod::Mods.insert(std::make_pair(uuid, pNewMod));
		return pNewMod;
	}

	Mod::~Mod()
	{
		for (auto& obj : m_Objects)
			delete obj.second;
	}
}