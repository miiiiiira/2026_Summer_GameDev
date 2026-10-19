#pragma once

#include "../Item.h"

class Jar : public Item
{
public:

	// 当たり判定用の半径
	static constexpr float COLLISION_RADIUS_X = 32.0f;	
	static constexpr float COLLISION_RADIUS_Y = 20.0f;

public:

	Jar(void);						// コンストラクタ

	void SetParam(void) override;	// 初期化
	void Break(void) override;		// 破壊時
	void Damage(void) override;		// ダメージ時
};

