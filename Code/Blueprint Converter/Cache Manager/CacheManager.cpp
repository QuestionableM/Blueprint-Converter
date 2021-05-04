#include "Blueprint Converter/Cache Manager/CacheManager.h"
#include "Lib/OtherFunc/OtherFunc.h"

SMBC::CachedMesh::~CachedMesh() {
	this->MeshName.clear();
	this->MeshPath.clear();
	this->MeshUuid.clear();
	this->Normals.clear();
	this->SubMeshCache.clear();
	this->TexPaths.TexList.clear();
	this->TexturePoints.clear();
	this->Vertices.clear();
}

void SMBC::CacheManager::LoadBlockIntoCache(SMBC::SM_Block& block) {
	for (SMBC::CachedBlock& blk : this->CachedBlocks)
		if (blk.uuid == block.uuid) return;

	SMBC::CachedBlock NewBlock;
	NewBlock.name = block.name;
	NewBlock.uuid = block.uuid;
	NewBlock.TexList = block.tex_list;

	this->CachedBlocks.push_back(NewBlock);
}

#include <assimp/scene.h>
#include <assimp/postprocess.h>

void SMBC::CacheManager::LoadPartIntoCache(
	SMBC::SM_Part& part,
	SMBC::CachedMesh& _CachedMesh,
	const bool& load_uvs,
	const bool& load_normals
) {
	const aiScene* _ModelScene = this->Importer.ReadFile(
		SMBC::Other::WideToUtf8(part.object_path).c_str(),
		aiProcess_FindInvalidData |
		aiProcess_RemoveComponent |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FindDegenerates |
		aiProcess_OptimizeMeshes
	);

	long long _FaceOffset = 0;
	long long _TexturePointOffset = 0;
	long long _NormalOffset = 0;
	if (_ModelScene && _ModelScene->HasMeshes()) {

		_CachedMesh.SubMeshCache.reserve(_ModelScene->mNumMeshes);
		for (uint32_t a = 0u; a < _ModelScene->mNumMeshes; a++) {
			_FaceOffset = (long long)_CachedMesh.Vertices.size();
			_TexturePointOffset = (long long)_CachedMesh.TexturePoints.size();
			_NormalOffset = (long long)_CachedMesh.Normals.size();

			const aiMesh* _Mesh = _ModelScene->mMeshes[a];

			_CachedMesh.Vertices.reserve(_Mesh->mNumVertices);
			bool _HasTextureCoords = (load_uvs && _Mesh->mTextureCoords[0] != NULL);
			bool _HasNormals = (load_normals && _Mesh->HasNormals());

			if (_HasTextureCoords) _CachedMesh.TexturePoints.reserve(_Mesh->mNumVertices);
			if (_HasNormals) _CachedMesh.Normals.reserve(_Mesh->mNumVertices);

			for (uint32_t b = 0u; b < _Mesh->mNumVertices; b++) {
				aiVector3D& _OldVertex = _Mesh->mVertices[b];
				_CachedMesh.Vertices.push_back({ _OldVertex.x, _OldVertex.y, _OldVertex.z });

				if (!_HasTextureCoords && !_HasNormals) continue;

				if (_HasTextureCoords) {
					aiVector3D& UVpt = _Mesh->mTextureCoords[0][b];
					_CachedMesh.TexturePoints.push_back({ UVpt.x, UVpt.y });
				}

				if (_HasNormals) {
					aiVector3D& NormalPt = _Mesh->mNormals[b];
					_CachedMesh.Normals.push_back({ NormalPt.x, NormalPt.y, NormalPt.z });
				}
			}

			SMBC::SubMeshCache _SubMeshCache;
			_SubMeshCache._MeshName = part.uuid + L" " + std::to_wstring(a);

			SMBC::Texture::TextureList _TList;
			bool _Success = false;

			switch (part.tex.TexType) {
			case SMBC::Texture::TextureType::SubMeshList:
				_Success = part.tex.GetTextureByName(std::to_wstring(a), _TList);
				break;
			case SMBC::Texture::TextureType::SubMeshMap:
				const aiMaterial* _MeshMat = _ModelScene->mMaterials[_Mesh->mMaterialIndex];
				aiString _MatName;
				_MeshMat->Get(AI_MATKEY_NAME, _MatName);

				std::string _StrName = std::string(_MatName.C_Str(), _MatName.length);
				std::wstring _WstrName = SMBC::Other::Utf8ToWide(_StrName);

				_Success = part.tex.GetTextureByName(_WstrName, _TList);
				break;
			}

			if (_Success && !_TList.dif.empty())
				_SubMeshCache._TexPath = _TList.dif;

			_SubMeshCache._DataIdx.reserve(_Mesh->mNumFaces);
			for (uint32_t b = 0; b < _Mesh->mNumFaces; b++) {
				const aiFace& _Face = _Mesh->mFaces[b];

				std::vector<std::vector<long long>> _DIdx;

				_DIdx.reserve(_Face.mNumIndices);
				for (uint32_t c = 0; c < _Face.mNumIndices; c++) {
					long long _indIdx = (long long)_Face.mIndices[c];

					_DIdx.push_back({
						_indIdx + _FaceOffset,
						(_HasTextureCoords ? (_indIdx + _TexturePointOffset) : -1),
						(_HasNormals ? (_indIdx + _NormalOffset) : -1)
						});
				}

				_SubMeshCache._DataIdx.push_back(_DIdx);
			}

			_CachedMesh.SubMeshCache.push_back(_SubMeshCache);
		}

		_CachedMesh.MeshUuid = part.uuid;
		_CachedMesh.TexPaths = part.tex;
		_CachedMesh.MeshName = part.name;
		_CachedMesh.MeshPath = part.object_path;

		this->CachedParts.push_back(_CachedMesh);
	}

	this->Importer.FreeScene();
}

