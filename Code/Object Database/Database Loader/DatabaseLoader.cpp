#include "DatabaseLoader.h"

#include "Object Database/Mod Data/ModData.h"
#include "Object Database/ObjectDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/Functions/Functions.h"
#include "Lib/File/FileFunc.h"
#include "Lib/ProgramSettings.h"

#include <filesystem>


namespace fs = std::filesystem;

glm::vec3 SMBC::DatabaseLoader::LoadBlockCollision(const nlohmann::json& collision) {
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

void SMBC::DatabaseLoader::GetTextureArray(
	const nlohmann::json& textureList,
	std::wstring& dif,
	std::wstring& asg,
	std::wstring& nor
) {
	size_t sz_list = textureList.size();
	size_t arr_size = (sz_list > 3 ? 3 : sz_list);

	for (size_t a = 0; a < arr_size; a++) {
		auto& texObj = textureList[a];

		std::wstring texStr = (texObj.is_string() ? SMBC::Other::Utf8ToWide(texObj.get<std::string>()) : L"");
		switch (a) {
		case 0: dif = texStr; break;
		case 1: asg = texStr; break;
		case 2: nor = texStr; break;
		}
	}
}

void SMBC::DatabaseLoader::GetRenderableData(
	const nlohmann::json& renderable,
	SMBC::Texture::Texture& texture,
	std::wstring& model_path
) {
	auto _LodList = renderable["lodList"];
	if (!_LodList.is_array()) return;

	auto& _LodItem = _LodList[0];
	if (!_LodItem.is_object()) return;

	auto& _Mesh = _LodItem["mesh"];
	if (!_Mesh.is_string()) return;
	std::wstring _WstrMesh = SMBC::PathReplacer::ReplaceKey(SMBC::Other::Utf8ToWide(_Mesh.get<std::string>()));

	if (_WstrMesh.empty()) return;
	model_path = _WstrMesh;

	if (_LodItem.contains("subMeshList")) {
		auto& SubMeshList = _LodItem["subMeshList"];
		if (SubMeshList.is_array()) {
			SMBC::Texture::Texture _Output(SMBC::Texture::TextureType::SubMeshList);

			uint32_t _idx = 0;
			for (auto& SubMesh : SubMeshList) {
				if (!SubMesh.is_object()) continue;

				auto& _TextureList = SubMesh["textureList"];
				if (!_TextureList.is_array()) continue;

				auto& idx_obj = SubMesh["idx"];
				uint32_t _mesh_idx = (idx_obj.is_number() ? idx_obj.get<int>() : _idx);

				std::wstring _DifTex, _AsgTex, _NorTex;
				SMBC::DatabaseLoader::GetTextureArray(_TextureList, _DifTex, _AsgTex, _NorTex);

				SMBC::Texture::TextureList _NewList(_DifTex, _AsgTex, _NorTex, std::to_wstring(_mesh_idx));
				_NewList.ReplaceTextureKeys();

				_Output.AddTexture(_NewList);
				_idx++;
			}

			texture = _Output;
		}
	}
	else if (_LodItem.contains("subMeshMap")) {
		auto& SubMeshMap = _LodItem["subMeshMap"];
		if (SubMeshMap.is_object()) {
			SMBC::Texture::Texture _Output(SMBC::Texture::TextureType::SubMeshMap);

			for (auto& SubMesh : SubMeshMap.items()) {
				if (!SubMesh.value().is_object()) continue;

				auto& _TextureList = SubMesh.value()["textureList"];
				if (!_TextureList.is_array()) continue;

				std::wstring _DifTex, _AsgTex, _NorTex;
				SMBC::DatabaseLoader::GetTextureArray(_TextureList, _DifTex, _AsgTex, _NorTex);

				std::wstring _WstrKey = SMBC::Other::Utf8ToWide(SubMesh.key());
				SMBC::Texture::TextureList _NewList(_DifTex, _AsgTex, _NorTex, _WstrKey);
				_NewList.ReplaceTextureKeys();

				_Output.AddTexture(_NewList);
			}

			texture = _Output;
		}
	}
}

bool SMBC::DatabaseLoader::ModUuidExists(const std::wstring& uuid) {
	for (SMBC::ModData& mod : SMBC::ObjectDatabase::ModDB)
		if (mod.uuid == uuid) return true;

	return false;
}

void SMBC::DatabaseLoader::LoadObjectFile(
	SMBC::ModData& mod,
	const std::wstring& path,
	SMBC::LangDB& translations
) {
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

			mod.AddBlockToDatabase(_BlockData);
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
			_ObjName += (L" (" + translations._Environment + L": " + _WstrUuid + L")");

			glm::vec3 _ObjectBounds = SMBC::DatabaseLoader::LoadBlockCollision(_Part);
			auto& _Renderable = _Part["renderable"];

			if (_Renderable.is_object()) {
				std::wstring _Mesh = L"";
				SMBC::Texture::Texture _TextureList(SMBC::Texture::TextureType::None);

				SMBC::DatabaseLoader::GetRenderableData(_Renderable, _TextureList, _Mesh);

				if (_Mesh.empty() || _TextureList.TexType == SMBC::Texture::TextureType::None) continue;

				SMBC::ObjectData _ObjData(_WstrUuid, _Mesh, _ObjName, _TextureList, _ObjectBounds);
				mod.AddPartToDatabase(_ObjData);
				SMBC::BlueprintConversionData::ProgressBarValue++;
			}
			else if (_Renderable.is_string()) {
				std::wstring _RenderableWstr = SMBC::Other::Utf8ToWide(_Renderable.get<std::string>());
				_RenderableWstr = SMBC::PathReplacer::ReplaceKey(_RenderableWstr);
				nlohmann::json _RendFile;

				if (!SMBC::JSON::OpenParseJson(_RenderableWstr, _RendFile)) continue;

				std::wstring _Mesh = L"";
				SMBC::Texture::Texture _TextureList(SMBC::Texture::TextureType::None);

				SMBC::DatabaseLoader::GetRenderableData(_RendFile, _TextureList, _Mesh);

				if (_Mesh.empty() || _TextureList.TexType == SMBC::Texture::TextureType::None) continue;

				SMBC::ObjectData _ObjData(_WstrUuid, _Mesh, _ObjName, _TextureList, _ObjectBounds);
				mod.AddPartToDatabase(_ObjData);
				SMBC::BlueprintConversionData::ProgressBarValue++;
			}
		}
	}
}

