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
	void Entity::WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const
	{
		const glm::mat4 model_matrix = this->GetTransformMatrix();

		pModel->WriteToFile(file, model_matrix, mOffset, this);

		ConvData::ProgressValue++;
	}
}