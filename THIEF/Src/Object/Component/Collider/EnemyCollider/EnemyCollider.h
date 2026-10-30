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

	// 初期化 
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 判定対象の敵を設定
	void SetEnemy(EnemyBase* enemy) { enemy_ = enemy; }

	// 当たり判定を取るプレイヤーを設定
	void SetPlayer(PlayerController* player) { player_ = player; }

	// 当たり判定に使うステージを設定
	void SetStage(Stage* stage) { stage_ = stage; }

	// 掴み判定に使うクロスヘアを設定
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