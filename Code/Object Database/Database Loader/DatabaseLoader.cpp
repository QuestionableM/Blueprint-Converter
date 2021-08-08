#include "DatabaseLoader.h"

#include "Object Database/Mod Data/ModData.h"
#include "Object Database/ObjectDatabase.h"
#include "Object Database/Keyword Replacer/KeywordReplacer.h"

#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/Functions/Functions.h"
#include "Lib/File/FileFunc.h"
#include "Lib/ProgramSettings.h"

#include <filesystem>

#include "DebugCon.h"

namespace fs = std::filesystem;

glm::vec3 SMBC::DatabaseLoader::LoadBlockCollision(const nlohmann::json& collision) {
	glm::vec3 _Collision(1.0f);

	bool isBoxCol = collision.contains("box");
	bool isHullCol = collision.contains("hull");

	if (isBoxCol || isHullCol) {
		std::string colKey = (isBoxCol ? "box" : "hull");
		const auto& _Coll = collision.at(colKey);

		if (_Coll.is_object()) {
			const auto& _XPos = SMBC::Json::Get(_Coll, "x");
			const auto& _YPos = SMBC::Json::Get(_Coll, "y");
			const auto& _ZPos = SMBC::Json::Get(_Coll, "z");

			if (_XPos.is_number() && _YPos.is_number() && _ZPos.is_number())
				_Collision = glm::vec3(_XPos.get<float>(), _YPos.get<float>(), _ZPos.get<float>());
		}
	}
	else {
		const auto& _CylCol = SMBC::Json::Get(collision, "cylinder");
		if (_CylCol.is_object()) {
			const auto& _Diameter = SMBC::Json::Get(_CylCol, "diameter");
			const auto& _Depth = SMBC::Json::Get(_CylCol, "depth");
			const auto& _Axis = SMBC::Json::Get(_CylCol, "axis");

			std::string _AxisString = (_Axis.is_string() ? _Axis.get<std::string>() : "z");

			if (_Diameter.is_number() && _Depth.is_number()) {
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
	const auto& _LodList = SMBC::Json::Get(renderable, "lodList");
	if (!_LodList.is_array()) return;

	const auto& _LodItem = SMBC::Json::Get(_LodList, 0);
	if (!_LodItem.is_object()) return;

	const auto& _Mesh = SMBC::Json::Get(_LodItem, "mesh");
	if (!_Mesh.is_string()) return;
	std::wstring _WstrMesh = SMBC::PathReplacer::ReplaceKey(SMBC::Other::Utf8ToWide(_Mesh.get<std::string>()));

	if (_WstrMesh.empty()) return;
	model_path = _WstrMesh;

	const auto& SubMeshList = SMBC::Json::Get(_LodItem, "subMeshList");
	if (SubMeshList.is_array()) {
		SMBC::Texture::Texture _Output(SMBC::Texture::TextureType::SubMeshList);

		uint32_t _idx = 0;
		for (auto& SubMesh : SubMeshList) {
			if (!SubMesh.is_object()) continue;

			const auto& _TextureList = SMBC::Json::Get(SubMesh, "textureList");
			if (!_TextureList.is_array()) continue;

			const auto& idx_obj = SMBC::Json::Get(SubMesh, "idx");
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
	else {
		const auto& SubMeshMap = SMBC::Json::Get(_LodItem, "subMeshMap");
		if (SubMeshMap.is_object()) {
			SMBC::Texture::Texture _Output(SMBC::Texture::TextureType::SubMeshMap);

			for (auto& SubMesh : SubMeshMap.items()) {
				if (!SubMesh.value().is_object()) continue;

				const auto& _TextureList = SMBC::Json::Get(SubMesh.value(), "textureList");
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

void SMBC::DatabaseLoader::LoadObjectFile(
	SMBC::ModData* mod,
	const std::wstring& path,
	SMBC::LangDB& translations
) {
	nlohmann::json _ObjectFile;

	if (!SMBC::Json::ParseJson(path, _ObjectFile))
		return;

	const auto& _BlockList = SMBC::Json::Get(_ObjectFile, "blockList");
	if (_BlockList.is_array()) {
		SMBC::BPConvData::ProgressBarMax += (int32_t)_BlockList.size();
		for (auto& _Block : _BlockList) {
			const auto& _Uuid = SMBC::Json::Get(_Block, "uuid");

			if (!_Uuid.is_string()) continue;
			std::wstring _WstrUuid = SMBC::Other::Utf8ToWide(_Uuid.get<std::string>());
			if (mod->UuidExists(_WstrUuid)) continue;

			const std::wstring& _BlockName = translations.GetTranslation(_WstrUuid);

			const auto& _Dif = SMBC::Json::Get(_Block, "dif");
			const auto& _Asg = SMBC::Json::Get(_Block, "asg");
			const auto& _Nor = SMBC::Json::Get(_Block, "nor");
			const auto& _Tiling = SMBC::Json::Get(_Block, "tiling");

			SMBC::Texture::TextureList _TexList;

			_TexList.dif = (_Dif.is_string() ? SMBC::Other::Utf8ToWide(_Dif.get<std::string>()) : L"");
			_TexList.asg = (_Asg.is_string() ? SMBC::Other::Utf8ToWide(_Asg.get<std::string>()) : L"");
			_TexList.nor = (_Nor.is_string() ? SMBC::Other::Utf8ToWide(_Nor.get<std::string>()) : L"");
			_TexList.ReplaceTextureKeys();

			int _TilingValue = (_Tiling.is_number() ? _Tiling.get<int>() : 4);
			if (_TilingValue > 16 || _TilingValue <= 0) _TilingValue = 4;

			SMBC::BlockData* _BlockData = new SMBC::BlockData(
				_TexList,
				_WstrUuid,
				_BlockName + (L" (" + translations._Environment + L": " + _WstrUuid + L")"),
				_TilingValue
			);
			mod->AddBlockToDatabase(_BlockData);

			SMBC::BPConvData::ProgressBarValue++;
		}
	}


	const auto& _PartList = SMBC::Json::Get(_ObjectFile, "partList");
	if (_PartList.is_array()) {
		SMBC::BPConvData::ProgressBarMax += (int32_t)_PartList.size();
		for (auto& _Part : _PartList) {
			const auto& _Uuid = SMBC::Json::Get(_Part, "uuid");
			if (!_Uuid.is_string()) continue;

			std::wstring _WstrUuid = SMBC::Other::Utf8ToWide(_Uuid.get<std::string>());
			if (mod->UuidExists(_WstrUuid)) continue;

			const std::wstring& _ObjName = translations.GetTranslation(_WstrUuid);

			glm::vec3 _ObjectBounds = SMBC::DatabaseLoader::LoadBlockCollision(_Part);
			const auto& _Renderable = SMBC::Json::Get(_Part, "renderable");

			if (_Renderable.is_object()) {
				std::wstring _Mesh = L"";
				SMBC::Texture::Texture _TextureList(SMBC::Texture::TextureType::None);

				SMBC::DatabaseLoader::GetRenderableData(_Renderable, _TextureList, _Mesh);
				if (_Mesh.empty() || _TextureList.TexType == SMBC::Texture::TextureType::None) continue;

				SMBC::ObjectData* _ObjData = new SMBC::ObjectData(
					_WstrUuid,
					_Mesh,
					_ObjName + (L" (" + translations._Environment + L": " + _WstrUuid + L")"),
					_TextureList,
					_ObjectBounds
				);
				mod->AddPartToDatabase(_ObjData);

				SMBC::BPConvData::ProgressBarValue++;
			}
			else if (_Renderable.is_string()) {
				std::wstring _RenderableWstr = SMBC::Other::Utf8ToWide(_Renderable.get<std::string>());
				_RenderableWstr = SMBC::PathReplacer::ReplaceKey(_RenderableWstr);

				nlohmann::json _RendFile;
				if (!SMBC::Json::ParseJson(_RenderableWstr, _RendFile))
					continue;

				std::wstring _Mesh = L"";
				SMBC::Texture::Texture _TextureList(SMBC::Texture::TextureType::None);

				SMBC::DatabaseLoader::GetRenderableData(_RendFile, _TextureList, _Mesh);
				if (_Mesh.empty() || _TextureList.TexType == SMBC::Texture::TextureType::None) continue;

				SMBC::ObjectData* _ObjData = new SMBC::ObjectData(
					_WstrUuid,
					_Mesh,
					_ObjName + (L" (" + translations._Environment + L": " + _WstrUuid + L")"),
					_TextureList,
					_ObjectBounds
				);

				mod->AddPartToDatabase(_ObjData);

				SMBC::BPConvData::ProgressBarValue++;
			}
		}
	}
}

void SMBC::DatabaseLoader::LoadGameDatabase() {
	SMBC::ModData* _VanillaParts = new SMBC::ModData(
		L"VANILLA GAME",
		L"",
		SMBC::Settings::PathToSM
	);

	for (std::wstring& path : SMBC::Settings::VanillaLanguagePaths)
		_VanillaParts->LoadTranslations(path);

	for (std::wstring& data_path : SMBC::Settings::SMDirDatabase) {
		if (!SMBC::File::FileExists(data_path)) continue;

		fs::directory_entry CurPath(data_path);
		if (CurPath.is_directory()) {
			fs::directory_iterator _DifIter(CurPath.path().wstring(), fs::directory_options::skip_permission_denied);
			for (auto& dir : _DifIter) {
				if (!dir.is_regular_file()) continue;

				SMBC::DatabaseLoader::LoadObjectFile(_VanillaParts, dir.path().wstring(), _VanillaParts->LanguageDB);
			}
		}
		else if (CurPath.is_regular_file())
			SMBC::DatabaseLoader::LoadObjectFile(_VanillaParts, CurPath.path().wstring(), _VanillaParts->LanguageDB);
	}

	SMBC::ObjectDatabase::ModDB.insert(std::make_pair(
		L"00000000-0000-0000-0000-000000000000",
		_VanillaParts
	));
}

void SMBC::DatabaseLoader::LoadModDatabase() {
	if (SMBC::Settings::ModFolders.empty()) return;

	for (std::wstring& _ModDirectory : SMBC::Settings::ModFolders) {
		if (!SMBC::File::FileExists(_ModDirectory)) continue;

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