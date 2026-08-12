#include "MapInfo.h"

namespace MapTable
{
	const std::unordered_map<STAGE_NUM, MapData> Table =
	{
		{ STAGE_NUM::STAGE_1,	{"Data/Image/GameScene/Map.png"	,843,1024,610,982}},
		{ STAGE_NUM::STAGE_2,	{"Data/Image/GameScene/Map2.png" ,938,719,868,599 }},
		{ STAGE_NUM::STAGE_3,	{"Data/Image/GameScene/Map3.png" ,905,916,132,823} },
	};
}