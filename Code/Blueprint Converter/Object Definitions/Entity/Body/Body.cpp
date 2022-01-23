#include "Body.h"

#include "Blueprint Converter/Convert Settings/ConvertSettings.h"

#include <fstream>

namespace SMBC
{
	Body::Body(const std::string& mBodyName)
	{
		this->mBodyName = mBodyName;
	}

	Body::~Body()
	{
		for (Entity* pEntity : mObjects)
			delete pEntity;
	}


	EntityType Body::Type() const
	{
		return EntityType::Body;
	}

	std::string Body::GetMtlName(const std::wstring& mat_name, const std::size_t& mIdx) const
	{
		return "NOT_IMPLEMENTED_FOR_BODIES";
	}

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

	std::wstring Body::GetName() const
	{
		return L"Body_" + std::to_wstring(this->mIndex);
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

	void Body::Add(Entity* mEntity)
	{
		mObjects.push_back(mEntity);
	}
}