#pragma once

#include "../../Component.h"

// 前方宣言
class Stage;
class PlayerController;
class Crosshair;

class DeliveryLocationCollider:public Component
{
public:

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void Draw2D(void) override;		// 更新

	// プレイヤー設定
	void SetPlayer(PlayerController* player) { player_ = player; }

	// クロスヘア設定
	void SetCrosshair(Crosshair* crosshair) { crosshair_ = crosshair; }

private:

	// アイテムと納品場所の当たり判定
	void ItemToDeliveryLocationCollision(void);

	// 納品完了スイッチとプレイヤー掴み機能との当たり判定
	void DoneSwitchToPlayerGrabbingCollision(void);

private:

	// ステージ
	Stage* stage_ = nullptr;

	// プレイヤー
	PlayerController* player_ = nullptr;

	// クロスヘア
	Crosshair* crosshair_ = nullptr;

	void DebugDraw(void);
};

