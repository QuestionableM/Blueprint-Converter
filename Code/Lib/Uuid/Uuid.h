#pragma once

#include <stduuid/uuid.h>

namespace SMBC
{
	class Uuid
	{
		uuids::uuid data;

		friend bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept;

		void StringToUuid(const std::string& str);
	public:
		Uuid(const std::string& uuid);
		Uuid() = default;

		void operator=(const std::string& uuid);
		
		std::string  ToString()  const;
		std::wstring ToWstring() const;
	};

	bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept;
	bool operator!=(const Uuid& lhs, const Uuid& rhs) noexcept;
}

namespace std
{
	template<>
	struct hash<SMBC::Uuid>
	{
		using argument_type = SMBC::Uuid;
		using result_type   = std::size_t;

		result_type operator()(argument_type const& uuid) const
		{
			std::hash<std::string> hasher;
			return static_cast<result_type>(hasher(uuid.ToString()));
		}
	};
}