#include "Uuid.h"

#include "Lib/String/String.h"

namespace SMBC
{
	Uuid::Uuid(const std::string& uuid)
	{
		this->data = uuid;
	}

	void Uuid::operator=(const std::string& uuid)
	{
		this->data = uuid;
	}

	std::string Uuid::ToString() const
	{
		return this->data;
	}

	std::wstring Uuid::ToWstring() const
	{
		return String::ToWide(this->data);
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