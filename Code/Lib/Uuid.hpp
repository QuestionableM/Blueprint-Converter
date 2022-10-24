#pragma once

#include <iomanip>
#include <sstream>
#include <string>

namespace SMBC
{
	class Uuid
	{
	public:
		inline Uuid() { std::memset(m_data8x16, 0, 16); }

		inline Uuid(const std::string& uuid) { this->FromString<std::string>(uuid); }
		inline Uuid(const std::wstring& uuid) { this->FromString<std::wstring>(uuid); }

		inline std::string ToString() const
		{
			std::stringstream l_stream;
			const auto v_setw = std::setw(2);

			l_stream << std::hex << std::setfill('0')
				<< v_setw << static_cast<int>(m_data8x16[0])
				<< v_setw << static_cast<int>(m_data8x16[1])
				<< v_setw << static_cast<int>(m_data8x16[2])
				<< v_setw << static_cast<int>(m_data8x16[3])
				<< '-'
				<< v_setw << static_cast<int>(m_data8x16[4])
				<< v_setw << static_cast<int>(m_data8x16[5])
				<< '-'
				<< v_setw << static_cast<int>(m_data8x16[6])
				<< v_setw << static_cast<int>(m_data8x16[7])
				<< '-'
				<< v_setw << static_cast<int>(m_data8x16[8])
				<< v_setw << static_cast<int>(m_data8x16[9])
				<< '-'
				<< v_setw << static_cast<int>(m_data8x16[10])
				<< v_setw << static_cast<int>(m_data8x16[11])
				<< v_setw << static_cast<int>(m_data8x16[12])
				<< v_setw << static_cast<int>(m_data8x16[13])
				<< v_setw << static_cast<int>(m_data8x16[14])
				<< v_setw << static_cast<int>(m_data8x16[15]);

			return l_stream.str();
		}

		inline std::wstring ToWstring() const
		{
			std::wstringstream v_stream;
			const auto v_setw = std::setw(2);

			v_stream << std::hex << std::setfill(L'0')
				<< v_setw << static_cast<int>(m_data8x16[0])
				<< v_setw << static_cast<int>(m_data8x16[1])
				<< v_setw << static_cast<int>(m_data8x16[2])
				<< v_setw << static_cast<int>(m_data8x16[3])
				<< L'-'
				<< v_setw << static_cast<int>(m_data8x16[4])
				<< v_setw << static_cast<int>(m_data8x16[5])
				<< L'-'
				<< v_setw << static_cast<int>(m_data8x16[6])
				<< v_setw << static_cast<int>(m_data8x16[7])
				<< L'-'
				<< v_setw << static_cast<int>(m_data8x16[8])
				<< v_setw << static_cast<int>(m_data8x16[9])
				<< L'-'
				<< v_setw << static_cast<int>(m_data8x16[10])
				<< v_setw << static_cast<int>(m_data8x16[11])
				<< v_setw << static_cast<int>(m_data8x16[12])
				<< v_setw << static_cast<int>(m_data8x16[13])
				<< v_setw << static_cast<int>(m_data8x16[14])
				<< v_setw << static_cast<int>(m_data8x16[15]);

			return v_stream.str();
		}

		inline std::size_t Hash() const
		{
			return (std::hash<unsigned __int64>{}(m_data64x2[0]) >> 1) ^ (std::hash<unsigned __int64>{}(m_data64x2[1]) << 2);
		}

	private:
		template<typename T>
		inline constexpr void FromString(const T& uuid_str)
		{
			static_assert(std::_Is_any_of_v<T, std::string, std::wstring>, "You can only pass std::string and std::wstring to Uuid::FromString function!");

			if (uuid_str.size() != 36)
			{
				std::memset(m_data8x16, 0, 16);
				return;
			}

			m_data8x16[0] = static_cast<unsigned char>(std::stoi(uuid_str.substr(0, 2), nullptr, 16));
			m_data8x16[1] = static_cast<unsigned char>(std::stoi(uuid_str.substr(2, 2), nullptr, 16));
			m_data8x16[2] = static_cast<unsigned char>(std::stoi(uuid_str.substr(4, 2), nullptr, 16));
			m_data8x16[3] = static_cast<unsigned char>(std::stoi(uuid_str.substr(6, 2), nullptr, 16));

			m_data8x16[4] = static_cast<unsigned char>(std::stoi(uuid_str.substr(9, 2), nullptr, 16));
			m_data8x16[5] = static_cast<unsigned char>(std::stoi(uuid_str.substr(11, 2), nullptr, 16));

			m_data8x16[6] = static_cast<unsigned char>(std::stoi(uuid_str.substr(14, 2), nullptr, 16));
			m_data8x16[7] = static_cast<unsigned char>(std::stoi(uuid_str.substr(16, 2), nullptr, 16));

			m_data8x16[8] = static_cast<unsigned char>(std::stoi(uuid_str.substr(19, 2), nullptr, 16));
			m_data8x16[9] = static_cast<unsigned char>(std::stoi(uuid_str.substr(21, 2), nullptr, 16));

			m_data8x16[10] = static_cast<unsigned char>(std::stoi(uuid_str.substr(24, 2), nullptr, 16));
			m_data8x16[11] = static_cast<unsigned char>(std::stoi(uuid_str.substr(26, 2), nullptr, 16));
			m_data8x16[12] = static_cast<unsigned char>(std::stoi(uuid_str.substr(28, 2), nullptr, 16));
			m_data8x16[13] = static_cast<unsigned char>(std::stoi(uuid_str.substr(30, 2), nullptr, 16));
			m_data8x16[14] = static_cast<unsigned char>(std::stoi(uuid_str.substr(32, 2), nullptr, 16));
			m_data8x16[15] = static_cast<unsigned char>(std::stoi(uuid_str.substr(34, 2), nullptr, 16));
		}

		friend bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept;
		friend bool operator!=(const Uuid& lhs, const Uuid& rhs) noexcept;

		union
		{
			unsigned __int8 m_data8x16[16];
			unsigned __int16 m_data16x8[8];
			unsigned __int32 m_data32x4[4];
			unsigned __int64 m_data64x2[2];
		};
	};

	inline bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept
	{
		return (
			lhs.m_data64x2[0] == rhs.m_data64x2[0] &&
			lhs.m_data64x2[1] == rhs.m_data64x2[1]
		);
	}

	inline bool operator!=(const Uuid& lhs, const Uuid& rhs) noexcept
	{
		return (
			lhs.m_data64x2[0] != rhs.m_data64x2[0] ||
			lhs.m_data64x2[1] != rhs.m_data64x2[1]
		);
	}
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