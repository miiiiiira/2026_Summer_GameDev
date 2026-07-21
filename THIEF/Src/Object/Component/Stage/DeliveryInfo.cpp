#include "DeliveryInfo.h"

namespace DeliveryTable
{
	const std::unordered_map<STAGE_NUM, DeliveryData> Table =
	{
		{ STAGE_NUM::STAGE_1,	{{255.0f,220.0f,285.0f } ,{-922.0f,220.0f,1924.0f } ,{-1148.0f,148.0f,2264.0f }}},
		{ STAGE_NUM::STAGE_2,	{{230.0f,220.0f,240.0f } ,{-6759.0f,220.0f,-851.0f } ,{-7072.0f,172.0f,-442.0f}}},
		{ STAGE_NUM::STAGE_3,	{{-922.0f,220.0f,1924.0f } ,{-922.0f,220.0f,1924.0f } ,{-1116.0f,150.0f,2255.0f }}}
	};
}