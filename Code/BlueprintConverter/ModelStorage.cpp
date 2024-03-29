#include "ModelStorage.hpp"

#include "BlueprintConverter\ConvertSettings.hpp"
#include "Lib\String.hpp"
#include "DebugCon.hpp"

#include <assimp\postprocess.h>

namespace SMBC
{
	const aiScene* ModelStorage::LoadScene(const std::wstring& path)
	{
		return Importer.ReadFile(
			String::ToUtf8(path).c_str(),
			aiProcess_FindInvalidData |
			aiProcess_RemoveComponent |
			aiProcess_JoinIdenticalVertices |
			aiProcess_FindDegenerates
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
		const std::size_t vertex_offset = model->vertices.size();
		const std::size_t uv_offset     = model->uvs.size();
		const std::size_t normal_offset = model->normals.size();

		sub_mesh->DataIdx.reserve(mesh->mNumFaces);
		for (unsigned int a = 0; a < mesh->mNumFaces; a++)
		{
			const aiFace& cFace = mesh->mFaces[a];
			std::vector<VertexData> d_idx;

			d_idx.reserve(cFace.mNumIndices);
			for (unsigned int b = 0; b < cFace.mNumIndices; b++)
			{
				const std::size_t ind_idx = (std::size_t)cFace.mIndices[b];

				d_idx.push_back({
					ind_idx + vertex_offset,
					(sub_mesh->has_uvs ? (ind_idx + uv_offset) : 0),
					(sub_mesh->has_normals ? (ind_idx + normal_offset) : 0)
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

			cSubMesh->has_normals = ConvertSettings::ExportNormals && cMesh->HasNormals();
			cSubMesh->has_uvs     = ConvertSettings::ExportUvs && cMesh->HasTextureCoords(0);

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

		model->mCenterPoint = pCenter / static_cast<float>(mVertexAmount);
	}

	Model* ModelStorage::LoadModel(const std::wstring& path)
	{
		const ModelStorageMap::iterator it = CachedModels.find(path);
		if (it != CachedModels.end())
			return it->second;

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