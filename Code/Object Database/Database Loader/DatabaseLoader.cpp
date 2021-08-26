#include "DatabaseLoader.h"

#include "Object Database/Mod Data/ModData.h"
#include "Object Database/ObjectDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/String/String.h"
#include "Lib/ConvData/ConvData.h"
#include "Lib/File/FileFunc.h"
#include "Lib/ProgramSettings.h"

#include <filesystem>

#include "DebugCon.h"

namespace fs = std::filesystem;
typedef SMBC::DatabaseLoader _DatabaseLoader;

glm::vec3 _DatabaseLoader::LoadBlockCollision(const nlohmann::json& collision) {
	glm::vec3 out_coll(1.0f);

	bool isBoxCol = collision.contains("box");
	bool isHullCol = collision.contains("hull");
	if (isBoxCol || isHullCol) {
		const auto& b_Col = collision.at(isBoxCol ? "box" : "hull");

		if (b_Col.is_object()) {
			const auto& xPos = SMBC::Json::Get(b_Col, "x");
			const auto& yPos = SMBC::Json::Get(b_Col, "y");
			const auto& zPos = SMBC::Json::Get(b_Col, "z");

			if (xPos.is_number() && yPos.is_number() && zPos.is_number())
				out_coll = glm::vec3(xPos.get<float>(), yPos.get<float>(), zPos.get<float>());
		}
	}
	else {
		const auto& cyl_col = SMBC::Json::Get(collision, "cylinder");
		if (cyl_col.is_object()) {
			const auto& c_Diameter = SMBC::Json::Get(cyl_col, "diameter");
			const auto& c_Depth = SMBC::Json::Get(cyl_col, "depth");

			if (c_Diameter.is_number() && c_Depth.is_number()) {
				float f_Diameter = c_Diameter.get<float>();
				float f_Depth = c_Depth.get<float>();

				const auto& c_Axis = SMBC::Json::Get(cyl_col, "axis");
				std::string c_AxisStr = (c_Axis.is_string() ? c_Axis.get<std::string>() : "z");

				if (c_AxisStr == "x" || c_AxisStr == "X")
					out_coll = glm::vec3(f_Depth, f_Diameter, f_Diameter);
				else if (c_AxisStr == "y" || c_AxisStr == "Y")
					out_coll = glm::vec3(f_Diameter, f_Depth, f_Diameter);
				else if (c_AxisStr == "z" || c_AxisStr == "Z")
					out_coll = glm::vec3(f_Diameter, f_Diameter, f_Depth);
			}
		}
		else {
			const auto& sphere_col = SMBC::Json::Get(collision, "sphere");
			if (sphere_col.is_object()) {
				const auto& s_Diameter = SMBC::Json::Get(sphere_col, "diameter");
				if (s_Diameter.is_number())
					out_coll = glm::vec3(s_Diameter.get<float>());
			}
		}
	}

	return out_coll;
}

void _DatabaseLoader::GetTextureArray(
	const nlohmann::json& textureList,
	std::wstring& dif,
	std::wstring& asg,
	std::wstring& nor
) {
	size_t sz_list = textureList.size();
	size_t arr_size = (sz_list > 3 ? 3 : sz_list);

	for (size_t a = 0; a < arr_size; a++) {
		auto& texObj = textureList[a];

		std::wstring texStr = (texObj.is_string() ? SMBC::String::ToWide(texObj.get<std::string>()) : L"");
		switch (a) {
		case 0: dif = texStr; break;
		case 1: asg = texStr; break;
		case 2: nor = texStr; break;
		}
	}
}

void _DatabaseLoader::LoadSubMeshTextures(
	const nlohmann::json& subMesh,
	const std::wstring& texKey,
	SMBC::Texture::Texture& texture
) {
	const auto& t_List = SMBC::Json::Get(subMesh, "textureList");
	if (!t_List.is_array()) return;

	std::wstring t_Dif, t_Asg, t_Nor;
	SMBC::DatabaseLoader::GetTextureArray(t_List, t_Dif, t_Asg, t_Nor);

	SMBC::Texture::TextureList _NewList(t_Dif, t_Asg, t_Nor);
	_NewList.ReplaceTextureKeys();

	texture.AddTexture(texKey, _NewList);
}

bool _DatabaseLoader::TryLoadSubMeshList(
	const nlohmann::json& lod,
	SMBC::Texture::Texture& texture
) {
	const auto& SubMeshList = SMBC::Json::Get(lod, "subMeshList");
	if (!SubMeshList.is_array()) return false;

	SMBC::Texture::Texture _Output(SMBC::Texture::TextureType::SubMeshList);

	uint32_t _idx = 0;
	for (const auto& SubMesh : SubMeshList) {
		if (!SubMesh.is_object()) continue;

		const auto& idx_obj = SMBC::Json::Get(SubMesh, "idx");
		uint32_t m_Idx = (idx_obj.is_number() ? idx_obj.get<int>() : _idx);

		SMBC::DatabaseLoader::LoadSubMeshTextures(SubMesh, std::to_wstring(m_Idx), _Output);
		_idx++;
	}

	texture = _Output;
	return true;
}

