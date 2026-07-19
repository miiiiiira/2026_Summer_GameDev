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

	// 画像座標
	Vector2 pos;

	// 画像サイズ
	Vector2 size;
};

namespace MouseSelectTypeTable
{
	enum MOUSE_SELECT_TYPE
	{
		RIGHT_ARROW,// 右矢印
		LEFT_ARROW,// 左矢印
		GAME_START,// ゲームスタート

		MAX,
		NON
	};

	extern const std::unordered_map<MOUSE_SELECT_TYPE, selectTypeData> Table;
}
