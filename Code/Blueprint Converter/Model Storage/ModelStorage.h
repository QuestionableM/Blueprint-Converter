#pragma once

#include "Blueprint Converter/Object Definitions/Model/Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <unordered_map>
#include <string>

namespace SMBC
{
	class ModelStorage
	{
		static std::unordered_map<std::wstring, SMBC::Model*> CachedModels;
		static Assimp::Importer Importer;

		static const aiScene* LoadScene(const std::wstring& path);
		static void LoadVertices(const aiMesh*& mesh, Model*& model);
		static void LoadMaterialName(const aiScene*& scene, const aiMesh*& mesh, SubMeshData*& sub_mesh);
		static void LoadIndices(const aiMesh*& mesh, Model*& model, SubMeshData*& sub_mesh);
		static void LoadSubMeshes(const aiScene*& scene, Model*& model);

	public:
		static SMBC::Model* LoadModel(const std::wstring& path);
		static void ClearStorage();
	};
}