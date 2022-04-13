#include "Entity.h"

#include "BlueprintConverter\ObjectDefinitions\Model.h"
#include "BlueprintConverter\BlueprintConverter.h"
#include "BlueprintConverter\ConvertSettings.h"

#include "Lib\ConvData.h"
#include "Lib\String.h"

#include <glm.hpp>
#include <gtx/transform.hpp>

namespace SMBC
{
	glm::mat4 Entity::GetTransformMatrix() const
	{
		return glm::mat4(1.0f);
	}

	std::size_t Entity::GetAmountOfObjects() const
	{
		return 1;
	}

	void Entity::WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const
	{
		const glm::mat4 model_matrix = this->GetTransformMatrix();

		pModel->WriteToFile(file, model_matrix, mOffset, this);

		ConvData::ProgressValue++;
	}

	std::size_t Entity::GetIndex() const
	{
		return this->mIndex;
	}

	SMBC::Color Entity::GetColor() const
	{
		return this->mColor;
	}

	SMBC::Uuid Entity::GetUuid() const
	{
		return SMBC::Uuid();
	}
}