#include "Uuid.h"

#include "Lib/String/String.h"

namespace SMBC
{
	void Uuid::StringToUuid(const std::string& str)
	{
		this->data = uuids::uuid::from_string(str);
	}

	Uuid::Uuid(const std::string& uuid)
	{
		this->StringToUuid(uuid);
	}

	void Uuid::operator=(const std::string& uuid)
	{
		this->StringToUuid(uuid);
	}

	std::string Uuid::ToString() const
	{
		return uuids::to_string(this->data);
	}

	std::wstring Uuid::ToWstring() const
	{
		return uuids::to_wstring(this->data);
	}

	bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept
	{
		return (lhs.data == rhs.data);
	}

	bool operator!=(const Uuid& lhs, const Uuid& rhs) noexcept
	{
		return !(lhs == rhs);
	}
}