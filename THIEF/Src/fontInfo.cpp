#include "fontInfo.h"

namespace FontTable
{
	const std::unordered_map<FONT_SIZE, int> Table =
	{
		{ FONT_SIZE::FONT_SIZE_16,	16},
		{ FONT_SIZE::FONT_SIZE_20,	20},
		{ FONT_SIZE::FONT_SIZE_21,	21},
		{ FONT_SIZE::FONT_SIZE_30,	30},
		{ FONT_SIZE::FONT_SIZE_60,	60}
	};
}