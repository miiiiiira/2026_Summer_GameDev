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
	VECTOR defaultPos_ = {};	// 初期位置
	VECTOR grabbedPos_ = {};	// 離された時の座標
	VECTOR localPos_ = {};		// プレイヤーとの相対座標

	VECTOR velocity_ = {};	// 重力
	float weight_ = 0.0f;	// 重さ

	// 当たり判定
	float collisionRadiusX_ = 0.0f;	//X半径
	float collisionRadiusY_ = 0.0f;	//Y半径

	// アイテム個々のパラメータ
	int price_ = 0;		// 金額(HP)
	int hardness_ = 0;	// アイテムの頑丈さ
	ITEM_SIZE size_;	// アイテムの大きさ

	int invincibilityFrames_ = 0;	// 無敵時間
	
	// 生存フラグ	true / 生存中にする , false / 生存している
	bool isAlive_ = true;	
	
	// 掴まれているか　true / 掴まれている, false / 掴まれていない
	bool isGrabbed_ = false;	

	// 空中から1度でもステージに接触したか　true / 接触した , false / 接触していない
	bool hasTouchedStage_ = true;	

	// 場所に入っているか	true / 入っている , false / 入っていない
	bool hasTouchedDeliveryLocation_ = false;	// 納品場所	
	bool hasTouchedCart_ = false;				// カート

	// ハイライト表記
	bool isFound_ = false;	// 一度でも発見したか　true / 見つけた , false / 見つけてない
	int foundCounter_ = 0;	// 発見した物を一定時間ハイライトさせる用のカウンター
	
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
