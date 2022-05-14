#pragma once

#include <string>

namespace SMBC
{
	class Uuid
	{
		union
		{
			unsigned __int8 m_data8x16[16];
			unsigned __int32 m_data32x4[4];
			unsigned __int64 m_data64x2[2];
		};

		friend bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept;
		friend bool operator!=(const Uuid& lhs, const Uuid& rhs) noexcept;

		void FromString(const std::string& str);

	public:
		Uuid();
		Uuid(const std::string& uuid);

		std::string  ToString()  const;
		std::wstring ToWstring() const;

		std::size_t Hash() const;
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
			return uuid.Hash();
		}
	};
}