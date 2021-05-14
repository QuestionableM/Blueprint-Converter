#include <filesystem>
#include <fstream>

#include "ObjectDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/Functions/Functions.h"
#include "Lib/File/FileFunc.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/Json/JsonFunc.h"
#include "Lib/ProgramSettings.h"

namespace fs = std::filesystem;

SMBC::ObjectData::ObjectData(
	const std::wstring& obj_uuid,
	const std::wstring& obj_path,
	const std::wstring& obj_name,
	SMBC::Texture::Texture& obj_textures,
	const glm::vec3& obj_bounds
) {
	this->_obj_path = obj_path;
	this->_obj_uuid = obj_uuid;
	this->_obj_name = obj_name;
	this->obj_textures = obj_textures;
	this->_obj_bounds = obj_bounds;
}

SMBC::BlockData::BlockData(
	const SMBC::Texture::TextureList& tex_list,
	const std::wstring& uuid,
	const std::wstring& name,
	const int& tiling
) {
	this->_block_tex = tex_list;
	this->_obj_uuid = uuid;
	this->_obj_name = name;
	this->_tiling = tiling;
}

std::vector<SMBC::ObjectData> SMBC::ObjectDatabase::ObjectDB = {};
std::vector<SMBC::BlockData> SMBC::ObjectDatabase::BlockDB = {};
SMBC::LangDB SMBC::ObjectDatabase::GameTranslations = SMBC::LangDB(L"GAME DATA");

void SMBC::ObjectDatabase::ClearDatabase() {
	SMBC::ObjectDatabase::ObjectDB.clear();
	SMBC::ObjectDatabase::BlockDB.clear();
}

void SMBC::ObjectDatabase::LoadLanguageFile(const std::wstring& path) {
	std::wstring _LangFile = path + L"/inventoryDescriptions.json";
	std::wstring _OtherLangFile = path + L"/InventoryItemDescriptions.json";

	std::wstring _Path = (SMBC::FILE::FileExists(_LangFile) ? _LangFile : SMBC::FILE::FileExists(_OtherLangFile) ? _OtherLangFile : L"");
	if (_Path.empty()) return;

	SMBC::ObjectDatabase::GameTranslations.LoadLanguageFile(_Path);
}

void SMBC::ObjectDatabase::AddObjectToDatabase(SMBC::ObjectData& obj_data) {
	for (SMBC::ObjectData& _data : SMBC::ObjectDatabase::ObjectDB)
		if (_data._obj_uuid == obj_data._obj_uuid) return;

	SMBC::ObjectDatabase::ObjectDB.push_back(obj_data);
}

glm::vec3 SMBC::ObjectDatabase::LoadBlockCollision(const nlohmann::json& collision) {
	glm::vec3 _Collision(1.0f);

	bool _contains_box = (collision.contains("box") && collision.at("box").is_object());
	bool _contains_hull = (collision.contains("hull") && collision.at("hull").is_object());

	if (_contains_box || _contains_hull) {
		auto& _Coll = (_contains_box ? collision.at("box") : collision.at("hull"));

		auto& _XPos = _Coll["x"];
		auto& _YPos = _Coll["y"];
		auto& _ZPos = _Coll["z"];

		if (_XPos.is_number() && _YPos.is_number() && _ZPos.is_number())
			_Collision = glm::vec3(_XPos.get<float>(), _YPos.get<float>(), _ZPos.get<float>());
	}
	else if (collision.contains("cylinder") && collision.at("cylinder").is_object()) {
		auto& _CylColl = collision.at("cylinder");

		auto& _Diameter = _CylColl["diameter"];
		auto& _Depth = _CylColl["depth"];
		auto& _Axis = _CylColl["axis"];

		if (_Diameter.is_number() && _Depth.is_number() && _Axis.is_string()) {
			std::string _AxisString = _Axis.get<std::string>();
			float _FDiameter = _Diameter.get<float>();
			float _FDepth = _Depth.get<float>();

			if (_AxisString == "x" || _AxisString == "X")
				_Collision = glm::vec3(_FDepth, _FDiameter, _FDiameter);
			else if (_AxisString == "y" || _AxisString == "Y")
				_Collision = glm::vec3(_FDiameter, _FDepth, _FDiameter);
			else if (_AxisString == "z" || _AxisString == "Z")
				_Collision = glm::vec3(_FDiameter, _FDiameter, _FDepth);
		}
	}

	return _Collision;
}

