#include "ItemInfo.h"

namespace ItemTable_Stage1
{
	const std::unordered_map<Tag, std::string> Table =
	{
		{ Tag::Item_Goblet, "Data/Model/Item/Goblet.mv1" },
		{ Tag::Item_Potion_Red, "Data/Model/Item/Potion_Red.mv1" },
		{ Tag::Item_Potion_Green, "Data/Model/Item/Potion_Green.mv1" },
		{ Tag::Item_Potion_Blue, "Data/Model/Item/Potion_Blue.mv1" },
		{ Tag::Item_Amphora, "Data/Model/Item/Amphora.mv1" },
		{ Tag::Item_Bottle, "Data/Model/Item/Bottle.mv1" },
		{ Tag::Item_Jar, "Data/Model/Item/Jar_Big.mv1"},
		{ Tag::Item_Mug, "Data/Model/Item/Mug.mv1"},
		{ Tag::Item_Skull,"Data/Model/Item/Skull.mv1" },
	};
}