#pragma once
#include <string>
#include <unordered_map>
#include "../../../../Scene/GameScene/GameScene.h"

struct MapData
{
	// 地図画像のファイルパス
	std::string path;

	// 地図画像のサイズ
	int mapImgSizeX;
	int mapImgSizeY;

	// 地図上でのプレイヤーの初期位置
	int playerSpawnPosX;
	int playerSpawnPosY;
};

namespace MapTable
{
	extern const std::unordered_map<STAGE_NUM, MapData> Table;
}