bool SMBC::ObjectDatabase::GetBlockByUuid(const std::wstring& uuid, SMBC::BlockData& object) {
	for (SMBC::BlockData& blk_data : SMBC::ObjectDatabase::BlockDB) {
		if (blk_data._obj_uuid == uuid) {
			object = blk_data;
			return true;
		}
	}

	return false;
}

bool SMBC::ObjectDatabase::GetPartByUuid(const std::wstring& uuid, SMBC::ObjectData& object) {
	for (SMBC::ObjectData& part_data : SMBC::ObjectDatabase::ObjectDB) {
		if (part_data._obj_uuid == uuid) {
			object = part_data;
			return true;
		}
	}

	return false;
}

void SMBC::ObjectDatabase::AddBlockToDatabase(SMBC::BlockData& blk_data) {
	for (SMBC::BlockData& b_data : SMBC::ObjectDatabase::BlockDB)
		if (b_data._obj_uuid == blk_data._obj_uuid) return;

	SMBC::ObjectDatabase::BlockDB.push_back(blk_data);
}

void SMBC::ObjectDatabase::GetRenderableData(
	const nlohmann::json& renderable,
	SMBC::Texture::Texture& texture,
	std::wstring& model_path
) {
	if (!renderable.contains("lodList")) return;
	auto _LodList = renderable.at("lodList");

	if (!(_LodList.is_array() && _LodList[0].is_object())) return;
	auto& _LodItem = _LodList[0];
	auto& _Mesh = _LodItem["mesh"];

	if (!_Mesh.is_string()) return;
	std::wstring _WstrMesh = SMBC::PathReplacer::ReplaceKey(SMBC::Other::Utf8ToWide(_Mesh.get<std::string>()));

	if (_WstrMesh.empty()) return;
	model_path = _WstrMesh;

	auto& SubMeshList = _LodItem["subMeshList"];
	auto& SubMeshMap = _LodItem["subMeshMap"];

	if (SubMeshList.is_array()) {
		SMBC::Texture::Texture _Output(SMBC::Texture::TextureType::SubMeshList);

		uint32_t _idx = 0;
		for (auto& SubMesh : SubMeshList) {
			if (!SubMesh.is_object()) continue;

			auto& _TextureList = SubMesh["textureList"];
			if (!_TextureList.is_array()) continue;

			auto& _J_Idx = SubMesh["idx"];
			uint32_t _mesh_idx = (_J_Idx.is_number() ? _J_Idx.get<int>() : _idx);

			std::wstring _DifTex = (_TextureList[0].is_string() ? SMBC::Other::Utf8ToWide(_TextureList[0].get<std::string>()) : L"");
			std::wstring _AsgTex = (_TextureList[1].is_string() ? SMBC::Other::Utf8ToWide(_TextureList[1].get<std::string>()) : L"");
			std::wstring _NorTex = (_TextureList[2].is_string() ? SMBC::Other::Utf8ToWide(_TextureList[2].get<std::string>()) : L"");

			SMBC::Texture::TextureList _NewList(_DifTex, _AsgTex, _NorTex, std::to_wstring(_mesh_idx));
			_NewList.ReplaceTextureKeys();

			_Output.AddTexture(_NewList);
			_idx++;
		}

		texture = _Output;
	}
	else if (SubMeshMap.is_object()) {
		SMBC::Texture::Texture _Output(SMBC::Texture::TextureType::SubMeshMap);

		for (auto& SubMesh : SubMeshMap.items()) {
			if (!SubMesh.value().is_object()) continue;

			auto& _TextureList = SubMesh.value()["textureList"];
			if (!_TextureList.is_array()) continue;

			std::wstring _DifTex = (_TextureList[0].is_string() ? SMBC::Other::Utf8ToWide(_TextureList[0].get<std::string>()) : L"");
			std::wstring _AsgTex = (_TextureList[1].is_string() ? SMBC::Other::Utf8ToWide(_TextureList[1].get<std::string>()) : L"");
			std::wstring _NorTex = (_TextureList[2].is_string() ? SMBC::Other::Utf8ToWide(_TextureList[2].get<std::string>()) : L"");

			std::wstring _WstrKey = SMBC::Other::Utf8ToWide(SubMesh.key());
			SMBC::Texture::TextureList _NewList(_DifTex, _AsgTex, _NorTex, _WstrKey);
			_NewList.ReplaceTextureKeys();

			_Output.AddTexture(_NewList);
		}

		texture = _Output;
	}
}

