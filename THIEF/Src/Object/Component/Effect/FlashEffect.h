#pragma once

#include "../Component.h"

class FlashEffect :public Component
{
public:

	FlashEffect(void);	// コンストラクタ

	void Init(void)override;	// 初期化
	void Update(void)override;	// 更新
	void Draw2D(void)override;	// 2D描画

	void SetEffect(int alpha, unsigned int color);	// 指定された設定でエフェクトを開始させる

private:

	int alpha_;				// アルファ値
	unsigned int color_;	// カラー値
};