bool _DatabaseLoader::TryLoadSubMeshMap(
	const nlohmann::json& lod,
	SMBC::Texture::Texture& texture
) {
	const auto& SubMeshMap = SMBC::Json::Get(lod, "subMeshMap");
	if (!SubMeshMap.is_object()) return false;

	SMBC::Texture::Texture _Output(SMBC::Texture::TextureType::SubMeshMap);

	for (const auto& SubMesh : SubMeshMap.items()) {
		if (!SubMesh.value().is_object()) continue;

		std::wstring _WstrKey = SMBC::String::ToWide(SubMesh.key());
		SMBC::DatabaseLoader::LoadSubMeshTextures(SubMesh.value(), _WstrKey, _Output);
	}

	texture = _Output;
	return true;
}

void _DatabaseLoader::GetRenderableData(
	const nlohmann::json& renderable,
	SMBC::Texture::Texture& texture,
	std::wstring& model_path
) {
	const auto& _LodList = SMBC::Json::Get(renderable, "lodList");
	if (!_LodList.is_array()) return;

	const auto& _LodItem = SMBC::Json::Get(_LodList, 0);
	if (!_LodItem.is_object()) return;

	const auto& _Mesh = SMBC::Json::Get(_LodItem, "mesh");
	if (!_Mesh.is_string()) return;
	std::wstring _WstrMesh = SMBC::PathReplacer::ReplaceKey(SMBC::String::ToWide(_Mesh.get<std::string>()));

	if (_WstrMesh.empty()) return;
	model_path = _WstrMesh;

	if (SMBC::DatabaseLoader::TryLoadSubMeshList(_LodItem, texture))
		return;

	if (SMBC::DatabaseLoader::TryLoadSubMeshMap(_LodItem, texture))
		return;
}

void _DatabaseLoader::LoadRenderableObject(
	SMBC::ModData*& mod,
	const nlohmann::json& r_Object,
	const std::wstring& uuid,
	const std::wstring& name,
	const glm::vec3& bounds
) {
	std::wstring _Mesh = L"";
	SMBC::Texture::Texture _TextureList(SMBC::Texture::TextureType::None);

	SMBC::DatabaseLoader::GetRenderableData(r_Object, _TextureList, _Mesh);
	if (_Mesh.empty() || _TextureList.TexType == SMBC::Texture::TextureType::None) return;

	SMBC::ObjectData* _ObjData = new SMBC::ObjectData(uuid, _Mesh, name, _TextureList, bounds);

	mod->AddPart(_ObjData);

	SMBC::ConvData::ProgressValue++;
}

void _DatabaseLoader::LoadBlockList(const nlohmann::json& f_Json, SMBC::ModData*& mod) {
	const auto& b_List = SMBC::Json::Get(f_Json, "blockList");
	if (!b_List.is_array()) return;

	SMBC::ConvData::ProgressMax += b_List.size();
	for (auto& _Block : b_List) {
		const auto& _Uuid = SMBC::Json::Get(_Block, "uuid");
		if (!_Uuid.is_string()) continue;

		std::wstring _WstrUuid = SMBC::String::ToWide(_Uuid.get<std::string>());
		const std::wstring& _BlockName = mod->LanguageDB.GetTranslation(_WstrUuid);

		const auto& _Dif = SMBC::Json::Get(_Block, "dif");
		const auto& _Asg = SMBC::Json::Get(_Block, "asg");
		const auto& _Nor = SMBC::Json::Get(_Block, "nor");
		const auto& _Tiling = SMBC::Json::Get(_Block, "tiling");

		SMBC::Texture::TextureList _TexList;

		_TexList.dif = (_Dif.is_string() ? SMBC::String::ToWide(_Dif.get<std::string>()) : L"");
		_TexList.asg = (_Asg.is_string() ? SMBC::String::ToWide(_Asg.get<std::string>()) : L"");
		_TexList.nor = (_Nor.is_string() ? SMBC::String::ToWide(_Nor.get<std::string>()) : L"");
		_TexList.ReplaceTextureKeys();

		int _TilingValue = (_Tiling.is_number() ? _Tiling.get<int>() : 4);
		if (_TilingValue > 16 || _TilingValue <= 0) _TilingValue = 4;

		SMBC::BlockData* _BlockData = new SMBC::BlockData(
			_TexList,
			_WstrUuid,
			_BlockName + (L" (" + mod->LanguageDB.Environment + L": " + _WstrUuid + L")"),
			_TilingValue
		);

		mod->AddBlock(_BlockData);

		SMBC::ConvData::ProgressValue++;
	}
}

