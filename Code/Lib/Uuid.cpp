#include "Uuid.h"

#include "Lib\String.h"

#include <iomanip>
#include <sstream>

namespace SMBC
{
	void Uuid::FromString(const std::string& str)
	{
		if (str.size() != 36) //if the string size doesn't match, then create a null uuid
		{
			std::memset(&m_data8x16, 0, 16);
			return;
		}

		std::string lUuidCopy = str;

		std::size_t lIdx = 0;
		while ((lIdx = lUuidCopy.find('-')) != std::string::npos)
			lUuidCopy.erase(lUuidCopy.begin() + lIdx);

		for (std::size_t a = 0; a < 16; a++)
			m_data8x16[a] = (unsigned char)std::stoi(lUuidCopy.substr(a * 2, 2), nullptr, 16);
	}

	Uuid::Uuid()
	{
		std::memset(&m_data8x16, 0, 16);
	}

	Uuid::Uuid(const std::string& uuid)
	{
		this->FromString(uuid);
	}

	std::string Uuid::ToString() const
	{
		std::stringstream l_stream;

		l_stream << std::hex << std::setfill('0')
			<< std::setw(2) << (int)m_data8x16[0]
			<< std::setw(2) << (int)m_data8x16[1]
			<< std::setw(2) << (int)m_data8x16[2]
			<< std::setw(2) << (int)m_data8x16[3]
			<< '-'
			<< std::setw(2) << (int)m_data8x16[4]
			<< std::setw(2) << (int)m_data8x16[5]
			<< '-'
			<< std::setw(2) << (int)m_data8x16[6]
			<< std::setw(2) << (int)m_data8x16[7]
			<< '-'
			<< std::setw(2) << (int)m_data8x16[8]
			<< std::setw(2) << (int)m_data8x16[9]
			<< '-'
			<< std::setw(2) << (int)m_data8x16[10]
			<< std::setw(2) << (int)m_data8x16[11]
			<< std::setw(2) << (int)m_data8x16[12]
			<< std::setw(2) << (int)m_data8x16[13]
			<< std::setw(2) << (int)m_data8x16[14]
			<< std::setw(2) << (int)m_data8x16[15];

		return l_stream.str();
	}

	std::wstring Uuid::ToWstring() const
	{
		return String::ToWide(this->ToString());
	}

	std::size_t Uuid::Hash() const
	{
		using __uint64 = unsigned __int64;

		return (std::hash<__uint64>{}(m_data64x2[0]) >> 1) ^ (std::hash<__uint64>{}(m_data64x2[1]) << 2);
	}

	bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept
	{
		return (
			lhs.m_data64x2[0] == rhs.m_data64x2[0] &&
			lhs.m_data64x2[1] == rhs.m_data64x2[1]
		);
	}

	bool operator!=(const Uuid& lhs, const Uuid& rhs) noexcept
	{
		return (
			lhs.m_data64x2[0] != rhs.m_data64x2[0] ||
			lhs.m_data64x2[1] != rhs.m_data64x2[1]
		);
	}

	bool operator< (const Uuid& lhs, const Uuid& rhs) noexcept
	{
		return (
			lhs.m_data64x2[0] < rhs.m_data64x2[0] ||
			lhs.m_data64x2[1] < rhs.m_data64x2[1]
		);
	}
}