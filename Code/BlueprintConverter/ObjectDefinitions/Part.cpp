#include "Part.h"

#include "BlueprintConverter\ObjectDefinitions\Model.h"
#include "BlueprintConverter\ConvertSettings.h"

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.h"

#include "Lib\ConvData.h"
#include "Lib\String.h"

#include <gtx/transform.hpp>

namespace SMBC
{
	std::string Part::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
	{
		std::string out_str = pParent->Uuid.ToString();

		{
			if (ConvertSettings::MatByColor)
				out_str.append(" " + mColor.StringHex());

			out_str.append(" " + std::to_string(mIdx + 1));
		}

		{
			const SubMeshData* pSubMesh = pModel->subMeshData[mIdx];
			const std::wstring tex_name = pParent->TextureList.PickString(mIdx, mat_name);

			std::string material_idx = "m1";

			ObjectTextureData oTexData;
			if (pParent->TextureList.GetEntry(tex_name, oTexData.mTextures))
				material_idx = MaterialManager::GetMaterialA(oTexData.mTextures.material);

			out_str.append(" " + material_idx);
		}

		return out_str;
	}

	void Part::FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const
	{
		std::string mtl_first_part = pParent->Uuid.ToString() + " ";

		if (ConvertSettings::MatByColor)
			mtl_first_part.append(mColor.StringHex() + " ");

		for (std::size_t a = 0; a < pModel->subMeshData.size(); a++)
		{
			const SubMeshData* pSubMesh = pModel->subMeshData[a];
			const std::wstring tex_name = pParent->TextureList.PickString(a, pSubMesh->MaterialName);

			ObjectTextureData oTexData;
			if (pParent->TextureList.GetEntry(tex_name, oTexData.mTextures))
			{
				oTexData.mColor = mColor;

				const std::string mat_idx = MaterialManager::GetMaterialA(oTexData.mTextures.material);
				const std::string mtl_name = mtl_first_part + std::to_string(a + 1) + " " + mat_idx;

				if (tex_map.find(mtl_name) != tex_map.end())
					continue;

				tex_map.insert(std::make_pair(mtl_name, oTexData));
			}
		}
	}

	void Part::FillTextureJson(nlohmann::json& mJson) const
	{
		const std::string mNameStr = String::ToUtf8(this->GetName());

		if (mJson.find(mNameStr) == mJson.end())
			mJson[mNameStr] = pParent->TextureList.ToJson();

		ConvData::ProgressValue++;
	}

	glm::vec3 Part::GetCenterPoint() const
	{
		const glm::vec3 pModelCenter = pModel->GetCenterPoint();
		const glm::mat4 model_mat = this->GetTransformMatrix();

		return model_mat * glm::vec4(pModelCenter, 1.0f);
	}



	glm::mat4 Part::GetTransformMatrix() const
	{
		const glm::mat4 axis_rotation = Rotations::GetRotationMatrix(this->mAxis);

		glm::mat4 model_matrix(1.0f);
		model_matrix *= glm::translate(this->mPosition);
		model_matrix *= axis_rotation;
		model_matrix *= glm::translate(pParent->Bounds / 2.0f);

		return model_matrix;
	}
}