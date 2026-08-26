#pragma once

#include "../Item.h"

class Bottle :public Item
{
public:

	static constexpr float COLLISION_RADIUS_X = 23.0f;	// 当たり判定用の半径X軸
	static constexpr float COLLISION_RADIUS_Y = 22.0f;	// 当たり判定用の半径Y軸

public:

	Bottle(void);					// コンストラクタ

	void SetParam(void) override;	// 初期化
	void Break(void) override;		// 破壊時
	void Damage(void) override;		// ダメージ時

private:
	
	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };	

	static constexpr int PRICE = 1200;		// 金額
	static constexpr int HARDNESS = 300;	// 頑丈さ
	static constexpr float WEIGHT = -8.0f;	// 重さ

};

