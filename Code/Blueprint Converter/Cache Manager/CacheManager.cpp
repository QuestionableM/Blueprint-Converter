#include "Blueprint Converter/Cache Manager/CacheManager.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/File/FileFunc.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <nlohmann/json.hpp>

typedef SMBC::ModelStorage _ModelStorage;

std::vector<SMBC::Model*> _ModelStorage::CachedModels = {};
Assimp::Importer _ModelStorage::Importer = Assimp::Importer();

SMBC::Model* _ModelStorage::LoadModel(
	const std::wstring& path,
	const bool& load_uvs,
	const bool& load_normals
) {
	for (SMBC::Model*& model_ptr : _ModelStorage::CachedModels)
		if (model_ptr->meshPath == path || SMBC::FILE::IsEquivalent(model_ptr->meshPath, path))
			return model_ptr;

	const aiScene* ModelScene = _ModelStorage::Importer.ReadFile(
		SMBC::Other::WideToUtf8(path).c_str(),
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
				subMeshData->MaterialName = SMBC::Other::Utf8ToWide(StrMatName);
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
		_ModelStorage::CachedModels.push_back(newModel);
		_ModelStorage::Importer.FreeScene();

		return newModel;
	}

	_ModelStorage::Importer.FreeScene();
	return nullptr;
}

void _ModelStorage::ClearStorage() {
	for (SMBC::Model*& model_ptr : _ModelStorage::CachedModels)
		delete model_ptr;

	_ModelStorage::CachedModels.clear();
}


typedef SMBC::ObjectStorage _ObjectStorage;

std::vector<SMBC::CachedBlock*> _ObjectStorage::CachedBlocks = {};
std::vector<SMBC::CachedPart*> _ObjectStorage::CachedParts = {};

SMBC::CachedBlock* _ObjectStorage::LoadBlock(SMBC::SM_Block& block, const bool& mat_by_color) {
	std::wstring _UuidStr = block.uuid;
	if (mat_by_color) {
		_UuidStr.append(L" ");
		_UuidStr.append(block.color);
	}

	for (SMBC::CachedBlock*& block_ptr : _ObjectStorage::CachedBlocks)
		if (block_ptr->uuid == _UuidStr) return block_ptr;

	SMBC::CachedBlock* newBlock = new SMBC::CachedBlock();
	newBlock->blkPtr = block.blkPtr;
	newBlock->uuid = _UuidStr;
	newBlock->color = block.color;

	_ObjectStorage::CachedBlocks.push_back(newBlock);
	return newBlock;
}

SMBC::CachedPart* _ObjectStorage::LoadPart(
	SMBC::SM_Part& part,
	const bool& load_uvs,
	const bool& load_normals,
	const bool& mat_by_color
) {
	std::wstring _UuidStr = part.objPtr->_obj_uuid;
	if (mat_by_color) {
		_UuidStr.append(L" ");
		_UuidStr.append(part.color);
	}

	for (SMBC::CachedPart*& part_ptr : _ObjectStorage::CachedParts)
		if (part_ptr->uuid == _UuidStr) return part_ptr;

	SMBC::CachedPart* newPart = new SMBC::CachedPart();
	newPart->meshPath = part.objPtr->_obj_path;
	newPart->name = part.objPtr->_obj_name;
	newPart->texPaths = part.objPtr->obj_textures;
	newPart->color = part.color;
	newPart->uuid = _UuidStr;
	newPart->modelPtr = _ModelStorage::LoadModel(part.objPtr->_obj_path, load_uvs, load_normals);

	_ObjectStorage::CachedParts.push_back(newPart);
	return newPart;
}

std::string HexToFloat(std::wstring& f) {
	int ir = std::stoi(f.substr(0, 2), nullptr, 16);
	int ig = std::stoi(f.substr(2, 2), nullptr, 16);
	int ib = std::stoi(f.substr(4, 2), nullptr, 16);

	float fr = (float)ir / 255.0f;
	float fg = (float)ig / 255.0f;
	float fb = (float)ib / 255.0f;

	std::string _output;
	_output.append(std::to_string(fr));
	_output.append(" ");
	_output.append(std::to_string(fg));
	_output.append(" ");
	_output.append(std::to_string(fb));

	return _output;
}

