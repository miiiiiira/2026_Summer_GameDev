#pragma once

#include <DxLib.h>
#include <unordered_map>
#include <string>
#include "../../Tag.h"

enum ITEM_SIZE
{
	BIG,
	MEDIUM,
	SMALL
};

struct ItemInfo
{
	// モデルのハンドル
	int modelId_;

	// 大きさ
	VECTOR scale_;

	// 離された時の座標
	VECTOR grabbedPos_;

	// プレイヤーとの相対座標
	VECTOR localPos_;

	// 初期位置
	VECTOR defaultPos_;

	// 金額(HP)
	int price_;

	// アイテムの頑丈さ
	int hardness_;

	// アイテムの大きさ
	ITEM_SIZE size_;

	// 重力
	VECTOR velocity_;

	// 重さ
	float weight_;

	// 当たり判定用半径
	float collisionRadiusX_;
	float collisionRadiusY_;

	// 生存フラグ	true / 生存中にする , false / 生存している
	bool isAlive_;

	// 掴まれているか　true / 掴まれている, false / 掴まれていない
	bool isGrabbed_;

	// 空中から1度でもステージに接触したか　true / 接触した , false / 接触していない
	bool hasTouchedStage_;

	// 納品場所に入っているか	true / 入っている , false / 入っていない
	bool hasTouchedDeliveryLocation_;

	// カートに入っているか	true / 入っている , false / 入っていない
	bool hasTouchedCart_;

	// 一度でも発見したか　true / 見つけた , false / 見つけてない
	bool isFound_;

	// 発見した物を一定時間ハイライトさせる用のカウンター
	int foundCounter_;

	// 無敵時間
	int invincibilityFrames_;
};


struct ItemData
{
	std::string path; // モデルのパス
	float posOffsetY; // Y座標のオフセット
	ITEM_SIZE size;
};

namespace ItemTable
{
	extern const std::unordered_map<Tag,ItemData> Table;
}
