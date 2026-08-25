#pragma once

#include "../Component.h"

// 表示種類
enum CROSSHAIR_TYPE
{
	CROSSHAIR_NOT_GRAB,	// 掴めない
	CROSSHAIR_CAN_GRAB,	// 掴める
	CROSSHAIR_GRABBING,	// 掴んでいる

	CROSSHAIR_MAX,
};

class Crosshair : public Component
{
public:

	// パッド時の場合
	static constexpr int CONTROLLER_GRAB_SCREEN_RANGE = 200;	// スクリーンの中心位置からの掴み可能な範囲の直径
	static constexpr int CONTROLLER_GRAB_SCREEN_RANGE_RAD = CONTROLLER_GRAB_SCREEN_RANGE / 2;	// スクリーンの中心位置からの掴み可能な範囲の半径

public:

	Crosshair(void);			// コンストラクタ
	~Crosshair(void)override;	// デストラクタ

	void Init(void)override;	// 初期化
	void Draw2D(void)override;	// 描画

	void ChangeCrosshair(const CROSSHAIR_TYPE type);	// クロスヘアの種類を変更

private:

	int img[CROSSHAIR_TYPE::CROSSHAIR_MAX];	// 画像ハンドル
	
	CROSSHAIR_TYPE type_;	// 表示中の種類
};

