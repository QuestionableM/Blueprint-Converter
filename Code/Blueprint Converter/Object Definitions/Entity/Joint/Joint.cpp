#include "Joint.h"

#include "Blueprint Converter/Object Definitions/Model/Model.h"
#include "Object Database/Rotations/ObjectRotations.hpp"
#include "Lib/String/String.h"

#include <gtx/transform.hpp>

namespace SMBC
{
	Joint::Joint(const PartData* pParent, const Body* pBody, Model* pModel, const glm::vec3& pos, const AxisData& mAxis, const Color& mColor)
	{
		this->pParent = pParent;
		this->pBody = pBody;
		this->pModel = pModel;
		this->mPosition = pos;
		this->mAxis = mAxis;
		this->mColor = mColor;
	}



	EntityType Joint::Type() const
	{
		return EntityType::Joint;
	}

	std::string Joint::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
	{
		return pParent->Uuid.ToString() + " " + mColor.StringHex() + " " + std::to_string(mIdx + 1);
	}

	void Joint::FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const
	{
		const std::string mtl_first_part = pParent->Uuid.ToString() + " " + mColor.StringHex() + " ";
		for (std::size_t a = 0; a < pModel->subMeshData.size(); a++)
		{
			const SubMeshData* pSubMesh = pModel->subMeshData[a];
			const std::wstring tex_name = pParent->TextureList.PickString(a, pSubMesh->MaterialName);

			ObjectTextureData oTexData;
			if (pParent->TextureList.GetEntry(tex_name, oTexData.mTextures))
			{
				oTexData.mColor = this->mColor;

				const std::string mtl_name = mtl_first_part + std::to_string(a + 1);
				
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
			mJson[mNameStr] = pParent->TextureList.ToJson();
	}

	std::wstring Joint::GetName() const
	{
		return pParent->Name + L" (" + pParent->ModPtr->Name + L": " + pParent->Uuid.ToWstring() + L")";
	}



	glm::mat4 Joint::GetTransformMatrix() const
	{
		const glm::mat4 joint_rotation = Rotations::GetRotationMatrix(this->mAxis);
		const glm::vec3 pos_offset = Rotations::GetOffsetPosition(this->mAxis);

		glm::mat4 model_matrix(1.0f);
		model_matrix *= glm::translate(this->mPosition + pos_offset);
		model_matrix *= joint_rotation;
		model_matrix *= glm::translate(pParent->Bounds / 2.0f);

		return model_matrix;
	}
}