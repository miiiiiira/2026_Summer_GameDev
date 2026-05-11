#pragma once

#include <DxLib.h>

enum class ITEM_TAG
{
	GOBLET,
	JEWELRY,

	MAX
};


struct ItemInfo
{
	// モデルのハンドル
	int modelId_;

	// 大きさ
	VECTOR scale_;

	// 向き
	VECTOR angle_;

	// 座標
	VECTOR pos_;

	// アイテムの種類
	ITEM_TAG itemTag_;

	// アイテムの金額
	int money_;

	// 重力
	float gravity_;

	// 重力をかけるか true / 重力をかける , false / 重力をかけない
	bool isGravity_;

	// 生存フラグ	true / 生存していない , false / 生存している
	bool isAlive_;
};