#include <filesystem>
#include <fstream>

#include "ObjectDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/Functions/Functions.h"
#include "Lib/File/FileFunc.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/Json/JsonFunc.h"

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

std::wstring SMBC::ObjectDatabase::_sm_path = L"";
std::vector<std::wstring> SMBC::ObjectDatabase::_mods_path = {};
std::vector<std::wstring> SMBC::ObjectDatabase::_sm_dir_database = {};
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
	nlohmann::json* _ObjectFile = SMBC::JSON::OpenParseJson(path);

	if (_ObjectFile && _ObjectFile->contains("blockList") && _ObjectFile->at("blockList").is_array()) {
		auto& _BlockList = _ObjectFile->at("blockList");

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

	if (_ObjectFile && _ObjectFile->contains("partList") && _ObjectFile->at("partList").is_array()) {
		auto& _PartList = _ObjectFile->at("partList");

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

				nlohmann::json* _RendFile = SMBC::JSON::OpenParseJson(_RenderableWstr);
				if (_RendFile) {
					std::wstring _Mesh = L"";
					SMBC::Texture::Texture _TextureList(SMBC::Texture::TextureType::None);

					SMBC::ObjectDatabase::GetRenderableData(*_RendFile, _TextureList, _Mesh);

					if (_Mesh.empty() || _TextureList.TexType == SMBC::Texture::TextureType::None) continue;

					SMBC::ObjectData _ObjData(_WstrUuid, _Mesh, _ObjName, _TextureList, _ObjectBounds);
					SMBC::ObjectDatabase::AddObjectToDatabase(_ObjData);
					SMBC::BlueprintConversionData::ProgressBarValue++;
				}

				delete _RendFile;
			}
		}
	}

	delete _ObjectFile;
}