void SMBC::CacheManager::ClearCachedMeshList() {
	for (uint32_t a = 0u; a < this->CachedParts.size(); a++) {
		SMBC::CachedMesh& _CMesh = this->CachedParts[a];

		for (uint32_t b = 0u; b < _CMesh.SubMeshCache.size(); b++) {
			SMBC::SubMeshCache& _SMeshCache = _CMesh.SubMeshCache[b];

			for (uint32_t c = 0u; c < _SMeshCache._DataIdx.size(); c++)
				_SMeshCache._DataIdx[c].clear();

			_SMeshCache._DataIdx.clear();
		}

		_CMesh.SubMeshCache.clear();
		_CMesh.MeshName.clear();
		_CMesh.MeshPath.clear();
		_CMesh.MeshUuid.clear();
		_CMesh.TexturePoints.clear();
		_CMesh.Normals.clear();
		_CMesh.Vertices.clear();
	}
	this->CachedParts.clear();

	for (uint32_t a = 0u; a < this->CachedBlocks.size(); a++) {
		SMBC::CachedBlock& _CBlock = this->CachedBlocks[a];

		_CBlock.name.clear();
		_CBlock.uuid.clear();
	}
	this->CachedBlocks.clear();
}

bool SMBC::CacheManager::GetCachedMesh(const std::wstring& uuid, SMBC::CachedMesh& c_mesh) {
	for (SMBC::CachedMesh& cMesh : this->CachedParts)
		if (cMesh.MeshUuid == uuid) {
			c_mesh = cMesh;
			return true;
		}

	return false;
}

void SMBC::CacheManager::WriteMtlFile(const std::wstring& path) {
	std::ofstream _MtlFile(path);

	if (!_MtlFile.is_open()) return;

	std::string _main_text = "Ns 323.999994\nKa 1.000000 1.000000 1.000000\nKd 0.800000 0.800000 0.800000\nKs 0.500000 0.500000 0.500000\nKe 0.000000 0.000000 0.000000\nNi 1.450000\nd 1.000000\nillum 2\n";

	for (unsigned int a = 0; a < this->CachedParts.size(); a++) {
		SMBC::CachedMesh& _CMesh = this->CachedParts[a];

		for (unsigned int b = 0; b < _CMesh.SubMeshCache.size(); b++) {
			SMBC::SubMeshCache& SMCache = _CMesh.SubMeshCache[b];

			std::string _mtl_mat = "newmtl " + SMBC::Other::WideToUtf8(SMCache._MeshName) + '\n' + _main_text;

			if (!SMCache._TexPath.empty())
				_mtl_mat += "map_Kd " + SMBC::Other::WideToUtf8(SMCache._TexPath) + '\n';

			_mtl_mat += '\n';

			_MtlFile.write(_mtl_mat.c_str(), _mtl_mat.length());
		}
	}

	for (uint32_t a = 0; a < this->CachedBlocks.size(); a++) {
		SMBC::CachedBlock& _CBlock = this->CachedBlocks[a];

		std::string _mtl_mat = "newmtl " + SMBC::Other::WideToUtf8(_CBlock.uuid) + '\n' + _main_text;

		if (!_CBlock.TexList.dif.empty())
			_mtl_mat += "map_Kd " + SMBC::Other::WideToUtf8(_CBlock.TexList.dif) + '\n';

		_mtl_mat += '\n';

		_MtlFile.write(_mtl_mat.c_str(), _mtl_mat.length());
	}

	_MtlFile.close();
}

#include <filesystem>
#include <nlohmann/json.hpp>

void SMBC::CacheManager::WriteTexturePaths(const std::wstring& path) {
	std::ofstream _TextureOutput(path);

	if (_TextureOutput.is_open()) {
		nlohmann::json _TextureList;

		for (SMBC::CachedMesh& _CMesh : this->CachedParts) {
			SMBC::Texture::Texture& _CurTex = _CMesh.TexPaths;
			if (_CurTex.TexList.size() == 0) continue;

			std::string _MeshNameStr = SMBC::Other::WideToUtf8(_CMesh.MeshName);
			_TextureList[_MeshNameStr] = {};

			switch (_CMesh.TexPaths.TexType) {
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

		for (SMBC::CachedBlock& _CBlock : this->CachedBlocks) {
			std::string _MeshNameStr = SMBC::Other::WideToUtf8(_CBlock.name);
			_TextureList[_MeshNameStr] = {};

			nlohmann::json _TexObj;
			if (!_CBlock.TexList.dif.empty()) _TexObj["Dif"] = SMBC::Other::WideToUtf8(_CBlock.TexList.dif);
			if (!_CBlock.TexList.asg.empty()) _TexObj["Asg"] = SMBC::Other::WideToUtf8(_CBlock.TexList.asg);
			if (!_CBlock.TexList.nor.empty()) _TexObj["Nor"] = SMBC::Other::WideToUtf8(_CBlock.TexList.nor);

			_TextureList[_MeshNameStr].push_back(_TexObj);
		}

		_TextureOutput << std::setw(4) << _TextureList;
		_TextureOutput.close();
	}
}