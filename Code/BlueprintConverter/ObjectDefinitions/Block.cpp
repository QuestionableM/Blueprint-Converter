#include "Block.hpp"

#include "BlueprintConverter\ObjectDefinitions\BlockModel.hpp"
#include "BlueprintConverter\ConvertSettings.hpp"
#include "BlueprintConverter\OffsetData.hpp"

#include "ObjectDatabase\ObjectRotations.hpp"
#include "ObjectDatabase\MaterialManager.hpp"

#include "Lib\ConvData.hpp"
#include "Lib\String.hpp"

#include "DebugCon.hpp"

#include <gtx\transform.hpp>

namespace SMBC
{
	std::string Block::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
	{
		std::string out_str = pParent->m_uuid.ToString();

		if (ConvertSettings::MatByColor)
			out_str.append(" " + mColor.StringHex());

		const std::string mat_idx = MaterialManager::GetMaterialA(pParent->m_textureList->material);
		out_str.append(" 1 " + mat_idx);

		return out_str;
	}

	void Block::FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const
	{
		const std::string mtl_name = this->GetMtlName(L"", 0);

		if (tex_map.find(mtl_name) != tex_map.end())
			return;

		ObjectTextureData oTexData;
		oTexData.mTextures = pParent->m_textureList;
		oTexData.mColor = this->mColor;

		tex_map.insert(std::make_pair(mtl_name, oTexData));
	}

	void Block::FillTextureJson(nlohmann::json& mJson) const
	{
		const std::string mNameStr = String::ToUtf8(this->GetName());

		if (mJson.find(mNameStr) == mJson.end())
			mJson[mNameStr] = pParent->m_textureList->ToJson();

		ConvData::ProgressValue++;
	}

	glm::vec3 Block::GetCenterPoint() const
	{
		Model new_block(L"BLOCK_INTERNAL");
		BlockModel::CreateBlockModel(new_block, this->mPosition, this->mScale / 2.0f, pParent->m_tiling);

		const glm::mat4 model_mat = this->GetTransformMatrix();

		return model_mat * glm::vec4(new_block.mCenterPoint, 1.0f);
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
		BlockModel::CreateBlockModel(new_block, this->mPosition, this->mScale / 2.0f, pParent->m_tiling);

		new_block.WriteToFile(file, block_matrix, mOffset, this);

		ConvData::ProgressValue++;
	}
}