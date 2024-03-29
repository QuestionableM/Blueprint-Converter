#include "Joint.hpp"

#include "BlueprintConverter\ObjectDefinitions\Model.hpp"
#include "BlueprintConverter\ConvertSettings.hpp"

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.hpp"

#include "Lib\ConvData.hpp"
#include "Lib\String.hpp"

#include <gtx\transform.hpp>

namespace SMBC
{
	std::string Joint::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
	{
		std::string out_str = pParent->m_uuid.ToString();
		
		{
			if (ConvertSettings::MatByColor)
				out_str.append(" " + mColor.StringHex());

			out_str.append(" " + std::to_string(mIdx + 1));
		}

		{
			const SubMeshData* pSubMesh = pModel->subMeshData[mIdx];
			//const std::wstring tex_name = pParent->m_textureList.PickString(mIdx, mat_name);

			std::string material_idx = "m1";

			ObjectTextureData oTexData;
			if (pParent->m_textureList->GetEntry(mat_name, mIdx, &oTexData.mTextures))
				material_idx = MaterialManager::GetMaterialA(oTexData.mTextures->material);

			out_str.append(" " + material_idx);
		}

		return out_str;
	}

	void Joint::FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const
	{
		std::string mtl_first_part = pParent->m_uuid.ToString() + " ";

		if (ConvertSettings::MatByColor)
			mtl_first_part.append(mColor.StringHex() + " ");

		for (std::size_t a = 0; a < pModel->subMeshData.size(); a++)
		{
			const SubMeshData* pSubMesh = pModel->subMeshData[a];

			ObjectTextureData oTexData;
			if (pParent->m_textureList->GetEntry(pSubMesh->MaterialName, a, &oTexData.mTextures))
			{
				oTexData.mColor = this->mColor;

				const std::string mat_idx = MaterialManager::GetMaterialA(oTexData.mTextures->material);
				const std::string mtl_name = mtl_first_part + std::to_string(a + 1) + " " + mat_idx;
				
				if (tex_map.find(mtl_name) != tex_map.end())
					continue;

				tex_map.insert(std::make_pair(mtl_name, oTexData));
			}
		}
	}

	void Joint::FillTextureJson(nlohmann::json& mJson) const
	{
		const std::string mNameStr = String::ToUtf8(this->GetName());

		if (mJson.find(mNameStr) == mJson.end())
			mJson[mNameStr] = pParent->m_textureList->ToJson();

		ConvData::ProgressValue++;
	}

	glm::vec3 Joint::GetCenterPoint() const
	{
		const glm::vec3 pModelCenter = pModel->GetCenterPoint();
		const glm::mat4 model_mat = this->GetTransformMatrix();

		return model_mat * glm::vec4(pModelCenter, 1.0f);
	}



	glm::mat4 Joint::GetTransformMatrix() const
	{
		const glm::mat4 joint_rotation = Rotations::GetRotationMatrix(this->mAxis);
		const glm::vec3 pos_offset = Rotations::GetOffsetPosition(this->mAxis);

		glm::mat4 model_matrix(1.0f);
		model_matrix *= glm::translate(this->mPosition + pos_offset);
		model_matrix *= joint_rotation;
		model_matrix *= glm::translate(pParent->m_bounds / 2.0f);

		return model_matrix;
	}
}