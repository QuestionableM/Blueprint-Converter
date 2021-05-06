#include "Blueprint Converter/Cache Manager/CacheManager.h"
#include "Lib/OtherFunc/OtherFunc.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool SMBC::CacheManager::LoadModel(
	std::wstring& path,
	SMBC::Model& model,
	const bool& load_uvs,
	const bool& load_normals
) {
	for (SMBC::Model& _m : this->CachedModels)
		if (_m.meshPath == path) {
			model = _m;
			return true;
		}

	const aiScene* ModelScene = this->Importer.ReadFile(
		SMBC::Other::WideToUtf8(path).c_str(),
		aiProcess_FindInvalidData |
		aiProcess_RemoveComponent |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_JoinIdenticalVertices |
		aiProcess_FindDegenerates |
		aiProcess_OptimizeMeshes
	);

	bool _HasMaterials = ModelScene->HasMaterials();

	long long FaceOffset = 0ll;
	long long TexturePointOffset = 0ll;
	long long NormalOffset = 0ll;

	if (ModelScene && ModelScene->HasMeshes()) {
		model.subMeshData.reserve(ModelScene->mNumMeshes);
		for (uint32_t a = 0u; a < ModelScene->mNumMeshes; a++) {
			FaceOffset = (long long)model.vertices.size();
			TexturePointOffset = (long long)model.uvs.size();
			NormalOffset = (long long)model.normals.size();

			const aiMesh* Mesh = ModelScene->mMeshes[a];

			bool _HasTextureCoords = (load_uvs && Mesh->mTextureCoords[0] != NULL);
			bool _HasNormals = (load_normals && Mesh->HasNormals());

			if (_HasTextureCoords) model.uvs.reserve(Mesh->mNumVertices);
			if (_HasNormals) model.normals.reserve(Mesh->mNumVertices);

			model.vertices.reserve(Mesh->mNumVertices);
			for (uint32_t b = 0u; b < Mesh->mNumVertices; b++) {
				aiVector3D& aiVert = Mesh->mVertices[b];
				model.vertices.push_back({ aiVert.x, aiVert.y, aiVert.z });

				if (!_HasTextureCoords && !_HasNormals) continue;

				if (_HasTextureCoords) {
					aiVector3D& Uv = Mesh->mTextureCoords[0][b];
					model.uvs.push_back({ Uv.x, Uv.y });
				}

				if (_HasNormals) {
					aiVector3D& Normal = Mesh->mNormals[b];
					model.normals.push_back({ Normal.x, Normal.y, Normal.z });
				}
			}

			SMBC::SubMeshData subMeshData;
			subMeshData.SubMeshIndex = a;
			subMeshData.MaterialName = L"";

			if (_HasMaterials) {
				const aiMaterial* MeshMat = ModelScene->mMaterials[Mesh->mMaterialIndex];
				aiString MatName;
				MeshMat->Get(AI_MATKEY_NAME, MatName);

				std::string StrMatName = std::string(MatName.C_Str(), MatName.length);
				subMeshData.MaterialName = SMBC::Other::Utf8ToWide(StrMatName);
			}

			subMeshData.DataIdx.reserve(Mesh->mNumFaces);
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

				subMeshData.DataIdx.push_back(DIdx);
			}

			model.subMeshData.push_back(subMeshData);
		}

		model.meshPath = path;

		this->CachedModels.push_back(model);
		this->Importer.FreeScene();
		return true;
	}

	this->Importer.FreeScene();
	return false;
}

void SMBC::CacheManager::LoadPart(
	SMBC::SM_Part& part,
	SMBC::CachedPart& cached_part,
	const bool& load_uvs,
	const bool& load_normal,
	const bool& mat_by_color
) {
	std::wstring _UuidTest = part.uuid;
	if (mat_by_color) {
		_UuidTest.append(L" ");
		_UuidTest.append(part.color);
	}

	for (SMBC::CachedPart& p : this->CachedParts)
		if (p.uuid == _UuidTest) {
			cached_part = p;
			return;
		}

	cached_part.meshPath = part.object_path;
	cached_part.name = part.name;
	cached_part.texPaths = part.tex;
	cached_part.uuid = _UuidTest;

	this->CachedParts.push_back(cached_part);
}

void SMBC::CacheManager::LoadBlock(SMBC::SM_Block& block, const bool& mat_by_color) {
	std::wstring _UuidTest = block.uuid;
	if (mat_by_color) {
		_UuidTest.append(L" ");
		_UuidTest.append(block.color);
	}

	for (SMBC::CachedBlock& blk : this->CachedBlocks)
		if (blk.uuid == _UuidTest) return;

	SMBC::CachedBlock NewBlock;
	NewBlock.name = block.name;
	NewBlock.uuid = _UuidTest;
	NewBlock.texList = block.tex_list;

	this->CachedBlocks.push_back(NewBlock);
}

