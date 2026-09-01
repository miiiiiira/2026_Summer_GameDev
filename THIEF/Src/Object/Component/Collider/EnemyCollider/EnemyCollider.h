#pragma once

#include "../../Component.h"
#include "../../Transform/Transform.h"

// 前方宣言
class EnemyBase;
class PlayerController;
class Stage;
class Crosshair;

class EnemyCollider : public Component
{
public:
	void Init(void) override;
	void Update(void) override;

	// 各種参照の設定
	void SetEnemy(EnemyBase* enemy) { enemy_ = enemy; }
	void SetPlayer(PlayerController* player) { player_ = player; }
	void SetStage(Stage* stage) { stage_ = stage; }
	void SetCrosshair(Crosshair* crosshair) { crosshair_ = crosshair; }

private:

	// 敵の攻撃とプレイヤーの当たり判定
	void CheckEnemyAttack(void);

	// 敵とプレイヤーの押し出し判定
	void CollisionEnemy2Player(void);

	// プレイヤーが敵を掴もうとしたときの判定
	void CollisionEnemy2PlayerGrab(void);

private:

	// 敵
	EnemyBase* enemy_ = nullptr;

	// プレイヤー
	PlayerController* player_ = nullptr;

	// ステージ
	Stage* stage_ = nullptr;

	// クロスヘア
	Crosshair* crosshair_ = nullptr;
};