void SMBC::ObjectDatabase::LoadGameDatabase() {
	for (std::wstring& data_path : SMBC::ObjectDatabase::_sm_dir_database) {
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
	if (SMBC::ObjectDatabase::_mods_path.size() <= 0) return;

	for (std::wstring& _ModDirectory : SMBC::ObjectDatabase::_mods_path) {
		if (!SMBC::FILE::FileExists(_ModDirectory)) continue;

		fs::directory_iterator _DirIter(_ModDirectory, fs::directory_options::skip_permission_denied);
		for (auto& dir : _DirIter) {
			if (!dir.is_directory()) continue;

			std::wstring _ModDescription = dir.path().wstring() + L"/description.json";
			nlohmann::json* _DescJson = SMBC::JSON::OpenParseJson(_ModDescription, true);
			if (_DescJson) {
				std::wstring _ModType = SMBC::JSON::GetJsonWstrA(_DescJson, "type");
				if (_ModType.empty() || _ModType != L"Blocks and Parts") continue;

				std::wstring _ModUuid = SMBC::JSON::GetJsonWstrA(_DescJson, "localId");
				if (_ModUuid.empty()) continue;

				std::wstring _ObjectsPath = dir.path().wstring() + L"/Objects/Database/ShapeSets";
				if (!SMBC::FILE::FileExists(_ObjectsPath)) continue;

				SMBC::PathReplacer::SetContentKey(_ModUuid);
				SMBC::PathReplacer::SetModKey(dir.path().wstring());

				std::wstring _LanguageDBPath = dir.path().wstring() + L"/Gui/Language/English/inventoryDescriptions.json";

				std::wstring _ModName = SMBC::JSON::GetJsonWstrA(_DescJson, "name", L"NO MOD NAME");
				SMBC::LangDB _LangDB(_ModName);
				_LangDB.LoadLanguageFile(_LanguageDBPath);

				fs::recursive_directory_iterator RecDirIter(_ObjectsPath, fs::directory_options::skip_permission_denied);
				for (auto& m_dir : RecDirIter) {
					if (!m_dir.is_regular_file() || !m_dir.path().has_extension() || m_dir.path().extension() != ".json") continue;

					SMBC::ObjectDatabase::LoadObjectFile(m_dir.path().wstring(), _LangDB);
				}
			}

			delete _DescJson;
		}
	}
}

void SMBC::ObjectDatabase::LoadConfig(const std::wstring& path) {
	nlohmann::json* _ConfigFile = SMBC::JSON::OpenParseJson(path);

	if (_ConfigFile) {
		auto& _ConfigData = *_ConfigFile;

		auto& _UserSettings = _ConfigData["UserSettings"];
		if (_UserSettings.is_object()) {
			auto& _SMPath = _UserSettings["ScrapPath"];

			SMBC::ObjectDatabase::LoadJsonWstrArray(_UserSettings, "BlueprintPaths", SMBC::Blueprint::BlueprintPaths);
			SMBC::ObjectDatabase::LoadJsonWstrArray(_UserSettings, "ScrapModsPath", SMBC::ObjectDatabase::_mods_path);

			if (_SMPath.is_string())
				SMBC::ObjectDatabase::_sm_path = SMBC::Other::Utf8ToWide(_SMPath.get<std::string>());
		}

		SMBC::ObjectDatabase::AddRegistryPathAndSave();
		if (!SMBC::ObjectDatabase::_sm_path.empty())
			SMBC::PathReplacer::AddKeyReplacement(SMBC::KeyReplacement(L"$GAME_FOLDER", SMBC::ObjectDatabase::_sm_path));

		auto& _ProgramSettings = _ConfigData["ProgramSettings"];
		if (_ProgramSettings.is_object()) {
			auto& _KeyWords = _ProgramSettings["Keywords"];

			if (_KeyWords.is_object()) {
				for (auto& keyword : _KeyWords.items()) {
					if (!keyword.value().is_string()) continue;

					std::wstring _WstrKey = SMBC::Other::Utf8ToWide(keyword.key());
					std::wstring _WstrRepl = SMBC::Other::Utf8ToWide(keyword.value().get<std::string>());
					_WstrRepl = SMBC::PathReplacer::ReplaceKey(_WstrRepl);

					SMBC::PathReplacer::AddKeyReplacement(SMBC::KeyReplacement(_WstrKey, _WstrRepl));
				}
			}

			SMBC::ObjectDatabase::LoadJsonWstrArray(_ProgramSettings, "ScrapObjectDatabase", SMBC::ObjectDatabase::_sm_dir_database);

			std::vector<std::wstring> _LanguageArray = {};
			SMBC::ObjectDatabase::LoadJsonWstrArray(_ProgramSettings, "LanguageDirectories", _LanguageArray);

			for (std::wstring& _str : _LanguageArray)
				SMBC::ObjectDatabase::LoadLanguageFile(_str);
		}
	}

	delete _ConfigFile;
}

void SMBC::ObjectDatabase::LoadJsonWstrArray(const nlohmann::json& file, const std::string& keyword, std::vector<std::wstring>& _array) {
	if (!file.contains(keyword)) return;

	auto& _JsonArray = file.at(keyword);
	if (!_JsonArray.is_array()) return;

	for (auto& _wstr : _JsonArray) {
		if (!_wstr.is_string()) continue;

		std::wstring _WstrPath = SMBC::Other::Utf8ToWide(_wstr.get<std::string>());
		_WstrPath = SMBC::PathReplacer::ReplaceKey(_WstrPath);

		_array.push_back(_WstrPath);
	}
}

void SMBC::ObjectDatabase::SaveConfigFile(const bool sm_path, const bool bp_list, const bool mod_list) {
	nlohmann::json _JsonOutput = SMBC::JSON::OpenParseJsonA(L"./Resources/Config.json");

	std::filesystem::create_directory(L"./Resources");
	std::ofstream _ConfigJson("./Resources/Config.json");

	if (!_ConfigJson.is_open()) return;

	if (_JsonOutput.contains("UserSettings") && !_JsonOutput.at("UserSettings").is_object())
		_JsonOutput["UserSettings"] = {};

	if (sm_path) {
		_JsonOutput["UserSettings"]["ScrapPath"] = SMBC::Other::WideToUtf8(SMBC::ObjectDatabase::_sm_path);
	}

	if (bp_list) {
		nlohmann::json _BlueprintList = {};

		for (std::wstring& _bp_obj : SMBC::Blueprint::BlueprintPaths)
			_BlueprintList.push_back(SMBC::Other::WideToUtf8(_bp_obj));

		_JsonOutput["UserSettings"]["BlueprintPaths"] = _BlueprintList;
	}

	if (mod_list) {
		nlohmann::json _ModList = {};

		for (std::wstring& _mod_obj : SMBC::ObjectDatabase::_mods_path)
			_ModList.push_back(SMBC::Other::WideToUtf8(_mod_obj));

		_JsonOutput["UserSettings"]["ScrapModsPath"] = _ModList;
	}

	_ConfigJson << std::setw(4) << _JsonOutput;

	_ConfigJson.close();
}

void SMBC::ObjectDatabase::AddArrayPath(const std::wstring& element, std::vector<std::wstring>& _array) {
	for (std::wstring& _wstr_e : _array) {
		if (_wstr_e == element || SMBC::FILE::IsEquivalent(_wstr_e, element))
			return;
	}

	_array.push_back(element);
}

void SMBC::ObjectDatabase::AddRegistryPathAndSave() {
	if (SMBC::ObjectDatabase::_sm_path.empty()) {
		std::wstring _Path = SMBC::Other::ReadRegistryKey(L"SOFTWARE\\Valve\\Steam", L"SteamPath");

		if (!_Path.empty() && SMBC::FILE::FileExists(_Path)) {
			std::wstring _ScrapPath = _Path + L"/steamapps/common/scrap mechanic";
			std::wstring _ScrapWorkshop = _Path + L"/steamapps/workshop/content/387990";

			if (SMBC::FILE::FileExists(_ScrapPath)) SMBC::ObjectDatabase::_sm_path = _ScrapPath;
			if (SMBC::FILE::FileExists(_ScrapWorkshop)) {
				SMBC::ObjectDatabase::AddArrayPath(_ScrapWorkshop, SMBC::ObjectDatabase::_mods_path);
				SMBC::ObjectDatabase::AddArrayPath(_ScrapWorkshop, SMBC::Blueprint::BlueprintPaths);
			}

			SMBC::ObjectDatabase::SaveConfigFile(true, true, true);
		}
	}
}