void _DatabaseLoader::LoadPartList(const nlohmann::json& f_Json, SMBC::ModData*& mod) {
	const auto& p_List = SMBC::Json::Get(f_Json, "partList");
	if (!p_List.is_array()) return;
	
	SMBC::ConvData::ProgressMax += p_List.size();
	for (auto& _Part : p_List) {
		const auto& _Uuid = SMBC::Json::Get(_Part, "uuid");
		if (!_Uuid.is_string()) continue;

		std::wstring _WstrUuid = SMBC::String::ToWide(_Uuid.get<std::string>());
		if (mod->UuidExists(_WstrUuid)) continue;

		const std::wstring& _ObjName = mod->LanguageDB.GetTranslation(_WstrUuid);
		const std::wstring& f_ObjName = _ObjName + (L" (" + mod->LanguageDB.Environment + L": " + _WstrUuid + L")");

		glm::vec3 _ObjectBounds = SMBC::DatabaseLoader::LoadBlockCollision(_Part);
		const auto& _Renderable = SMBC::Json::Get(_Part, "renderable");

		if (_Renderable.is_object()) {
			SMBC::DatabaseLoader::LoadRenderableObject(mod, _Renderable, _WstrUuid, f_ObjName, _ObjectBounds);
		}
		else if (_Renderable.is_string()) {
			std::wstring _RenderableWstr = SMBC::String::ToWide(_Renderable.get<std::string>());
			_RenderableWstr = SMBC::PathReplacer::ReplaceKey(_RenderableWstr);

			nlohmann::json _RendFile;
			if (!SMBC::Json::ParseJson(_RenderableWstr, _RendFile))
				continue;

			SMBC::DatabaseLoader::LoadRenderableObject(mod, _RendFile, _WstrUuid, f_ObjName, _ObjectBounds);
		}
	}
}

void _DatabaseLoader::LoadObjectFile(SMBC::ModData* mod, const std::wstring& path) {
	nlohmann::json _ObjectFile;
	if (!SMBC::Json::ParseJson(path, _ObjectFile)) return;

	SMBC::DatabaseLoader::LoadBlockList(_ObjectFile, mod);
	SMBC::DatabaseLoader::LoadPartList(_ObjectFile, mod);
}

void _DatabaseLoader::LoadGameDatabase() {
	SMBC::ModData* _VanillaParts = new SMBC::ModData(
		L"VANILLA GAME",
		L"",
		SMBC::Settings::PathToSM
	);

	for (std::wstring& path : SMBC::Settings::VanillaLanguagePaths)
		_VanillaParts->LoadTranslations(path);

	for (std::wstring& data_path : SMBC::Settings::SMDirDatabase) {
		if (!SMBC::File::Exists(data_path)) continue;

		fs::directory_entry CurPath(data_path);
		if (CurPath.is_directory()) {
			fs::directory_iterator _DifIter(CurPath.path().wstring(), fs::directory_options::skip_permission_denied);
			for (auto& dir : _DifIter) {
				if (!dir.is_regular_file()) continue;

				SMBC::DatabaseLoader::LoadObjectFile(_VanillaParts, dir.path().wstring());
			}
		}
		else if (CurPath.is_regular_file())
			SMBC::DatabaseLoader::LoadObjectFile(_VanillaParts, CurPath.path().wstring());
	}

	SMBC::ObjectDatabase::ModDB.insert(std::make_pair(
		L"00000000-0000-0000-0000-000000000000",
		_VanillaParts
	));
}

void _DatabaseLoader::LoadModDatabase() {
	if (SMBC::Settings::ModFolders.empty()) return;

	for (std::wstring& _ModDirectory : SMBC::Settings::ModFolders) {
		if (!SMBC::File::Exists(_ModDirectory)) continue;

		fs::directory_iterator _DirIter(_ModDirectory, fs::directory_options::skip_permission_denied);
		for (auto& dir : _DirIter) {
			if (!dir.is_directory()) continue;

			std::wstring _ModDescription = dir.path().wstring() + L"/description.json";
			
			nlohmann::json _DescJson;
			if (!SMBC::Json::ParseJson(_ModDescription, _DescJson, true))
				continue;

			std::wstring _ModType = SMBC::Json::GetWstr(_DescJson, "type");
			if (_ModType != L"Blocks and Parts") continue;

			std::wstring _ModUuid = SMBC::Json::GetWstr(_DescJson, "localId");
			if (_ModUuid.empty()) continue;
			if (SMBC::ObjectDatabase::ModDB.find(_ModUuid) != SMBC::ObjectDatabase::ModDB.end()) continue;

			std::wstring _ModName = SMBC::Json::GetWstr(_DescJson, "name");
			if (_ModName.empty()) continue;

			const auto& _FileIdJson = SMBC::Json::Get(_DescJson, "fileId");
			long long _FileId = _FileIdJson.is_number() ? _FileIdJson.get<long long>() : 0ll;

			SMBC::ModData* NewMod = new SMBC::ModData(
				_ModName,
				(_FileId > 0) ? std::to_wstring(_FileId) : L"",
				dir.path().wstring()
			);

			NewMod->LoadTranslations();

			SMBC::PathReplacer::SetModData(NewMod->path, _ModUuid);
			NewMod->LoadObjects();

			DebugOutL("[Mod] Loaded: ", SMBC::Color::YELLOW_INT, NewMod->name, SMBC::Color::WHITE, " (Objects: ", SMBC::Color::CYAN_INT, (NewMod->BlockDB.size() + NewMod->ObjectDB.size()), SMBC::Color::WHITE, ")");

			SMBC::ObjectDatabase::ModDB.insert(std::make_pair(_ModUuid, NewMod));
		}
	}
}