void SMBC::ObjectDatabase::LoadObjectFile(const std::wstring& path, SMBC::LangDB& translations) {
	nlohmann::json _ObjectFile;
	if (!SMBC::JSON::OpenParseJson(path, _ObjectFile)) return;

	if (_ObjectFile.contains("blockList") && _ObjectFile.at("blockList").is_array()) {
		auto& _BlockList = _ObjectFile.at("blockList");

		SMBC::BlueprintConversionData::ProgressBarMax += (int32_t)_BlockList.size();
		for (auto& _Block : _BlockList) {
			auto& _Uuid = _Block["uuid"];

			if (!_Uuid.is_string()) continue;
			std::wstring _WstrUuid = SMBC::Other::Utf8ToWide(_Uuid.get<std::string>());

			std::wstring _BlockName = translations.GetTranslationString(_WstrUuid);
			if (_BlockName.empty()) _BlockName = L"BLOCK NOT FOUND";
			_BlockName += (L" (" + translations._Environment + L": " + _WstrUuid + L")");

			auto& _Dif = _Block["dif"];
			auto& _Asg = _Block["asg"];
			auto& _Nor = _Block["nor"];
			auto& _Tiling = _Block["tiling"];

			SMBC::Texture::TextureList _TexList;

			_TexList.dif = (_Dif.is_string() ? SMBC::Other::Utf8ToWide(_Dif.get<std::string>()) : L"");
			_TexList.asg = (_Asg.is_string() ? SMBC::Other::Utf8ToWide(_Asg.get<std::string>()) : L"");
			_TexList.nor = (_Nor.is_string() ? SMBC::Other::Utf8ToWide(_Nor.get<std::string>()) : L"");
			_TexList.ReplaceTextureKeys();

			int _TilingValue = (_Tiling.is_number() ? _Tiling.get<int>() : 4);
			if (_TilingValue > 16 || _TilingValue <= 0) _TilingValue = 4;

			SMBC::BlockData _BlockData(_TexList, _WstrUuid, _BlockName, _TilingValue);

			SMBC::ObjectDatabase::AddBlockToDatabase(_BlockData);
			SMBC::BlueprintConversionData::ProgressBarValue++;
		}
	}

	if (_ObjectFile.contains("partList") && _ObjectFile.at("partList").is_array()) {
		auto& _PartList = _ObjectFile.at("partList");

		SMBC::BlueprintConversionData::ProgressBarMax += (int32_t)_PartList.size();
		for (auto& _Part : _PartList) {
			auto& _Uuid = _Part["uuid"];
			if (!_Uuid.is_string()) continue;
			std::wstring _WstrUuid = SMBC::Other::Utf8ToWide(_Uuid.get<std::string>());

			std::wstring _ObjName = translations.GetTranslationString(_WstrUuid);
			if (_ObjName.empty()) _ObjName = L"BLOCK NOT FOUND";
			_ObjName += (L" (" + translations._Environment + L": " + _WstrUuid + L")");

			glm::vec3 _ObjectBounds = SMBC::ObjectDatabase::LoadBlockCollision(_Part);
			auto& _Renderable = _Part["renderable"];

			if (_Renderable.is_object()) {
				std::wstring _Mesh = L"";
				SMBC::Texture::Texture _TextureList(SMBC::Texture::TextureType::None);

				SMBC::ObjectDatabase::GetRenderableData(_Renderable, _TextureList, _Mesh);

				if (_Mesh.empty() || _TextureList.TexType == SMBC::Texture::TextureType::None) continue;

				SMBC::ObjectData _ObjData(_WstrUuid, _Mesh, _ObjName, _TextureList, _ObjectBounds);
				SMBC::ObjectDatabase::AddObjectToDatabase(_ObjData);
				SMBC::BlueprintConversionData::ProgressBarValue++;
			}
			else if (_Renderable.is_string()) {
				std::wstring _RenderableWstr = SMBC::Other::Utf8ToWide(_Renderable.get<std::string>());
				_RenderableWstr = SMBC::PathReplacer::ReplaceKey(_RenderableWstr);
				nlohmann::json _RendFile;

				if (!SMBC::JSON::OpenParseJson(_RenderableWstr, _RendFile)) continue;

				std::wstring _Mesh = L"";
				SMBC::Texture::Texture _TextureList(SMBC::Texture::TextureType::None);

				SMBC::ObjectDatabase::GetRenderableData(_RendFile, _TextureList, _Mesh);

				if (_Mesh.empty() || _TextureList.TexType == SMBC::Texture::TextureType::None) continue;

				SMBC::ObjectData _ObjData(_WstrUuid, _Mesh, _ObjName, _TextureList, _ObjectBounds);
				SMBC::ObjectDatabase::AddObjectToDatabase(_ObjData);
				SMBC::BlueprintConversionData::ProgressBarValue++;
			}
		}
	}
}

