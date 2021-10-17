#include "ModData.h"
#include "Lib/File/FileFunc.h"
#include "Object Database/Database Loader/DatabaseLoader.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"
#include "Lib/ConvData/ConvData.h"
#include "Lib/String/String.h"
#include "DebugCon.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace SMBC
{
	std::unordered_map<Uuid, ObjectData*> Mod::AllObjects = {};
	std::unordered_map<Uuid, Mod*>        Mod::Mods       = {};

	bool Mod::GetBlockTextures(const nlohmann::json& block, SMBC::Texture::TextureList& tex)
	{
		const auto& bDif = Json::Get(block, "dif");
		const auto& bAsg = Json::Get(block, "asg");
		const auto& bNor = Json::Get(block, "nor");

		tex.dif = (bDif.is_string() ? String::ToWide(bDif.get<std::string>()) : L"");
		tex.asg = (bAsg.is_string() ? String::ToWide(bAsg.get<std::string>()) : L"");
		tex.nor = (bNor.is_string() ? String::ToWide(bNor.get<std::string>()) : L"");

		tex.ReplaceTextureKeys();

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

			SMBC::Texture::TextureList tex_list;
			if (!Mod::GetBlockTextures(lBlock, tex_list)) continue;

			int tiling_value = (bTiling.is_number() ? bTiling.get<int>() : 4);
			if (tiling_value > 16 || tiling_value <= 0) tiling_value = 4;

			const std::wstring& bNameWstr = this->LanguageDB.GetTranslation(uuid_obj);

			BlockData* new_blk = new BlockData(uuid_obj, bNameWstr, tex_list, tiling_value);
			new_blk->ModPtr = this;

			const auto new_pair = std::make_pair(new_blk->Uuid, new_blk);
			AllObjects.insert(new_pair);
			Objects.insert(new_pair);

			ConvData::ProgressValue++;
		}
	}

	void Mod::LoadTextureList(const nlohmann::json& texList, Texture::TextureList& entry)
	{
		std::size_t arr_sz = texList.size();
		std::size_t list_sz = (arr_sz > 3 ? 3 : arr_sz);

		for (std::size_t a = 0; a < list_sz; a++)
		{
			const auto& cur_item = texList.at(a);

			std::wstring wstr_path = L"";
			if (cur_item.is_string())
			{
				wstr_path = String::ToWide(cur_item.get<std::string>());
				wstr_path = PathReplacer::ReplaceKey(wstr_path);
			}

			switch (a)
			{
			case 0: entry.dif = wstr_path; break;
			case 1: entry.asg = wstr_path; break;
			case 2: entry.nor = wstr_path; break;
			}
		}

		entry.ReplaceTextureKeys();
	}

	void Mod::AddSubMesh(const nlohmann::json& subMesh, Texture::Texture& tex, const std::wstring& idx)
	{
		const auto& sTexList = Json::Get(subMesh, "textureList");
		if (!sTexList.is_array()) return;

		Texture::TextureList new_entry;
		Mod::LoadTextureList(sTexList, new_entry);

		tex.TexList.insert(std::make_pair(idx, new_entry));
	}

	bool Mod::LoadSubMeshes(const nlohmann::json& pLodItem, Texture::Texture& tex)
	{
		const auto& pMeshList = Json::Get(pLodItem, "subMeshList");
		if (pMeshList.is_array())
		{
			Texture::Texture out_tex(Texture::TextureType::SubMeshList);

			std::size_t _idx = 0;
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
		else
		{
			const auto& pMeshMap = Json::Get(pLodItem, "subMeshMap");
			if (pMeshMap.is_object())
			{
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
		}

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

			const std::wstring& pNameWstr = this->LanguageDB.GetTranslation(uuid_obj);
			const glm::vec3 pBounds = Mod::LoadPartCollision(lPart);

			PartData* new_part = new PartData(uuid_obj, mesh_path, pNameWstr, tex_data, pBounds);
			new_part->ModPtr = this;

			const auto new_pair = std::make_pair(new_part->Uuid, new_part);

			AllObjects.insert(new_pair);
			Objects.insert(new_pair);

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
		try
		{
			fs::directory_iterator dir_iter(dir, fs::directory_options::skip_permission_denied);

			for (const auto& dir : dir_iter)
			{
				if (!dir.is_regular_file()) continue;

				const fs::path& fPath = dir.path();

				if (fPath.has_filename() && fPath.has_extension() && fPath.extension() == ".json")
					this->LoadObjectFile(fPath.wstring());
			}
		}
		catch (...) { DebugErrorL("Scan Error: ", dir); }
	}

	void Mod::LoadTranslations(const std::wstring& path)
	{
		std::wstring _Path = (path.empty() ? (this->Path + L"/Gui/Language/English") : path);

		std::wstring _LangFile = _Path + L"/inventoryDescriptions.json";
		std::wstring _OtherLangFile = _Path + L"/InventoryItemDescriptions.json";

		std::wstring _FinalPath = (File::Exists(_LangFile) ? _LangFile : File::Exists(_OtherLangFile) ? _OtherLangFile : L"");

		this->LanguageDB.LoadLanguageFile(_FinalPath);
	}

	void Mod::AddObject(ObjectData* object)
	{
		if (AllObjects.find(object->Uuid) != AllObjects.end())
			return;

		const auto new_pair = std::make_pair(object->Uuid, object);

		Objects.insert(new_pair);
		AllObjects.insert(new_pair);
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
		for (auto& mod : Mods)
			delete mod.second;

		Mods.clear();
		AllObjects.clear();
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

		return (current_obj != nullptr) ? current_obj->ToPart() : nullptr;
	}

	const BlockData* Mod::GetBlock(const SMBC::Uuid& uuid)
	{
		const ObjectData* current_obj = GetObject(uuid);
		
		return (current_obj != nullptr) ? current_obj->ToBlock() : nullptr;
	}

	Mod* Mod::CreateModFromDirectory(const std::wstring& dir)
	{
		const std::wstring mDescPath = dir + L"/description.json";

		nlohmann::json mDescJson = Json::LoadParseJson(mDescPath, true);
		if (!mDescJson.is_object()) return nullptr;

		const auto& mType = Json::Get(mDescJson, "type");
		if (!mType.is_string() || mType.get<std::string>() != "Blocks and Parts")
			return nullptr;

		const auto& mUuid = Json::Get(mDescJson, "localId");
		const auto& mName = Json::Get(mDescJson, "name");

		if (!mUuid.is_string() || !mName.is_string()) return nullptr;

		const auto& mFileIdStr = Json::Get(mDescJson, "fileId");
		long long mFileId = mFileIdStr.is_number() ? mFileIdStr.get<long long>() : 0ll;

		return Mod::CreateMod(
			SMBC::Uuid(mUuid.get<std::string>()),
			String::ToWide(mName.get<std::string>()),
			(mFileId > 0) ? std::to_wstring(mFileId) : L"",
			dir
		);
	}

	Mod* Mod::CreateMod(
		const SMBC::Uuid& uuid,
		const std::wstring& name,
		const std::wstring& workshop_id,
		const std::wstring& dir
	) {
		if (Mods.find(uuid) != Mods.end())
		{
			DebugErrorL("The specified uuid is already used by the other mod: ", Mods.at(uuid)->Name, " (", uuid.ToString(), ")");
			return nullptr;
		}

		Mod* NewMod = new Mod();
		NewMod->Uuid = uuid;
		NewMod->Name = name;
		NewMod->WorkshopId = workshop_id;
		NewMod->Path = dir;

		DebugOutL("Loaded a new mod: ", Color::YELLOW_INT, NewMod->Name);

		Mods.insert(std::make_pair(uuid, NewMod));
		return NewMod;
	}

	Mod::~Mod()
	{
		for (auto& obj : Objects)
			delete obj.second;
	}
}