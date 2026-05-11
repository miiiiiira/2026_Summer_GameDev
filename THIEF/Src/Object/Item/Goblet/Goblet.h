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
	static constexpr VECTOR DEFAULT_POS = { 0.0f,0.0f,0.0f };

	// 金額
	static constexpr int MONEY = 1000;

	// コンストラクタ
	Goblet(void);
	// デストラクタ
	~Goblet(void)override;

	// 読み込み処理
	void Load(void) override;
	// 初期化処理
	void Init(void) override;
};

