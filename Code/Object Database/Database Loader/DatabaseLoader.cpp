#include "DatabaseLoader.h"

#include "Object Database/Mod Data/ModData.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/String/String.h"
#include "Lib/ConvData/ConvData.h"
#include "Lib/File/FileFunc.h"
#include "Lib/ProgramSettings.h"

#include <filesystem>

#include "DebugCon.h"

namespace fs = std::filesystem;

namespace SMBC
{
	glm::vec3 DatabaseLoader::LoadBlockCollision(const nlohmann::json& collision)
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

				if (c_Diameter.is_number() && c_Depth.is_number()) {
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
				if (sphere_col.is_object()) {
					const auto& s_Diameter = Json::Get(sphere_col, "diameter");
					if (s_Diameter.is_number())
						out_coll = glm::vec3(s_Diameter.get<float>());
				}
			}
		}

		return out_coll;
	}

	void DatabaseLoader::GetTextureArray(
		const nlohmann::json& textureList,
		std::wstring& dif,
		std::wstring& asg,
		std::wstring& nor
	) {
		size_t sz_list = textureList.size();
		size_t arr_size = (sz_list > 3 ? 3 : sz_list);

		for (size_t a = 0; a < arr_size; a++) {
			auto& texObj = textureList[a];

			std::wstring texStr = (texObj.is_string() ? String::ToWide(texObj.get<std::string>()) : L"");
			switch (a) {
			case 0: dif = texStr; break;
			case 1: asg = texStr; break;
			case 2: nor = texStr; break;
			}
		}
	}

	void DatabaseLoader::LoadSubMeshTextures(
		const nlohmann::json& subMesh,
		const std::wstring& texKey,
		Texture::Texture& texture
	) {
		const auto& t_List = Json::Get(subMesh, "textureList");
		if (!t_List.is_array()) return;

		std::wstring t_Dif, t_Asg, t_Nor;
		DatabaseLoader::GetTextureArray(t_List, t_Dif, t_Asg, t_Nor);

		Texture::TextureList _NewList(t_Dif, t_Asg, t_Nor);
		_NewList.ReplaceTextureKeys();

		texture.AddTexture(texKey, _NewList);
	}

	bool DatabaseLoader::TryLoadSubMeshList(
		const nlohmann::json& lod,
		Texture::Texture& texture
	) {
		const auto& SubMeshList = Json::Get(lod, "subMeshList");
		if (!SubMeshList.is_array()) return false;

		Texture::Texture _Output(Texture::TextureType::SubMeshList);

		uint32_t _idx = 0;
		for (const auto& SubMesh : SubMeshList) {
			if (!SubMesh.is_object()) continue;

			const auto& idx_obj = Json::Get(SubMesh, "idx");
			uint32_t m_Idx = (idx_obj.is_number() ? idx_obj.get<int>() : _idx);

			DatabaseLoader::LoadSubMeshTextures(SubMesh, std::to_wstring(m_Idx), _Output);
			_idx++;
		}

		texture = _Output;
		return true;
	}

	bool DatabaseLoader::TryLoadSubMeshMap(
		const nlohmann::json& lod,
		Texture::Texture& texture
	) {
		const auto& SubMeshMap = Json::Get(lod, "subMeshMap");
		if (!SubMeshMap.is_object()) return false;

		Texture::Texture _Output(Texture::TextureType::SubMeshMap);

		for (const auto& SubMesh : SubMeshMap.items()) {
			if (!SubMesh.value().is_object()) continue;

			std::wstring _WstrKey = String::ToWide(SubMesh.key());
			DatabaseLoader::LoadSubMeshTextures(SubMesh.value(), _WstrKey, _Output);
		}

		texture = _Output;
		return true;
	}

	void DatabaseLoader::GetRenderableData(
		const nlohmann::json& renderable,
		Texture::Texture& texture,
		std::wstring& model_path
	) {
		const auto& _LodList = Json::Get(renderable, "lodList");
		if (!_LodList.is_array()) return;

		const auto& _LodItem = Json::Get(_LodList, 0);
		if (!_LodItem.is_object()) return;

		const auto& _Mesh = Json::Get(_LodItem, "mesh");
		if (!_Mesh.is_string()) return;
		std::wstring _WstrMesh = PathReplacer::ReplaceKey(String::ToWide(_Mesh.get<std::string>()));

		if (_WstrMesh.empty()) return;
		model_path = _WstrMesh;

		if (DatabaseLoader::TryLoadSubMeshList(_LodItem, texture))
			return;

		if (DatabaseLoader::TryLoadSubMeshMap(_LodItem, texture))
			return;
	}

	void DatabaseLoader::LoadRenderableObject(
		Mod*& mod,
		const nlohmann::json& r_Object,
		const std::wstring& uuid,
		const std::wstring& name,
		const glm::vec3& bounds
	) {
		std::wstring _Mesh = L"";
		Texture::Texture _TextureList(Texture::TextureType::None);

		DatabaseLoader::GetRenderableData(r_Object, _TextureList, _Mesh);
		if (_Mesh.empty() || _TextureList.TexType == Texture::TextureType::None) return;

		PartData* part_data = new PartData(uuid, _Mesh, name, _TextureList, bounds);

		mod->AddObject(part_data);

		ConvData::ProgressValue++;
	}

	void DatabaseLoader::LoadBlockList(const nlohmann::json& f_Json, Mod*& mod)
	{
		const auto& b_List = Json::Get(f_Json, "blockList");
		if (!b_List.is_array()) return;

		ConvData::ProgressMax += b_List.size();
		for (auto& _Block : b_List) {
			const auto& _Uuid = Json::Get(_Block, "uuid");
			if (!_Uuid.is_string()) continue;

			std::wstring _WstrUuid = String::ToWide(_Uuid.get<std::string>());
			const std::wstring& _BlockName = mod->LanguageDB.GetTranslation(_WstrUuid);

			const auto& _Dif = Json::Get(_Block, "dif");
			const auto& _Asg = Json::Get(_Block, "asg");
			const auto& _Nor = Json::Get(_Block, "nor");
			const auto& _Tiling = Json::Get(_Block, "tiling");

			Texture::TextureList _TexList;

			_TexList.dif = (_Dif.is_string() ? String::ToWide(_Dif.get<std::string>()) : L"");
			_TexList.asg = (_Asg.is_string() ? String::ToWide(_Asg.get<std::string>()) : L"");
			_TexList.nor = (_Nor.is_string() ? String::ToWide(_Nor.get<std::string>()) : L"");
			_TexList.ReplaceTextureKeys();

			int _TilingValue = (_Tiling.is_number() ? _Tiling.get<int>() : 4);
			if (_TilingValue > 16 || _TilingValue <= 0) _TilingValue = 4;

			BlockData* _BlockData = new BlockData(
				_WstrUuid,
				_BlockName + (L" (" + mod->LanguageDB.Environment + L": " + _WstrUuid + L")"),
				_TexList,
				_TilingValue
			);

			mod->AddObject(_BlockData);

			ConvData::ProgressValue++;
		}
	}

	void DatabaseLoader::LoadPartList(const nlohmann::json& f_Json, Mod*& mod)
	{
		const auto& p_List = Json::Get(f_Json, "partList");
		if (!p_List.is_array()) return;

		ConvData::ProgressMax += p_List.size();
		for (auto& _Part : p_List) {
			const auto& _Uuid = Json::Get(_Part, "uuid");
			if (!_Uuid.is_string()) continue;

			std::wstring _WstrUuid = String::ToWide(_Uuid.get<std::string>());
			if (mod->UuidExists(_WstrUuid)) continue;

			const std::wstring& _ObjName = mod->LanguageDB.GetTranslation(_WstrUuid);
			const std::wstring& f_ObjName = _ObjName + (L" (" + mod->LanguageDB.Environment + L": " + _WstrUuid + L")");

			glm::vec3 _ObjectBounds = DatabaseLoader::LoadBlockCollision(_Part);
			const auto& _Renderable = Json::Get(_Part, "renderable");

			if (_Renderable.is_object()) {
				DatabaseLoader::LoadRenderableObject(mod, _Renderable, _WstrUuid, f_ObjName, _ObjectBounds);
			}
			else if (_Renderable.is_string()) {
				std::wstring _RenderableWstr = String::ToWide(_Renderable.get<std::string>());
				_RenderableWstr = PathReplacer::ReplaceKey(_RenderableWstr);

				nlohmann::json _RendFile = Json::LoadParseJson(_RenderableWstr);

				DatabaseLoader::LoadRenderableObject(mod, _RendFile, _WstrUuid, f_ObjName, _ObjectBounds);
			}
		}
	}

	void DatabaseLoader::LoadObjectFile(Mod* mod, const std::wstring& path)
	{
		nlohmann::json _ObjectFile = Json::LoadParseJson(path);
		if (!_ObjectFile.is_object()) return;

		DatabaseLoader::LoadBlockList(_ObjectFile, mod);
		DatabaseLoader::LoadPartList(_ObjectFile, mod);
	}

	void DatabaseLoader::LoadGameDatabase()
	{
		Mod* _VanillaParts = Mod::CreateMod(
			L"00000000-0000-0000-0000-000000000000",
			L"VANILLA GAME",
			L"",
			Settings::PathToSM
		);

		for (std::wstring& path : Settings::VanillaLanguagePaths)
			_VanillaParts->LoadTranslations(path);

		for (std::wstring& data_path : Settings::SMDirDatabase)
		{
			if (!File::Exists(data_path)) continue;

			fs::directory_entry CurPath(data_path);
			if (CurPath.is_directory())
			{
				fs::directory_iterator _DifIter(CurPath.path().wstring(), fs::directory_options::skip_permission_denied);
				for (auto& dir : _DifIter)
				{
					if (!dir.is_regular_file()) continue;

					DatabaseLoader::LoadObjectFile(_VanillaParts, dir.path().wstring());
				}
			}
			else if (CurPath.is_regular_file())
				DatabaseLoader::LoadObjectFile(_VanillaParts, CurPath.path().wstring());
		}
	}

	void DatabaseLoader::LoadModDatabase()
	{
		if (Settings::ModFolders.empty()) return;

		for (std::wstring& _ModDirectory : Settings::ModFolders)
		{
			if (!File::Exists(_ModDirectory)) continue;

			fs::directory_iterator _DirIter(_ModDirectory, fs::directory_options::skip_permission_denied);
			for (auto& dir : _DirIter)
			{
				if (!dir.is_directory()) continue;

				Mod* NewMod = Mod::CreateModFromDirectory(dir.path().wstring());
				if (!NewMod) continue;

				NewMod->LoadTranslations();

				PathReplacer::SetModData(NewMod->Path, NewMod->Uuid);
				NewMod->LoadObjects();
			}
		}
	}

	void DatabaseLoader::LoadDatabase()
	{
		Mod::ClearMods();

		ConvData::SetState(State::ReadingDatabase);

		DatabaseLoader::LoadGameDatabase();
		DatabaseLoader::LoadModDatabase();
	}
}