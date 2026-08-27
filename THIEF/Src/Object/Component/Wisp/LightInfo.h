#pragma once
#include <DxLib.h>
#include <string>
#include <unordered_map>

enum LIGHT_TYPE
{
	COLOR_0, // モデルについている既存のテクスチャ色
	COLOR_1,
	COLOR_2,
	COLOR_3,
	COLOR_4,
	COLOR_5,
	COLOR_6,
	COLOR_7,
	COLOR_8,
	COLOR_9,
	COLOR_10,
	COLOR_11,
	COLOR_12,
};

struct LightData
{
	std::string path;	// モデルのパス
	VECTOR color;		// ライト用のカラーコード
};

namespace LightTable
{
	extern const std::unordered_map<LIGHT_TYPE, LightData> Table;
}

