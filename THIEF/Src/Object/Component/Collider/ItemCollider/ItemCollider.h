#pragma once

#include "../../Component.h"
#include "../../Transform/Transform.h"

// 前方宣言
class Item;
class PlayerController;
class Stage;

// アイテム衝突コンポーネント
class ItemCollider : public Component
{
public:
	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新

	// プレイヤー設定
	void SetPlayer(PlayerController* player) { player_ = player; }

	// ステージ設定
	void SetStage(Stage* stage) { stage_ = stage; }

	// 接地判定
	bool IsGround(void) const { return isGround_; }

	// 接地フラグを折る
	void IsGroundFold(void) { isGround_ = false; }

private:

	// プレイヤーの掴み機能との当たり判定
	void PlayerGrabCollision(void);

	// ステージとの当たり判定
	void StageCollision(void);

private:

	// 接地フラグ
	bool isGround_ = false;

	// アイテム
	Item* item_ = nullptr;

	// プレイヤー
	PlayerController* player_ = nullptr;

	// ステージ
	Stage* stage_ = nullptr;
};

