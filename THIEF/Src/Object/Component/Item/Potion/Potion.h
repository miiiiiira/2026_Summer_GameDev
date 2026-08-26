#pragma once

#include "../Item.h"

class Potion : public Item
{
public:

	static constexpr float COLLISION_RADIUS_X = 15.0f;	// 当たり判定用の半径
	static constexpr float COLLISION_RADIUS_Y = 14.0f;	// 当たり判定用の半径

public:

	Potion(void);					// コンストラクタ

	void SetParam(void) override;	// 初期化
	void Break(void) override;		// 破壊時
	void Damage(void) override;		// ダメージ時

private:
	
	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };
	
	static constexpr int PRICE = 800;		// 金額
	static constexpr int HARDNESS = 250;	// 頑丈さ
	static constexpr float WEIGHT = -3.0f;	// 重さ
};


