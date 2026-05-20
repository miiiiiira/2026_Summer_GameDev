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

	// 前回座標
	VECTOR prevPos_;

	// アイテムの種類
	ITEM_TAG itemTag_;

	// 金額(アイテムのHP)
	int money_;

	// アイテムの耐久値
	float durability_;

	// 重力
	VECTOR velocity_;

	// 当たり判定用半径
	float collisionRadius_;

	// 当たり判定のカプセル用ずらし数
	float collisionOffset_;

	// プレイヤーとの相対座標
	VECTOR localPos_;

	// 生存フラグ	true / 生存中にする , false / 生存している
	bool isAlive_;

	// 掴まれているか　true / 掴まれている, false / 掴まれていない
	bool isGrabbed;
};