#pragma once
#include <DxLib.h>
#include <unordered_map>
#include "../../../Scene/GameScene/GameScene.h"

struct DeliveryData
{
	// 納品場所の大きさ
	VECTOR deliverySize_;
	// ステージと納品場所までの相対座標
	VECTOR deliveryLocalPos_;
	// ステージと納品完了スイッチまでの相対座標
	VECTOR doneSwitchLocalPos_;
};

namespace DeliveryTable
{
	extern const std::unordered_map<STAGE_NUM, DeliveryData> Table;
}