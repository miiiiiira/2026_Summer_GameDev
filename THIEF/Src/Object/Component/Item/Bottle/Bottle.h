#pragma once
#include "../Item.h"
class Bottle :public Item
{
public:

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };

	// 金額
	static constexpr int MONEY = 1200;

	// 当たり判定用の半径
	static constexpr float COLLISION_RADIUS_X = 23.0f;
	static constexpr float COLLISION_RADIUS_Y = 22.0f;

	// 頑丈さ
	static constexpr int HARDNESS = 20;

	// 重さ
	static constexpr float WEIGHT = -8.0f;

	// コンストラクタ
	Bottle();
	// デストラクタ
	~Bottle(void)override;

	// 初期化処理
	void SetParam(void) override;

	// 破壊時の処理
	void Break(void) override;
};

