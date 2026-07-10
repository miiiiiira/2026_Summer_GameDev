#pragma once
#include "../Item.h"
class Skull : public Item
{
public:

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// 金額
	static constexpr int PRICE = 3000;

	// 当たり判定用の半径
	static constexpr float COLLISION_RADIUS_X = 50.0f;
	static constexpr float COLLISION_RADIUS_Y = 5.0f;

	// 頑丈さ
	static constexpr int HARDNESS = 700;

	// 重さ
	static constexpr float WEIGHT = -10.0f;

	// コンストラクタ
	Skull(void);
	// デストラクタ
	~Skull(void)override;

	// 初期化処理
	void SetParam(void) override;

	// 破壊時の処理
	void Break(void) override;

	// ダメージ時の処理
	void Damage(void) override;
};

