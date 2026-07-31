#pragma once
#include <unordered_map>

enum FONT_SIZE
{
	FONT_SIZE_16,
	FONT_SIZE_20,
	FONT_SIZE_21,
	FONT_SIZE_30,
	FONT_SIZE_60,

	FONT_MAX
};

namespace FontTable
{
	extern const std::unordered_map<FONT_SIZE, int> Table;
}