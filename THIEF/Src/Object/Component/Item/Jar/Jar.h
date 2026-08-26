#pragma once

#include "../Item.h"

class Jar : public Item
{
public:

	static constexpr float COLLISION_RADIUS_X = 32.0f;	// 当たり判定用の半径X軸
	static constexpr float COLLISION_RADIUS_Y = 20.0f;	// 当たり判定用の半径Y軸

public:

	Jar(void);						// コンストラクタ

	void SetParam(void) override;	// 初期化
	void Break(void) override;		// 破壊時
	void Damage(void) override;		// ダメージ時

private:

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };	

	static constexpr int PRICE = 2500;		// 金額
	static constexpr int HARDNESS = 300;	// 頑丈さ
	static constexpr float WEIGHT = -5.0f;	// 重さ
};

