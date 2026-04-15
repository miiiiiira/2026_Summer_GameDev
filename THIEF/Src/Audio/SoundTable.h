#pragma once
#include <unordered_map>

#include "SoundInfo.h"

// シーン別サウンドテーブルを宣言しておく
namespace SoundTable_System
{
	extern const std::unordered_map<SoundID, std::string> Table;
}

namespace SoundTable_Title
{
	extern const std::unordered_map<SoundID, std::string> Table;
}

namespace SoundTable_Game
{
	extern const std::unordered_map<SoundID, std::string> Table;
}
