#include "ItemInfo.h"

namespace ItemTable_Stage1
{
	const std::unordered_map<Tag, std::string> Table =
	{
		{ Tag::Goblet, "Data/Model/Item/Goblet.mv1" },
		{ Tag::Potion, "Data/Model/Item/Potion_Blue.mv1" },
		{ Tag::Amphora, "Data/Model/Item/Amphora.mv1" },
		{ Tag::Bottle, "Data/Model/Item/Bottle.mv1" },
		{ Tag::Jar, "Data/Model/Item/Jar_Big.mv1"},
		{ Tag::Mug, "Data/Model/Item/Mug.mv1"},
		{ Tag::Skull,"Data/Model/Item/Skull.mv1" },
	};
}