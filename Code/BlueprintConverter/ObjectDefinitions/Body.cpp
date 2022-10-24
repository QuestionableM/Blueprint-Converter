#include "Body.hpp"

#include "BlueprintConverter\ConvertSettings.hpp"

#include <fstream>

namespace SMBC
{
	void Body::FillTextureMap(std::unordered_map<std::string, ObjectTextureData>& tex_map) const
	{
		for (Entity* pEntity : mObjects)
			pEntity->FillTextureMap(tex_map);
	}

	void Body::FillTextureJson(nlohmann::json& mJson) const
	{
		for (Entity* pEntity : mObjects)
			pEntity->FillTextureJson(mJson);
	}

	glm::vec3 Body::GetCenterPoint() const
	{
		glm::vec3 pCenter(0.0f);

		for (Entity* pEntity : mObjects)
			pCenter += pEntity->GetCenterPoint();

		return pCenter / static_cast<float>(mObjects.size());
	}



	std::size_t Body::GetAmountOfObjects() const
	{
		std::size_t final_sum = 0;

		for (Entity* pEntity : mObjects)
			final_sum += pEntity->GetAmountOfObjects();

		return final_sum;
	}

	void Body::WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const
	{
		if (ConvertSettings::SeparationMethod != 0)
		{
			const std::string cName = "\no " + this->mBodyName;
			file.write(cName.c_str(), cName.size());
		}

		for (Entity* pEntity : mObjects)
			pEntity->WriteObjectToFile(file, mOffset);
	}
}