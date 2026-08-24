#pragma once
#include <unordered_map>

#include "SoundInfo.h"

// シーン別サウンドテーブルを宣言しておく
namespace SoundTable_System
{
	extern const std::unordered_map<SoundID, SoundData> Table;
}

namespace SoundTable_Title
{
	extern const std::unordered_map<SoundID, SoundData> Table;
}

namespace SoundTable_MainMenu
{
	extern const std::unordered_map<SoundID, SoundData> Table;
}

namespace SoundTable_LightSelect
{
	extern const std::unordered_map<SoundID, SoundData> Table;
}

namespace SoundTable_Game
{
	extern const std::unordered_map<SoundID, SoundData> Table;
}

namespace SoundTable_StageClear
{
	extern const std::unordered_map<SoundID, SoundData> Table;
}

namespace SoundTable_Shop
{
	extern const std::unordered_map<SoundID, SoundData> Table;
}

namespace SoundTable_GameOver
{
	extern const std::unordered_map<SoundID, SoundData> Table;
}

namespace SoundTable_GameClear
{
	extern const std::unordered_map<SoundID, SoundData> Table;
}