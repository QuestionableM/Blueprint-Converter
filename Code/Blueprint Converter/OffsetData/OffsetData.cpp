#include "OffsetData.h"

namespace SMBC
{
	void OffsetData::UpdateValues(const glm::vec3& val)
	{
		this->pt_sum += val;
		this->pt_count++;
	}
}