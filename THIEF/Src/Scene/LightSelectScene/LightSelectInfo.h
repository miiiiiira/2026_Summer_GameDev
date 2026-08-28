#pragma once
#include <string>
#include <unordered_map>
#include "../../Object/Component/Wisp/LightInfo.h"
#include "../../Common/Math/Vector2.h"

namespace LightSelectSceneWispTable
{
	extern const std::unordered_map<LIGHT_TYPE, std::string> Table;
}

struct selectTypeData
{
	// 画像パス
	std::string path;
	std::string framePath;
	
	Vector2 pos;	// 画像座標
	
	Vector2 size;	// 画像サイズ
};

namespace LightSelectTypeTable
{
	enum SELECT_TYPE
	{
		RIGHT_ARROW,// 右矢印
		LEFT_ARROW,	// 左矢印
		GAME_START,	// ゲームスタート

		MAX,
		NON
	};

	extern const std::unordered_map<SELECT_TYPE, selectTypeData> Table;
}
