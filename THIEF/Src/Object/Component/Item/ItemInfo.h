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

	// 前回座標
	VECTOR prevPos_;

	// 離された時の座標
	VECTOR grabbedPos_;

	// アイテムの種類
	ITEM_TAG itemTag_;

	// 金額(HP)
	int money_;

	// アイテムの頑丈さ
	float hardness_;

	// 重力
	VECTOR velocity_;

	// 当たり判定用半径
	float collisionRadiusX_;
	float collisionRadiusY_;

	// プレイヤーとの相対座標
	VECTOR localPos_;

	// 生存フラグ	true / 生存中にする , false / 生存している
	bool isAlive_;

	// 掴まれているか　true / 掴まれている, false / 掴まれていない
	bool isGrabbed;

	// 空中から1度でもステージに接触したかどうか　true / 接触した , false / 接触していない
	bool hasTouchedStage_;

	// 納品場所に入っているかどうか		true / 入っている , false / 入っていない
	bool hasTouchedDeliveryLocation_;
};