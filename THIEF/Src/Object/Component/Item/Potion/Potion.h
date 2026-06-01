#pragma once
#include "../Item.h"

class Potion : public Item
{
public:

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };

	// 金額
	static constexpr int MONEY = 2000;

	// 当たり判定用の半径
	static constexpr float COLLISION_RADIUS_X = 6.0f;
	static constexpr float COLLISION_RADIUS_Y = 5.0f;

	// 頑丈さ
	static constexpr int HARDNESS = 0;

	// 重さ
	static constexpr float WEIGHT = -3.0f;

	// コンストラクタ
	Potion();
	// デストラクタ
	~Potion(void)override;

	// 初期化処理
	void SetParam(void) override;
};


