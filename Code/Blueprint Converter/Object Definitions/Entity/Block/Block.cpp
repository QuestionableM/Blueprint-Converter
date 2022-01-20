#include "Block.h"

#include "Object Database/Rotations/ObjectRotations.hpp"
#include "Blueprint Converter/Object Definitions/Model/BlockModel.h"

#include "Lib/ConvData/ConvData.h"
#include "Lib/String/String.h"

#include <gtx/transform.hpp>

namespace SMBC
{
	Block::Block(
		const BlockData* pParent,
		const Body* pBody,
		const glm::vec3& position,
		const glm::vec3& size,
		const AxisData& mAxisData,
		const Color& mColor,
		const std::size_t& obj_idx)
	{
		this->pParent = pParent;
		this->pBody = pBody;
		this->mPosition = position;
		this->mScale = size;
		this->mAxis = mAxisData;
		this->mColor = mColor;
		this->mIndex = obj_idx;
	}

	EntityType Block::Type() const
	{
		return EntityType::Block;
	}

	std::string Block::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
	{
		return pParent->Uuid.ToString() + " " + mColor.StringHex() + " 1";
	}

	void Block::FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const
	{
		const std::string mtl_name = this->GetMtlName(L"", 0);

		if (tex_map.find(mtl_name) != tex_map.end())
			return;

		ObjectTextureData oTexData;
		oTexData.mTextures = pParent->TextureList;
		oTexData.mColor = this->mColor;

		tex_map.insert(std::make_pair(mtl_name, oTexData));
	}

	void Block::FillTextureJson(nlohmann::json& mJson) const
	{
		const std::string mNameStr = String::ToUtf8(this->GetName());

		if (mJson.find(mNameStr) == mJson.end())
			mJson[mNameStr] = pParent->TextureList.ToJson();
	}

	std::wstring Block::GetName() const
	{
		return pParent->Name + L" (" + pParent->ModPtr->Name + L": " + pParent->Uuid.ToWstring() + L")";
	}



	glm::mat4 Block::GetTransformMatrix() const
	{
		const glm::mat4 block_matrix = Rotations::GetRotationMatrix(this->mAxis);

		glm::mat4 transform_matrix(1.0f);
		transform_matrix *= glm::translate(this->mPosition);
		transform_matrix *= block_matrix;
		transform_matrix *= glm::translate(this->mScale / 2.0f);

		return transform_matrix;
	}

	void Block::WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const
	{
		const glm::mat4 block_matrix = this->GetTransformMatrix();

		Model new_block(L"BLOCK_INTERNAL");
		BlockModel::CreateBlockModel(new_block, this->mPosition, this->mScale / 2.0f, pParent->Tiling);

		new_block.WriteToFile(file, block_matrix, mOffset, this);

		ConvData::ProgressValue++;
	}
}