#pragma once

#include "Lib\WinInclude.hpp"

class ConsoleColor
{
	WORD m_Color = 0;

	constexpr ConsoleColor() = default;

	friend constexpr ConsoleColor operator"" _fg(const unsigned __int64) noexcept;
	friend constexpr ConsoleColor operator"" _bg(const unsigned __int64) noexcept;

public:
	inline constexpr operator WORD() const noexcept
	{
		return m_Color;
	}

	inline constexpr ConsoleColor operator|(ConsoleColor rhs) const noexcept
	{
		ConsoleColor l_Output;
		l_Output.m_Color = this->m_Color | rhs.m_Color;

		return l_Output;
	}
};

inline constexpr ConsoleColor operator"" _fg(const unsigned __int64 val) noexcept
{
	ConsoleColor l_Output;

	if ((val & 0b1000) != 0) l_Output.m_Color |= FOREGROUND_RED;
	if ((val & 0b0100) != 0) l_Output.m_Color |= FOREGROUND_GREEN;
	if ((val & 0b0010) != 0) l_Output.m_Color |= FOREGROUND_BLUE;
	if ((val & 0b0001) != 0) l_Output.m_Color |= FOREGROUND_INTENSITY;

	return l_Output;
}

inline constexpr ConsoleColor operator"" _bg(const unsigned __int64 val) noexcept
{
	ConsoleColor l_Output;

	if ((val & 0b1000) != 0) l_Output.m_Color |= BACKGROUND_RED;
	if ((val & 0b0100) != 0) l_Output.m_Color |= BACKGROUND_GREEN;
	if ((val & 0b0010) != 0) l_Output.m_Color |= BACKGROUND_BLUE;
	if ((val & 0b0001) != 0) l_Output.m_Color |= BACKGROUND_INTENSITY;

	return l_Output;
}