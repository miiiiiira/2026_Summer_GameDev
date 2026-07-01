#pragma once

#include "../../Component.h"

// 前方宣言
class Stage;
class PlayerController;
class Crosshair;

class CartCollider :public Component
{
public:

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void Draw2D(void) override;		// 更新

	// ステージ設定
	void SetStage(Stage* stage) { stage_ = stage; }

	// プレイヤー設定
	void SetPlayer(PlayerController* player) { player_ = player; }

	// クロスヘア設定
	void SetCrosshair(Crosshair* crosshair) { crosshair_ = crosshair; }

private:

	// アイテムとカートの当たり判定
	void ItemToCartCollision(void);

	// カートとプレイヤー掴み機能との当たり判定
	void CartToPlayerGrabbingCollision(void);

private:

	// ステージ
	Stage* stage_ = nullptr;

	// プレイヤー
	PlayerController* player_ = nullptr;

	// クロスヘア
	Crosshair* crosshair_ = nullptr;

	void DebugDraw(void);
};
