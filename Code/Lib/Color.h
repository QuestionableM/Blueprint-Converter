#pragma once

#include <sstream>
#include <iomanip>
#include <string>

namespace SMBC
{
	class Color
	{
	public:
		inline Color(const std::string& color) { this->FromString<std::string>(color); }
		inline Color(const std::wstring& color) { this->FromString<std::wstring>(color); }

		inline Color(const unsigned char& r, const unsigned char& g, const unsigned char& b)
		{
			m_bytes[0] = r;
			m_bytes[1] = g;
			m_bytes[2] = b;
		}

		Color() = default;

		inline void operator=(const std::string& color) { this->FromString<std::string>(color); }
		inline void operator=(const std::wstring& color) { this->FromString<std::wstring>(color); }

		inline std::string String() const
		{
			return std::to_string(m_bytes[0]) + " " + std::to_string(m_bytes[1]) + " " + std::to_string(m_bytes[2]);
		}

		inline std::string StringNormalized() const
		{
			const float v_r_norm = static_cast<float>(m_bytes[0]) / 255.0f;
			const float v_g_norm = static_cast<float>(m_bytes[1]) / 255.0f;
			const float v_b_norm = static_cast<float>(m_bytes[2]) / 255.0f;

			return std::to_string(v_r_norm) + " " + std::to_string(v_g_norm) + " " + std::to_string(v_b_norm);
		}

		inline std::string StringHex() const
		{
			std::stringstream v_sstream;

			const int v_fullInt = m_bytes[2] | (m_bytes[1] << 8) | (m_bytes[0] << 16);
			v_sstream << std::hex << std::setfill('0') << std::setw(6) << v_fullInt;

			return v_sstream.str();
		}

	private:
		template<class T>
		inline void FromString(const T& color)
		{
			static_assert(std::_Is_any_of_v<T, std::string, std::wstring>, "FromString can only be used with the following types: std::string, std::wstring");

			if (color.size() < 6)
				return;

			m_bytes[0] = static_cast<unsigned char>(std::stoi(color.substr(0, 2), nullptr, 16));
			m_bytes[1] = static_cast<unsigned char>(std::stoi(color.substr(2, 2), nullptr, 16));
			m_bytes[2] = static_cast<unsigned char>(std::stoi(color.substr(4, 2), nullptr, 16));
		}

		unsigned char m_bytes[3];
	};
}