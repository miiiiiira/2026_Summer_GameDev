#pragma once

#include "../ItemBase.h"

class Goblet : public ItemBase
{
public:

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };

	// 座標
	static constexpr VECTOR DEFAULT_POS = { 10.0f,100.0f,20.0f };

	// 金額
	static constexpr int MONEY = 1000;

	// 当たり判定用の半径
	static constexpr float COLLISION_RADIUS = 5.0f;

	// 当たり判定用のオフセット
	static constexpr float COLLISION_OFFSET = 7.0f;

	// 頑丈さ
	static constexpr float HARDNESS = 10.0f;

	// コンストラクタ
	Goblet(void);
	// デストラクタ
	~Goblet(void)override;

	// 読み込み処理
	void Load(void) override;
	// 初期化処理
	void Init(void) override;
};

