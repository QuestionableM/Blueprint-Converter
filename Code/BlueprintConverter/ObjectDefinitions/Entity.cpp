#include "Entity.hpp"

#include "BlueprintConverter\ObjectDefinitions\Model.hpp"
#include "BlueprintConverter\BlueprintConverter.hpp"
#include "BlueprintConverter\ConvertSettings.hpp"

#include "Lib\ConvData.hpp"
#include "Lib\String.hpp"

#include <gtx\transform.hpp>
#include <glm.hpp>

namespace SMBC
{
	void Entity::WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const
	{
		const glm::mat4 model_matrix = this->GetTransformMatrix();

		pModel->WriteToFile(file, model_matrix, mOffset, this);

		ConvData::ProgressValue++;
	}
}