bool SMBC::CacheManager::GetModel(const std::wstring& path, SMBC::Model& model) {
	for (SMBC::Model& m : this->CachedModels)
		if (m.meshPath == path) {
			model = m;
			return true;
		}
	
	return false;
}

void SMBC::CacheManager::ClearCache() {
	this->CachedModels.clear();
	this->CachedBlocks.clear();
	this->CachedParts.clear();
}

void SMBC::CacheManager::WriteMtlFile(const std::wstring& path) {
	std::ofstream _MtlFile(path);

	if (!_MtlFile.is_open()) return;

	std::string _main_text = "Ns 323.999994\nKa 1.000000 1.000000 1.000000\nKd 0.800000 0.800000 0.800000\nKs 0.500000 0.500000 0.500000\nKe 0.000000 0.000000 0.000000\nNi 1.450000\nd 1.000000\nillum 2\n";

	for (uint32_t a = 0u; a < this->CachedParts.size(); a++) {
		SMBC::CachedPart& _CPart = this->CachedParts[a];

		SMBC::Model _CModel;
		if (!this->GetModel(_CPart.meshPath, _CModel)) continue;

		for (uint32_t b = 0u; b < _CModel.subMeshData.size(); b++) {
			SMBC::SubMeshData& SMData = _CModel.subMeshData[b];

			std::wstring _MtlName = _CPart.uuid + L' ' + std::to_wstring(SMData.SubMeshIndex);

			std::string _mtl_mat = "newmtl " + SMBC::Other::WideToUtf8(_MtlName) + '\n' + _main_text;

			bool _Success = false;
			SMBC::Texture::TextureList _TList;

			switch (_CPart.texPaths.TexType) {
			case SMBC::Texture::TextureType::SubMeshList:
				_Success = _CPart.texPaths.GetTextureByName(std::to_wstring(SMData.SubMeshIndex), _TList);
				break;
			case SMBC::Texture::TextureType::SubMeshMap:
				_Success = _CPart.texPaths.GetTextureByName(SMData.MaterialName, _TList);
				break;
			}

			if (_Success && !_TList.dif.empty())
				_mtl_mat += "map_Kd " + SMBC::Other::WideToUtf8(_TList.dif) + '\n';

			_mtl_mat += '\n';

			_MtlFile.write(_mtl_mat.c_str(), _mtl_mat.length());
		}
	}

	for (uint32_t a = 0u; a < this->CachedBlocks.size(); a++) {
		SMBC::CachedBlock& _CBlock = this->CachedBlocks[a];

		std::string _mtl_mat = "newmtl " + SMBC::Other::WideToUtf8(_CBlock.uuid) + '\n' + _main_text;

		if (!_CBlock.texList.dif.empty())
			_mtl_mat += "map_Kd " + SMBC::Other::WideToUtf8(_CBlock.texList.dif) + '\n';

		_mtl_mat += '\n';

		_MtlFile.write(_mtl_mat.c_str(), _mtl_mat.length());
	}

	_MtlFile.close();
}

#include <filesystem>
#include <nlohmann/json.hpp>

void SMBC::CacheManager::WriteTexturePaths(const std::wstring& path) {
	std::ofstream _TextureOutput(path);

	if (!_TextureOutput.is_open()) return;

	nlohmann::json _TextureList;

	for (SMBC::CachedPart& _CMesh : this->CachedParts) {
		SMBC::Texture::Texture& _CurTex = _CMesh.texPaths;
		if (_CurTex.TexList.size() == 0) continue;

		std::string _MeshNameStr = SMBC::Other::WideToUtf8(_CMesh.name);
		_TextureList[_MeshNameStr] = {};

		switch (_CMesh.texPaths.TexType) {
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
		if (!_CBlock.texList.dif.empty()) _TexObj["Dif"] = SMBC::Other::WideToUtf8(_CBlock.texList.dif);
		if (!_CBlock.texList.asg.empty()) _TexObj["Asg"] = SMBC::Other::WideToUtf8(_CBlock.texList.asg);
		if (!_CBlock.texList.nor.empty()) _TexObj["Nor"] = SMBC::Other::WideToUtf8(_CBlock.texList.nor);

		_TextureList[_MeshNameStr].push_back(_TexObj);
	}

	_TextureOutput << std::setw(4) << _TextureList;
	_TextureOutput.close();
}