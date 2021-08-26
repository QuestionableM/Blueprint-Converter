#include "Blueprint Converter/Cache Manager/CacheManager.h"
#include "Lib/String/String.h"
#include "Lib/File/FileFunc.h"

#include <fstream>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <nlohmann/json.hpp>

#include "DebugCon.h"

typedef SMBC::ModelStorage _ModelStorage;

std::unordered_map<std::wstring, SMBC::Model*> _ModelStorage::CachedModels = {};
Assimp::Importer _ModelStorage::Importer = Assimp::Importer();

SMBC::Model* _ModelStorage::LoadModel(
	const std::wstring& path,
	const bool& load_uvs,
	const bool& load_normals
) {
	if (_ModelStorage::CachedModels.find(path) != _ModelStorage::CachedModels.end())
		return _ModelStorage::CachedModels.at(path);

	DebugOut("[Model] Loading: ", path, "\n");

	const aiScene* ModelScene = _ModelStorage::Importer.ReadFile(
		SMBC::String::ToUtf8(path).c_str(),
		aiProcess_FindInvalidData |
		aiProcess_RemoveComponent |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FindDegenerates |
		aiProcess_OptimizeMeshes
	);

	if (ModelScene && ModelScene->HasMeshes()) {
		SMBC::Model* newModel = new SMBC::Model();

		long long FaceOffset = 0ll;
		long long TexturePointOffset = 0ll;
		long long NormalOffset = 0ll;

		bool _HasMaterials = ModelScene->HasMaterials();

		newModel->subMeshData.reserve(ModelScene->mNumMeshes);
		for (uint32_t a = 0u; a < ModelScene->mNumMeshes; a++) {
			FaceOffset = (long long)newModel->vertices.size();
			TexturePointOffset = (long long)newModel->uvs.size();
			NormalOffset = (long long)newModel->normals.size();

			const aiMesh* Mesh = ModelScene->mMeshes[a];

			bool _HasTextureCoords = (load_uvs && Mesh->mTextureCoords[0] != NULL);
			bool _HasNormals = (load_normals && Mesh->HasNormals());

			if (_HasTextureCoords) newModel->uvs.reserve(Mesh->mNumVertices);
			if (_HasNormals) newModel->normals.reserve(Mesh->mNumVertices);

			newModel->vertices.reserve(Mesh->mNumVertices);
			for (uint32_t b = 0u; b < Mesh->mNumVertices; b++) {
				aiVector3D& aiVert = Mesh->mVertices[b];
				newModel->vertices.push_back({ aiVert.x, aiVert.y, aiVert.z });

				if (!_HasTextureCoords && !_HasNormals) continue;

				if (_HasTextureCoords) {
					aiVector3D& Uv = Mesh->mTextureCoords[0][b];
					newModel->uvs.push_back({ Uv.x, Uv.y });
				}

				if (_HasNormals) {
					aiVector3D& Normal = Mesh->mNormals[b];
					newModel->normals.push_back({ Normal.x, Normal.y, Normal.z });
				}
			}

			SMBC::SubMeshData* subMeshData = new SMBC::SubMeshData();
			subMeshData->SubMeshIndex = a;
			subMeshData->MaterialName = L"";

			if (_HasMaterials) {
				const aiMaterial* MeshMat = ModelScene->mMaterials[Mesh->mMaterialIndex];
				aiString MatName;
				MeshMat->Get(AI_MATKEY_NAME, MatName);

				std::string StrMatName = std::string(MatName.C_Str(), MatName.length);
				subMeshData->MaterialName = SMBC::String::ToWide(StrMatName);
			}

			subMeshData->DataIdx.reserve(Mesh->mNumFaces);
			for (uint32_t b = 0u; b < Mesh->mNumFaces; b++) {
				const aiFace& Face = Mesh->mFaces[b];

				std::vector<std::vector<long long>> DIdx;

				DIdx.reserve(Face.mNumIndices);
				for (uint32_t c = 0u; c < Face.mNumIndices; c++) {
					long long indIdx = (long long)Face.mIndices[c];

					DIdx.push_back({
						indIdx + FaceOffset,
						(_HasTextureCoords ? (indIdx + TexturePointOffset) : -1),
						(_HasNormals ? (indIdx + NormalOffset) : -1)
					});
				}

				subMeshData->DataIdx.push_back(DIdx);
			}

			newModel->subMeshData.push_back(subMeshData);
		}

		newModel->meshPath = path;
		_ModelStorage::CachedModels.insert(std::make_pair(path, newModel));
		_ModelStorage::Importer.FreeScene();

		return newModel;
	}

	_ModelStorage::Importer.FreeScene();
	return nullptr;
}

void _ModelStorage::ClearStorage() {
	for (auto& model_data : _ModelStorage::CachedModels)
		delete model_data.second;

	_ModelStorage::CachedModels.clear();
}


typedef SMBC::ObjectStorage _ObjectStorage;