void SMBC::ObjectDatabase::LoadGameDatabase() {
	for (std::wstring& data_path : SMBC::Settings::SMDirDatabase) {
		if (!SMBC::FILE::FileExists(data_path)) continue;

		fs::directory_entry CurPath(data_path);
		if (CurPath.is_directory()) {
			fs::directory_iterator _DifIter(CurPath.path().wstring(), fs::directory_options::skip_permission_denied);
			for (auto& dir : _DifIter) {
				if (!dir.is_regular_file()) continue;

				SMBC::ObjectDatabase::LoadObjectFile(dir.path().wstring(), SMBC::ObjectDatabase::GameTranslations);
			}
		}
		else if (CurPath.is_regular_file())
			SMBC::ObjectDatabase::LoadObjectFile(CurPath.path().wstring(), SMBC::ObjectDatabase::GameTranslations);
	}
}

void SMBC::ObjectDatabase::LoadModDatabase() {
	if (SMBC::Settings::ModFolders.empty()) return;

	for (std::wstring& _ModDirectory : SMBC::Settings::ModFolders) {
		if (!SMBC::FILE::FileExists(_ModDirectory)) continue;

		fs::directory_iterator _DirIter(_ModDirectory, fs::directory_options::skip_permission_denied);
		for (auto& dir : _DirIter) {
			if (!dir.is_directory()) continue;

			std::wstring _ModDescription = dir.path().wstring() + L"/description.json";
			nlohmann::json _DescJson;

			if (!SMBC::JSON::OpenParseJson(_ModDescription, _DescJson, true)) continue;

			std::wstring _ModType = SMBC::JSON::GetJsonWstr(_DescJson, "type");
			if (_ModType != L"Blocks and Parts") continue;

			std::wstring _ModUuid = SMBC::JSON::GetJsonWstr(_DescJson, "localId");
			if (_ModUuid.empty()) continue;

			std::wstring _ObjectsPath = dir.path().wstring() + L"/Objects/Database/ShapeSets";
			if (!SMBC::FILE::FileExists(_ObjectsPath)) continue;

			SMBC::PathReplacer::SetContentKey(_ModUuid);
			SMBC::PathReplacer::SetModKey(dir.path().wstring());

			std::wstring _LanguageDBPath = dir.path().wstring() + L"/Gui/Language/English/inventoryDescriptions.json";

			std::wstring _ModName = SMBC::JSON::GetJsonWstr(_DescJson, "name");
			if (_ModName.empty()) _ModName = L"NO MOD NAME";

			SMBC::LangDB _LangDB(_ModName);
			_LangDB.LoadLanguageFile(_LanguageDBPath);

			fs::recursive_directory_iterator RecDirIter(_ObjectsPath, fs::directory_options::skip_permission_denied);
			for (auto& m_dir : RecDirIter) {
				if (!m_dir.is_regular_file() || !m_dir.path().has_extension() || m_dir.path().extension() != ".json") continue;

				SMBC::ObjectDatabase::LoadObjectFile(m_dir.path().wstring(), _LangDB);
			}
		}
	}
}