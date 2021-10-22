#include "ModelStorage.h"
#include "Lib/String/String.h"
#include "DebugCon.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace SMBC
{
	std::unordered_map<std::wstring, Model*> ModelStorage::CachedModels = {};
	Assimp::Importer ModelStorage::Importer = Assimp::Importer();

	Model* ModelStorage::LoadModel(
		const std::wstring& path,
		const bool& load_uvs,
		const bool& load_normals
	) {
		if (CachedModels.find(path) != CachedModels.end())
			return CachedModels.at(path);

		DebugOut("[Model] Loading: ", path, "\n");

		const aiScene* ModelScene = Importer.ReadFile(
			SMBC::String::ToUtf8(path).c_str(),
			aiProcess_FindInvalidData |
			aiProcess_RemoveComponent |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_JoinIdenticalVertices |
			aiProcess_FindDegenerates |
			aiProcess_OptimizeMeshes
		);

		if (ModelScene && ModelScene->HasMeshes())
		{
			SMBC::Model* newModel = new SMBC::Model();

			long long FaceOffset = 0ll;
			long long TexturePointOffset = 0ll;
			long long NormalOffset = 0ll;

			bool _HasMaterials = ModelScene->HasMaterials();

			newModel->subMeshData.reserve(ModelScene->mNumMeshes);
			for (uint32_t a = 0u; a < ModelScene->mNumMeshes; a++)
			{
				FaceOffset = (long long)newModel->vertices.size();
				TexturePointOffset = (long long)newModel->uvs.size();
				NormalOffset = (long long)newModel->normals.size();

				const aiMesh* Mesh = ModelScene->mMeshes[a];

				bool _HasTextureCoords = (load_uvs && Mesh->mTextureCoords[0] != NULL);
				bool _HasNormals = (load_normals && Mesh->HasNormals());

				if (_HasTextureCoords) newModel->uvs.reserve(Mesh->mNumVertices);
				if (_HasNormals) newModel->normals.reserve(Mesh->mNumVertices);

				newModel->vertices.reserve(Mesh->mNumVertices);
				for (uint32_t b = 0u; b < Mesh->mNumVertices; b++)
				{
					newModel->vertices.push_back(*reinterpret_cast<glm::vec3*>(&Mesh->mVertices[b]));

					if (!_HasTextureCoords && !_HasNormals) continue;

					if (_HasTextureCoords)
					{
						newModel->uvs.push_back(*reinterpret_cast<glm::vec2*>(&Mesh->mTextureCoords[0][b]));
					}

					if (_HasNormals)
					{
						newModel->normals.push_back(*reinterpret_cast<glm::vec3*>(&Mesh->mNormals[b]));
					}
				}

				SMBC::SubMeshData* subMeshData = new SMBC::SubMeshData();
				subMeshData->SubMeshIndex = a;
				subMeshData->MaterialName = L"";

				if (_HasMaterials)
				{
					const aiMaterial* MeshMat = ModelScene->mMaterials[Mesh->mMaterialIndex];
					aiString MatName;
					MeshMat->Get(AI_MATKEY_NAME, MatName);

					std::string StrMatName = std::string(MatName.C_Str(), MatName.length);
					subMeshData->MaterialName = SMBC::String::ToWide(StrMatName);
				}

				subMeshData->DataIdx.reserve(Mesh->mNumFaces);
				for (uint32_t b = 0u; b < Mesh->mNumFaces; b++)
				{
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
			CachedModels.insert(std::make_pair(path, newModel));
			Importer.FreeScene();

			return newModel;
		}

		Importer.FreeScene();
		return nullptr;
	}

	void ModelStorage::ClearStorage()
	{
		for (auto& model_data : CachedModels)
			delete model_data.second;

		CachedModels.clear();
	}
}