void _ObjectStorage::WriteMtlFile(const std::wstring& path, const bool& add_colors) {
	std::ofstream _MtlFile(path);

	if (!_MtlFile.is_open()) return;

	std::string _fp_t = "Ns 324\nKa 1 1 1\nKd ";
	std::string _sp_t = "Ks 0.5 0.5 0.5\nKe 0 0 0\nNi 1.45\nd 1\nillum2\n";

	for (std::size_t a = 0; a < _ObjectStorage::CachedParts.size(); a++) {
		SMBC::CachedPart*& _CPart = _ObjectStorage::CachedParts[a];
		
		if (_CPart->modelPtr == nullptr) continue;

		for (std::size_t b = 0; b < _CPart->modelPtr->subMeshData.size(); b++) {
			SMBC::SubMeshData*& SMData = _CPart->modelPtr->subMeshData[b];

			std::wstring _MtlName = _CPart->uuid + L' ' + std::to_wstring(SMData->SubMeshIndex);

			std::string _mtl_mat;
			_mtl_mat.append("newmtl " + SMBC::Other::WideToUtf8(_MtlName) + '\n');
			_mtl_mat.append(_fp_t);

			if (add_colors)
				_mtl_mat.append(HexToFloat(_CPart->color) + '\n');
			else
				_mtl_mat.append("0.8 0.8 0.8\n");

			_mtl_mat.append(_sp_t);

			bool _Success = false;
			SMBC::Texture::TextureList _TList;

			switch (_CPart->texPaths.TexType) {
			case SMBC::Texture::TextureType::SubMeshList:
				_Success = _CPart->texPaths.GetTextureByName(std::to_wstring(SMData->SubMeshIndex), _TList);
				break;
			case SMBC::Texture::TextureType::SubMeshMap:
				_Success = _CPart->texPaths.GetTextureByName(SMData->MaterialName, _TList);
				break;
			}

			if (_Success) {
				if (!_TList.nor.empty()) {
					_mtl_mat.append("map_Bump ");
					_mtl_mat.append(SMBC::Other::WideToUtf8(_TList.nor));
					_mtl_mat.append("\n");
				}

				if (!_TList.dif.empty()) {
					_mtl_mat.append("map_Kd ");
					_mtl_mat.append(SMBC::Other::WideToUtf8(_TList.dif));
					_mtl_mat.append("\n");
				}

				if (!_TList.asg.empty()) {
					_mtl_mat.append("map_Ks ");
					_mtl_mat.append(SMBC::Other::WideToUtf8(_TList.asg));
					_mtl_mat.append("\n");
				}
			}

			_mtl_mat.append("\n");

			_MtlFile.write(_mtl_mat.c_str(), _mtl_mat.length());
		}
	}

	for (std::size_t a = 0; a < _ObjectStorage::CachedBlocks.size(); a++) {
		SMBC::CachedBlock*& _CBlock = _ObjectStorage::CachedBlocks[a];
		if (_CBlock->blkPtr == nullptr) continue;

		std::string _mtl_mat;
		_mtl_mat.append("newmtl " + SMBC::Other::WideToUtf8(_CBlock->uuid) + '\n');
		_mtl_mat.append(_fp_t);

		if (add_colors)
			_mtl_mat.append(HexToFloat(_CBlock->color) + '\n');
		else
			_mtl_mat.append("0.8 0.8 0.8\n");

		_mtl_mat.append(_sp_t);

		SMBC::Texture::TextureList& texData = _CBlock->blkPtr->_block_tex;
		if (!texData.nor.empty()) {
			_mtl_mat.append("map_Bump ");
			_mtl_mat.append(SMBC::Other::WideToUtf8(texData.nor));
			_mtl_mat.append("\n");
		}

		if (!texData.dif.empty()) {
			_mtl_mat.append("map_Kd ");
			_mtl_mat.append(SMBC::Other::WideToUtf8(texData.dif));
			_mtl_mat.append("\n");
		}

		if (!texData.asg.empty()) {
			_mtl_mat.append("map_Ks ");
			_mtl_mat.append(SMBC::Other::WideToUtf8(texData.asg));
			_mtl_mat.append("\n");
		}

		_mtl_mat.append("\n");

		_MtlFile.write(_mtl_mat.c_str(), _mtl_mat.length());
	}

	_MtlFile.close();
}