std::unordered_map<std::wstring, SMBC::CachedBlock*> _ObjectStorage::CachedBlocks = {};
std::unordered_map<std::wstring, SMBC::CachedPart*> _ObjectStorage::CachedParts = {};

SMBC::CachedBlock* _ObjectStorage::LoadBlock(SMBC::SM_Block& block, const bool& mat_by_color) {
	std::wstring _UuidStr = block.uuid;
	if (mat_by_color)
		SMBC::String::Combine(_UuidStr, L" ", block.color);

	if (_ObjectStorage::CachedBlocks.find(_UuidStr) != _ObjectStorage::CachedBlocks.end())
		return _ObjectStorage::CachedBlocks.at(_UuidStr);

	SMBC::CachedBlock* newBlock = new SMBC::CachedBlock();
	newBlock->blkPtr = block.blkPtr;
	newBlock->color = block.color;

	_ObjectStorage::CachedBlocks.insert(std::make_pair(_UuidStr, newBlock));
	return newBlock;
}

SMBC::CachedPart* _ObjectStorage::LoadPart(
	SMBC::SM_Part& part,
	const bool& load_uvs,
	const bool& load_normals,
	const bool& mat_by_color
) {
	std::wstring _UuidStr = part.objPtr->uuid;
	if (mat_by_color)
		SMBC::String::Combine(_UuidStr, L" ", part.color);

	if (_ObjectStorage::CachedParts.find(_UuidStr) != _ObjectStorage::CachedParts.end())
		return _ObjectStorage::CachedParts.at(_UuidStr);

	SMBC::CachedPart* newPart = new SMBC::CachedPart();
	newPart->objPtr = part.objPtr;
	newPart->color = part.color;
	newPart->modelPtr = _ModelStorage::LoadModel(newPart->objPtr->path, load_uvs, load_normals);

	_ObjectStorage::CachedParts.insert(std::make_pair(_UuidStr, newPart));
	return newPart;
}

std::string HexToFloat(const std::wstring& f) {
	float fr = (float)std::stoi(f.substr(0, 2), nullptr, 16) / 255.0f;
	float fg = (float)std::stoi(f.substr(2, 2), nullptr, 16) / 255.0f;
	float fb = (float)std::stoi(f.substr(4, 2), nullptr, 16) / 255.0f;

	std::string _output;
	SMBC::String::Combine(_output, fr, " ", fg, " ", fb);

	return _output;
}

void _ObjectStorage::WriteMtlFile(const std::wstring& path, const bool& add_colors) {
	std::ofstream _MtlFile(path);

	if (!_MtlFile.is_open()) return;

	std::string _fp_t = "Ns 324\nKa 1 1 1\nKd ";
	std::string _sp_t = "Ks 0.5 0.5 0.5\nKe 0 0 0\nNi 1.45\nd 1\nillum2\n";

	for (const auto& _PartData : _ObjectStorage::CachedParts) {
		const SMBC::CachedPart* _CPart = _PartData.second;
		
		if (_CPart->modelPtr == nullptr) continue;

		for (std::size_t b = 0; b < _CPart->modelPtr->subMeshData.size(); b++) {
			const SMBC::SubMeshData* SMData = _CPart->modelPtr->subMeshData[b];

			std::wstring _MtlName = _PartData.first + L' ' + std::to_wstring(SMData->SubMeshIndex);

			std::string _mtl_mat = "newmtl ";
			SMBC::String::Combine(_mtl_mat, _MtlName, "\n", _fp_t);

			if (add_colors)
				SMBC::String::Combine(_mtl_mat, HexToFloat(_CPart->color), "\n");
			else
				_mtl_mat.append("0.8 0.8 0.8\n");

			_mtl_mat.append(_sp_t);

			bool _Success = false;
			SMBC::Texture::TextureList _TList;

			SMBC::Texture::Texture& c_Tex = _CPart->objPtr->texList;
			switch (c_Tex.TexType) {
			case SMBC::Texture::TextureType::SubMeshList:
				_Success = c_Tex.GetTextureByName(std::to_wstring(SMData->SubMeshIndex), _TList);
				break;
			case SMBC::Texture::TextureType::SubMeshMap:
				_Success = c_Tex.GetTextureByName(SMData->MaterialName, _TList);
				break;
			}

			if (_Success) {
				if (!_TList.nor.empty())
					SMBC::String::Combine(_mtl_mat, "map_Bump ", _TList.nor, "\n");

				if (!_TList.dif.empty())
					SMBC::String::Combine(_mtl_mat, "map_Kd ", _TList.dif, "\n");

				if (!_TList.asg.empty())
					SMBC::String::Combine(_mtl_mat, "map_Ks ", _TList.asg, "\n");
			}

			_mtl_mat.append("\n");

			_MtlFile.write(_mtl_mat.c_str(), _mtl_mat.length());
		}
	}

	for (const auto& _BlockData : _ObjectStorage::CachedBlocks) {
		const SMBC::CachedBlock* _CBlock = _BlockData.second;

		if (_CBlock->blkPtr == nullptr) continue;

		std::string _mtl_mat = "newmtl ";
		SMBC::String::Combine(_mtl_mat, _BlockData.first, "\n", _fp_t);

		if (add_colors)
			SMBC::String::Combine(_mtl_mat, HexToFloat(_CBlock->color), "\n");
		else
			_mtl_mat.append("0.8 0.8 0.8\n");

		_mtl_mat.append(_sp_t);

		SMBC::Texture::TextureList& texData = _CBlock->blkPtr->texList;
		if (!texData.nor.empty())
			SMBC::String::Combine(_mtl_mat, "map_Bump ", texData.nor, "\n");

		if (!texData.dif.empty())
			SMBC::String::Combine(_mtl_mat, "map_Kd ", texData.dif, "\n");

		if (!texData.asg.empty())
			SMBC::String::Combine(_mtl_mat, "map_Ks ", texData.asg, "\n");

		_mtl_mat.append("\n");

		_MtlFile.write(_mtl_mat.c_str(), _mtl_mat.length());
	}

	_MtlFile.close();
}

