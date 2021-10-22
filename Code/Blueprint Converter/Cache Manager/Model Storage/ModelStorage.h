#pragma once

#include "Blueprint Converter/Object Definitions/Model/Model.h"

#include <assimp/Importer.hpp>
#include <unordered_map>
#include <string>

namespace SMBC
{
	class ModelStorage
	{
		static std::unordered_map<std::wstring, SMBC::Model*> CachedModels;
		static Assimp::Importer Importer;

	public:
		static SMBC::Model* LoadModel(
			const std::wstring& path,
			const bool& load_uvs,
			const bool& load_normals
		);

		static void ClearStorage();
	};
}