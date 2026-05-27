#pragma once

#include "../../Component.h"

// 前方宣言
class Stage;
class PlayerController;

class DeliveryLocationCollider:public Component
{
public:

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void Draw(void) override;		// 更新

	// プレイヤー設定
	void SetPlayer(PlayerController* player) { player_ = player; }

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

	void DebugDraw(void);
};

