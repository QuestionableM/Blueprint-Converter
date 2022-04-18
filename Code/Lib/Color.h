#pragma once

#include <string>

namespace SMBC
{
	class Color
	{
		unsigned char bytes[3];

		void SetColorInternal(const std::string& color);

	public:
		Color(const std::string& color);
		Color(const unsigned char& r, const unsigned char& g, const unsigned char& b);
		Color() = default;

		void operator=(const std::string& color);

		std::string String() const;
		std::string StringNormalized() const;
		std::string StringHex() const;
	};
}