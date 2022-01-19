#include "Body.h"

namespace SMBC
{
	Body::Body(const std::size_t& mIndex)
	{
		this->mIndex = mIndex;
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

	std::size_t Body::GetAmountOfObjects() const
	{
		std::size_t final_sum = 0;

		for (Entity* pEntity : mObjects)
			final_sum += pEntity->GetAmountOfObjects();

		return final_sum;
	}

	void Body::WriteObjectToFile(std::ofstream& file, OffsetData& mOffset) const
	{
		for (Entity* pEntity : mObjects)
			pEntity->WriteObjectToFile(file, mOffset);
	}

	void Body::Add(Entity* mEntity)
	{
		mObjects.push_back(mEntity);
	}
}