void _ObjectStorage::WriteTexturePaths(const std::wstring& path) {
	std::ofstream _TextureOutput(path);

	if (!_TextureOutput.is_open()) return;

	nlohmann::json _TextureList;

	for (const auto& _CMeshData : _ObjectStorage::CachedParts) {
		const SMBC::CachedPart* _CMesh = _CMeshData.second;

		const SMBC::Texture::Texture& _CurTex = _CMesh->objPtr->texList;
		if (_CurTex.TexList.size() == 0) continue;

		std::string _MeshNameStr = SMBC::String::ToUtf8(_CMesh->objPtr->name);
		_TextureList[_MeshNameStr] = {};

		switch (_CurTex.TexType) {
		case SMBC::Texture::TextureType::SubMeshList:
			for (const auto& t_ListData : _CurTex.TexList) {
				const SMBC::Texture::TextureList& t_List = t_ListData.second;

				nlohmann::json t_Obj;

				if (!t_List.dif.empty()) t_Obj["Dif"] = SMBC::String::ToUtf8(t_List.dif);
				if (!t_List.asg.empty()) t_Obj["Asg"] = SMBC::String::ToUtf8(t_List.asg);
				if (!t_List.nor.empty()) t_Obj["Nor"] = SMBC::String::ToUtf8(t_List.nor);

				_TextureList[_MeshNameStr].push_back(t_Obj);
			}

			break;
		case SMBC::Texture::TextureType::SubMeshMap:
			for (const auto& t_ListData : _CurTex.TexList) {
				const SMBC::Texture::TextureList& t_List = t_ListData.second;
				std::string t_ListStr = SMBC::String::ToUtf8(t_ListData.first);

				nlohmann::json t_Obj;

				if (!t_List.dif.empty()) t_Obj["Dif"] = SMBC::String::ToUtf8(t_List.dif);
				if (!t_List.asg.empty()) t_Obj["Asg"] = SMBC::String::ToUtf8(t_List.asg);
				if (!t_List.nor.empty()) t_Obj["Nor"] = SMBC::String::ToUtf8(t_List.nor);

				_TextureList[_MeshNameStr][t_ListStr] = t_Obj;
			}

			break;
		}
	}

	for (const auto& _CBlockData : _ObjectStorage::CachedBlocks) {
		const SMBC::CachedBlock* _CBlock = _CBlockData.second;

		if (_CBlock->blkPtr == nullptr) continue;

		std::string _MeshNameStr = SMBC::String::ToUtf8(_CBlock->blkPtr->name);
		_TextureList[_MeshNameStr] = {};

		SMBC::Texture::TextureList& curTex = _CBlock->blkPtr->texList;

		nlohmann::json _TexObj;
		if (!curTex.dif.empty()) _TexObj["Dif"] = SMBC::String::ToUtf8(curTex.dif);
		if (!curTex.asg.empty()) _TexObj["Asg"] = SMBC::String::ToUtf8(curTex.asg);
		if (!curTex.nor.empty()) _TexObj["Nor"] = SMBC::String::ToUtf8(curTex.nor);

		_TextureList[_MeshNameStr].push_back(_TexObj);
	}

	_TextureOutput << std::setfill('\t') << std::setw(1) << _TextureList;
	_TextureOutput.close();
}

void _ObjectStorage::ClearStorage() {
	DebugOut("Clearing object storage\n");

	for (const auto& _BlockData : _ObjectStorage::CachedBlocks)
		delete _BlockData.second;

	for (const auto& _PartData : _ObjectStorage::CachedParts)
		delete _PartData.second;

	_ObjectStorage::CachedBlocks.clear();
	_ObjectStorage::CachedParts.clear();
}