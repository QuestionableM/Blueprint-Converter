#pragma once

namespace SMBC
{
	namespace Bit
	{
		template<typename T>
		inline constexpr void SetBit(T& value, T mask, bool state)
		{
			if (state)
				value |= mask;
			else
				value &= ~mask;
		}

		template<typename T>
		inline constexpr bool GetBit(const T value, T mask)
		{
			return (value & mask) == mask;
		}
	}
}