void SMBC::DatabaseLoader::LoadGameDatabase() {
	SMBC::ModData _VanillaParts(
		L"00000000-0000-0000-0000-000000000000",
		L"VANILLA GAME",
		L"",
		SMBC::Settings::PathToSM
	);

	for (std::wstring& path : SMBC::Settings::VanillaLanguagePaths)
		_VanillaParts.LoadTranslations(path);

	for (std::wstring& data_path : SMBC::Settings::SMDirDatabase) {
		if (!SMBC::FILE::FileExists(data_path)) continue;

		fs::directory_entry CurPath(data_path);
		if (CurPath.is_directory()) {
			fs::directory_iterator _DifIter(CurPath.path().wstring(), fs::directory_options::skip_permission_denied);
			for (auto& dir : _DifIter) {
				if (!dir.is_regular_file()) continue;

				SMBC::DatabaseLoader::LoadObjectFile(_VanillaParts, dir.path().wstring(), _VanillaParts.LanguageDB);
			}
		}
		else if (CurPath.is_regular_file())
			SMBC::DatabaseLoader::LoadObjectFile(_VanillaParts, CurPath.path().wstring(), _VanillaParts.LanguageDB);
	}

	SMBC::ObjectDatabase::ModDB.push_back(_VanillaParts);
}

void SMBC::DatabaseLoader::LoadModDatabase() {
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
			if (_ModUuid.empty() || SMBC::DatabaseLoader::ModUuidExists(_ModUuid)) continue;

			std::wstring _ModName = SMBC::JSON::GetJsonWstr(_DescJson, "name");
			if (_ModName.empty()) continue;

			long long _FileId = 0;
			if (_DescJson.contains("fileId")) {
				auto& _FileIdJson = _DescJson.at("fileId");

				if (_FileIdJson.is_number())
					_FileId = _FileIdJson.get<long long>();
			}

			SMBC::ModData NewMod(
				_ModUuid,
				_ModName,
				(_FileId > 0) ? std::to_wstring(_FileId) : L"",
				dir.path().wstring()
			);

			NewMod.LoadTranslations();
			NewMod.LoadObjects();

			SMBC::ObjectDatabase::ModDB.push_back(NewMod);
		}
	}
}