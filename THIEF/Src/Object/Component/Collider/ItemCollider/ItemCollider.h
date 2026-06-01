#pragma once

#include "../../Component.h"
#include "../../Transform/Transform.h"

// 前方宣言
class Item;
class PlayerController;
class Stage;
class Crosshair;

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

	// クロスヘア設定
	void SetCrosshair(Crosshair* crosshair) { crosshair_ = crosshair; }

	// 接地判定
	bool IsGround(void) const { return isGround_; }

	// 接地フラグを折る
	void IsGroundFold(void) { isGround_ = false; }

	// 無敵時間をリセットする
	void ResetInvincibilityFrame(void) { invincibilityFrames_ = INVINCIBILITY_FRAMES; }

private:

	// プレイヤーの掴み機能との当たり判定
	void PlayerGrabCollision(void);

	// ステージとの当たり判定
	void StageCollision(void);

	// 無敵時間の更新処理
	void UpdateInvincibility(void);

private:

	// 60フレームで大体1.5秒くらいの無敵時間
	static constexpr int INVINCIBILITY_FRAMES = 60;

	// 接地フラグ
	bool isGround_ = false;

	// 無敵時間
	int invincibilityFrames_;

	// アイテム
	Item* item_ = nullptr;

	// プレイヤー
	PlayerController* player_ = nullptr;

	// ステージ
	Stage* stage_ = nullptr;

	// クロスヘア
	Crosshair* crosshair_ = nullptr;
};

