#include "Entity.h"

#include "Blueprint Converter/Convert Settings/ConvertSettings.h"
#include "Blueprint Converter/Object Definitions/Model/Model.h"
#include "Blueprint Converter/BlueprintConverter.h"

#include "Lib/String/String.h"
#include "Lib/OtherFunc/OtherFunc.h"
#include "Lib/ConvData/ConvData.h"

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
}