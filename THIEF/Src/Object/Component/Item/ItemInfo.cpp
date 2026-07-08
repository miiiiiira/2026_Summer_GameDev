#include "ItemInfo.h"
#include "Goblet/Goblet.h"
#include "Potion/Potion.h"
#include "Amphora/Amphora.h"
#include "Bottle/Bottle.h"
#include "Jar/Jar.h"
#include "Mug/Mug.h"
#include "Skull/Skull.h"

namespace ItemTable
{
	const std::unordered_map<Tag, ItemData> Table =
	{
		{ Tag::Item_Goblet,			{"Data/Model/Item/Goblet.mv1" ,Goblet::COLLISION_RADIUS_Y + 13.0f,ITEM_SIZE::SMALL}},
		{ Tag::Item_Potion_Red,		{"Data/Model/Item/Potion_Red.mv1" ,Potion::COLLISION_RADIUS_Y + 13.0f,ITEM_SIZE::SMALL} },
		{ Tag::Item_Potion_Green,	{"Data/Model/Item/Potion_Green.mv1" ,Potion::COLLISION_RADIUS_Y + 13.0f,ITEM_SIZE::SMALL} },
		{ Tag::Item_Potion_Blue,	{"Data/Model/Item/Potion_Blue.mv1" ,Potion::COLLISION_RADIUS_Y + 13.0f,ITEM_SIZE::SMALL} },
		{ Tag::Item_Amphora,		{"Data/Model/Item/Amphora.mv1" ,Amphora::COLLISION_RADIUS_Y + 35.0f,ITEM_SIZE::BIG} },
		{ Tag::Item_Bottle,			{"Data/Model/Item/Bottle.mv1" ,Bottle::COLLISION_RADIUS_Y + 13.0f,ITEM_SIZE::MEDIUM} },
		{ Tag::Item_Jar,			{"Data/Model/Item/Jar.mv1",Jar::COLLISION_RADIUS_Y + 13.0f,ITEM_SIZE::MEDIUM} },
		{ Tag::Item_Mug,			{"Data/Model/Item/Mug.mv1",Mug::COLLISION_RADIUS_Y + 13.0f,ITEM_SIZE::SMALL} },
		{ Tag::Item_Skull,			{"Data/Model/Item/Skull.mv1" ,Skull::COLLISION_RADIUS_Y + 13.0f,ITEM_SIZE::MEDIUM} }
	};
}
