#include "ModelStorage.h"

#include "BlueprintConverter\ConvertSettings.h"
#include "Lib\String.h"
#include "DebugCon.h"

#include <assimp/postprocess.h>

namespace SMBC
{
	std::unordered_map<std::wstring, Model*> ModelStorage::CachedModels = {};
	Assimp::Importer ModelStorage::Importer = Assimp::Importer();

	const aiScene* ModelStorage::LoadScene(const std::wstring& path)
	{
		return Importer.ReadFile(
			String::ToUtf8(path).c_str(),
			aiProcess_FindInvalidData |
			aiProcess_RemoveComponent |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_JoinIdenticalVertices |
			aiProcess_FindDegenerates |
			aiProcess_OptimizeMeshes
		);
	}

	void ModelStorage::LoadVertices(const aiMesh*& mesh, Model*& model)
	{
		const bool has_uvs     = ConvertSettings::ExportUvs     && mesh->HasTextureCoords(0);
		const bool has_normals = ConvertSettings::ExportNormals && mesh->HasNormals();

		if (has_uvs) model->uvs.reserve(mesh->mNumVertices);
		if (has_normals) model->normals.reserve(mesh->mNumVertices);

		model->vertices.reserve(mesh->mNumVertices);
		for (unsigned int a = 0; a < mesh->mNumVertices; a++)
		{
			model->vertices.push_back(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[a]));

			if (has_uvs) model->uvs.push_back(*reinterpret_cast<glm::vec2*>(&mesh->mTextureCoords[0][a]));
			if (has_normals) model->normals.push_back(*reinterpret_cast<glm::vec3*>(&mesh->mNormals[a]));
		}
	}

	void ModelStorage::LoadMaterialName(const aiScene*& scene, const aiMesh*& mesh, SubMeshData*& sub_mesh)
	{
		if (!scene->HasMaterials()) return;

		const aiMaterial* cMeshMat = scene->mMaterials[mesh->mMaterialIndex];
		aiString cMatName;
		cMeshMat->Get(AI_MATKEY_NAME, cMatName);

		const std::string cStrMatName = std::string(cMatName.C_Str(), cMatName.length);
		sub_mesh->MaterialName = String::ToWide(cStrMatName);
	}

	void ModelStorage::LoadIndices(const aiMesh*& mesh, Model*& model, SubMeshData*& sub_mesh)
	{
		const long long mVertOffset = model->vertices.size();
		const long long mUvOffset = model->uvs.size();
		const long long mNormalOffset = model->normals.size();

		const bool has_uvs     = ConvertSettings::ExportUvs     && mesh->HasTextureCoords(0);
		const bool has_normals = ConvertSettings::ExportNormals && mesh->HasNormals();

		sub_mesh->DataIdx.reserve(mesh->mNumFaces);
		for (unsigned int a = 0; a < mesh->mNumFaces; a++)
		{
			const aiFace& cFace = mesh->mFaces[a];
			std::vector<VertexData> d_idx;

			d_idx.reserve(cFace.mNumIndices);
			for (unsigned int b = 0; b < cFace.mNumIndices; b++)
			{
				long long ind_idx = (long long)cFace.mIndices[b];

				d_idx.push_back({
					ind_idx + mVertOffset,
					(has_uvs ? (ind_idx + mUvOffset) : -1),
					(has_normals ? (ind_idx + mNormalOffset) : -1)
				});
			}

			sub_mesh->DataIdx.push_back(d_idx);
		}
	}

	void ModelStorage::LoadSubMeshes(const aiScene*& scene, Model*& model)
	{
		model->subMeshData.reserve(scene->mNumMeshes);
		for (unsigned int a = 0; a < scene->mNumMeshes; a++)
		{
			const aiMesh* cMesh = scene->mMeshes[a];
			SubMeshData* cSubMesh = new SubMeshData(a);

			ModelStorage::LoadIndices(cMesh, model, cSubMesh);
			ModelStorage::LoadVertices(cMesh, model);
			ModelStorage::LoadMaterialName(scene, cMesh, cSubMesh);

			model->subMeshData.push_back(cSubMesh);
		}
	}

	void ModelStorage::CalculateMeshCenter(Model*& model)
	{
		glm::vec3 pCenter(0.0f);

		const std::size_t mVertexAmount = model->vertices.size();
		for (std::size_t pVertId = 0; pVertId < mVertexAmount; pVertId++)
			pCenter += model->vertices[pVertId];

		model->mCenterPoint = pCenter / (float)mVertexAmount;
	}

	Model* ModelStorage::LoadModel(const std::wstring& path)
	{
		if (CachedModels.find(path) != CachedModels.end())
			return CachedModels.at(path);

		const aiScene* ModelScene = ModelStorage::LoadScene(path);
		if (ModelScene && ModelScene->HasMeshes())
		{
			Model* newModel = new Model(path);

			ModelStorage::LoadSubMeshes(ModelScene, newModel);
			ModelStorage::CalculateMeshCenter(newModel);

			CachedModels.insert(std::make_pair(path, newModel));
			Importer.FreeScene();

			return newModel;
		}

		DebugErrorL("[Model] Couldn't load: ", path);

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