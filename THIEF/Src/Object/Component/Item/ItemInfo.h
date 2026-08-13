#pragma once

#include <DxLib.h>
#include <unordered_map>
#include <string>
#include "../../Tag.h"

enum ITEM_SIZE
{
	BIG,	// 大きい
	MEDIUM,	// 中くらい
	SMALL	// 小さい
};

struct ItemInfo
{
	int modelId_ = -1;	// モデルのハンドル

	VECTOR scale_ = {};	// 大きさ

	VECTOR grabbedPos_ = {};	// 離された時の座標

	VECTOR localPos_ = {};	// プレイヤーとの相対座標

	VECTOR defaultPos_ = {};	// 初期位置

	int price_ = 0;	// 金額(HP)

	int hardness_ = 0;	// アイテムの頑丈さ

	ITEM_SIZE size_;	// アイテムの大きさ

	VECTOR velocity_ = {};	// 重力

	float weight_ = 0.0f;	// 重さ
	
	float collisionRadiusX_ = 0.0f;	// 当たり判定用X半径
	float collisionRadiusY_ = 0.0f;	// 当たり判定用Y半径
	
	bool isAlive_ = true;	// 生存フラグ	true / 生存中にする , false / 生存している
	
	bool isGrabbed_ = false;	// 掴まれているか　true / 掴まれている, false / 掴まれていない

	bool hasTouchedStage_ = true;	// 空中から1度でもステージに接触したか　true / 接触した , false / 接触していない

	bool hasTouchedDeliveryLocation_ = false;	// 納品場所に入っているか	true / 入っている , false / 入っていない

	bool hasTouchedCart_ = false;	// カートに入っているか	true / 入っている , false / 入っていない

	bool isFound_ = false;	// 一度でも発見したか　true / 見つけた , false / 見つけてない

	int foundCounter_ = 0;	// 発見した物を一定時間ハイライトさせる用のカウンター
	
	int invincibilityFrames_ = 0;	// 無敵時間
};

struct DamageInfo
{
	VECTOR pos = {};	// 座標
	int damage = 0;		// ダメージ数
	int count = 0;		// ダメージ数表記カウント
};

struct ItemData
{
	std::string path;	// モデルのパス
	float posOffsetY;	// Y座標のオフセット
	ITEM_SIZE size;		// アイテムのサイズ
};

namespace ItemTable
{
	extern const std::unordered_map<Tag,ItemData> Table;
}
