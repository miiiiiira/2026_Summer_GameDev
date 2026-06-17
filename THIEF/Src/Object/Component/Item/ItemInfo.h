#pragma once

#include <DxLib.h>
#include <unordered_map>
#include <string>
#include "../../Tag.h"

struct ItemInfo
{
	// モデルのハンドル
	int modelId_;

	// 大きさ
	VECTOR scale_;

	// 向き
	VECTOR angle_;

	// 離された時の座標
	VECTOR grabbedPos_;

	// 金額(HP)
	int price_;

	// アイテムの頑丈さ
	int hardness_;

	// 重力
	VECTOR velocity_;

	// 重さ
	float weight_;

	// 当たり判定用半径
	float collisionRadiusX_;
	float collisionRadiusY_;

	// プレイヤーとの相対座標
	VECTOR localPos_;

	// 生存フラグ	true / 生存中にする , false / 生存している
	bool isAlive_;

	// 掴まれているか　true / 掴まれている, false / 掴まれていない
	bool isGrabbed_;

	// 空中から1度でもステージに接触したかどうか　true / 接触した , false / 接触していない
	bool hasTouchedStage_;

	// 納品場所に入っているかどうか		true / 入っている , false / 入っていない
	bool hasTouchedDeliveryLocation_;

	// 無敵時間
	int invincibilityFrames_;
};

struct ItemData
{
	std::string path; // モデルのパス
	float posOffsetY; // Y座標のオフセット
};


namespace ItemTable_Stage1
{
	extern const std::unordered_map<Tag,ItemData> Table;
}