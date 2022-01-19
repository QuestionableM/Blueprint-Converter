#include "Part.h"

#include "Blueprint Converter/Object Definitions/Model/Model.h"
#include "Object Database/Rotations/ObjectRotations.hpp"

#include <gtx/transform.hpp>

namespace SMBC
{
	Part::Part(const PartData* pParent, Model* pModel, Body* pBody, const glm::vec3& position, const AxisData& mAxisData, const Color& mColor, const std::size_t& obj_idx)
	{
		this->pParent = pParent;
		this->pModel = pModel;
		this->pBody = pBody;
		this->mPosition = position;
		this->mAxis = mAxisData;
		this->mColor = mColor;
		this->mIndex = obj_idx;
	}

	EntityType Part::Type() const
	{
		return EntityType::Part;
	}

	std::string Part::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
	{
		return pParent->Uuid.ToString() + " " + mColor.StringHex() + " " + std::to_string(mIdx + 1);
	}

	void Part::FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const
	{
		const std::string mtl_first_part = pParent->Uuid.ToString() + " " + mColor.StringHex() + " ";
		for (std::size_t a = 0; a < pModel->subMeshData.size(); a++)
		{
			const SubMeshData* pSubMesh = pModel->subMeshData[a];
			const std::wstring tex_name = pParent->TextureList.PickString(a, pSubMesh->MaterialName);

			ObjectTextureData oTexData;
			if (pParent->TextureList.GetTextureByName(tex_name, oTexData.mTextures))
			{
				oTexData.mColor = mColor;

				const std::string mtl_name = mtl_first_part + std::to_string(a + 1);

				if (tex_map.find(mtl_name) != tex_map.end())
					continue;

				tex_map.insert(std::make_pair(mtl_name, oTexData));
			}
		}
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