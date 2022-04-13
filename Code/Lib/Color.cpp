#include "Color.h"

#include "Lib\String.h"

#include <sstream>
#include <iomanip>

namespace SMBC
{
	void Color::SetColorInternal(const std::string& color)
	{
		bytes[0] = (unsigned char)std::stoi(color.substr(0, 2), nullptr, 16);
		bytes[1] = (unsigned char)std::stoi(color.substr(2, 2), nullptr, 16);
		bytes[2] = (unsigned char)std::stoi(color.substr(4, 2), nullptr, 16);
	}

	Color::Color(const std::string& color)
	{
		this->SetColorInternal(color);
	}

	Color::Color(const unsigned char& r, const unsigned char& g, const unsigned char& b)
	{
		bytes[0] = r;
		bytes[1] = g;
		bytes[2] = b;
	}

	void Color::operator=(const std::string& color)
	{
		this->SetColorInternal(color);
	}

	std::string Color::String() const
	{
		std::string output;
		String::Combine(output, bytes[0], " ", bytes[1], " ", bytes[2]);

		return output;
	}

	std::string Color::StringNormalized() const
	{
		float norm_r = (float)bytes[0] / 255.0f;
		float norm_g = (float)bytes[1] / 255.0f;
		float norm_b = (float)bytes[2] / 255.0f;

		std::string output;
		String::Combine(output, norm_r, " ", norm_g, " ", norm_b);

		return output;
	}

	std::string Color::StringHex() const
	{
		std::stringstream sstream;

		int full_str = bytes[2] | bytes[1] << 8 | bytes[0] << 16;
		sstream << std::hex << std::setfill('0') << std::setw(6) << full_str;

		return sstream.str();
	}
}