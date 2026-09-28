#pragma once

#include "../Component.h"

class FlashEffect :public Component
{
public:

	FlashEffect(void);			// コンストラクタ

	void Init(void)override;	// 初期化
	void Update(void)override;	// 更新
	void Draw2D(void)override;	// 2D描画

public:

	// 指定された設定でエフェクトを開始させる
	// 指定されたアルファ値、カラー値を設定
	void SetEffect(int alpha, unsigned int color) { alpha_ = alpha, color_ = color; }

private:

	// アルファ値
	int alpha_ = 0;			

	// カラー値
	unsigned int color_ = 0xffffff;	
};