void _ObjectStorage::WriteTexturePaths(const std::wstring& path) {
	std::ofstream _TextureOutput(path);

	if (!_TextureOutput.is_open()) return;

	nlohmann::json _TextureList;

	for (SMBC::CachedPart*& _CMesh : _ObjectStorage::CachedParts) {
		SMBC::Texture::Texture& _CurTex = _CMesh->texPaths;
		if (_CurTex.TexList.size() == 0) continue;

		std::string _MeshNameStr = SMBC::Other::WideToUtf8(_CMesh->name);
		_TextureList[_MeshNameStr] = {};

		switch (_CurTex.TexType) {
		case SMBC::Texture::TextureType::SubMeshList:
			for (SMBC::Texture::TextureList& _TexList : _CurTex.TexList) {
				nlohmann::json _TextureObject;
				if (!_TexList.dif.empty()) _TextureObject["Dif"] = SMBC::Other::WideToUtf8(_TexList.dif);
				if (!_TexList.asg.empty()) _TextureObject["Asg"] = SMBC::Other::WideToUtf8(_TexList.asg);
				if (!_TexList.nor.empty()) _TextureObject["Nor"] = SMBC::Other::WideToUtf8(_TexList.nor);

				_TextureList[_MeshNameStr].push_back(_TextureObject);
			}

			break;
		case SMBC::Texture::TextureType::SubMeshMap:
			for (SMBC::Texture::TextureList& _TList : _CurTex.TexList) {
				std::string _TexListStr = SMBC::Other::WideToUtf8(_TList.name);

				nlohmann::json _TexObj;
				if (!_TList.dif.empty()) _TexObj["Dif"] = SMBC::Other::WideToUtf8(_TList.dif);
				if (!_TList.asg.empty()) _TexObj["Asg"] = SMBC::Other::WideToUtf8(_TList.asg);
				if (!_TList.nor.empty()) _TexObj["Nor"] = SMBC::Other::WideToUtf8(_TList.nor);

				_TextureList[_MeshNameStr][_TexListStr] = _TexObj;
			}

			break;
		}
	}

	for (SMBC::CachedBlock*& _CBlock : _ObjectStorage::CachedBlocks) {
		if (_CBlock->blkPtr == nullptr) continue;

		std::string _MeshNameStr = SMBC::Other::WideToUtf8(_CBlock->blkPtr->_obj_name);
		_TextureList[_MeshNameStr] = {};

		SMBC::Texture::TextureList& curTex = _CBlock->blkPtr->_block_tex;

		nlohmann::json _TexObj;
		if (!curTex.dif.empty()) _TexObj["Dif"] = SMBC::Other::WideToUtf8(curTex.dif);
		if (!curTex.asg.empty()) _TexObj["Asg"] = SMBC::Other::WideToUtf8(curTex.asg);
		if (!curTex.nor.empty()) _TexObj["Nor"] = SMBC::Other::WideToUtf8(curTex.nor);

		_TextureList[_MeshNameStr].push_back(_TexObj);
	}

	_TextureOutput << std::setw(4) << _TextureList;
	_TextureOutput.close();
}

void _ObjectStorage::ClearStorage() {
	for (SMBC::CachedBlock*& block_ptr : _ObjectStorage::CachedBlocks)
		delete block_ptr;

	for (SMBC::CachedPart*& part_ptr : _ObjectStorage::CachedParts)
		delete part_ptr;

	_ObjectStorage::CachedBlocks.clear();
	_ObjectStorage::CachedParts.clear();
}