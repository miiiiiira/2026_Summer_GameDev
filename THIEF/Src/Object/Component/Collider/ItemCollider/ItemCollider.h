#pragma once

#include "../../Component.h"
#include "../../Transform/Transform.h"

// 前方宣言
class Item;
class PlayerController;
class Stage;
class Crosshair;
class Wisp;

// アイテム衝突コンポーネント
class ItemCollider : public Component
{
private:
	// めり込み防止
	static constexpr float SKIN = 0.2f;

	// 壁スライド反復回数
	static constexpr int MAX_BOUNCE = 4;

	// どの角度までを床として扱うか
	static constexpr float FLOOR_NORMAL_Y = 0.95f;

public:
	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新

	// プレイヤー設定
	void SetPlayer(PlayerController* player) { player_ = player; }

	// ステージ設定
	void SetStage(Stage* stage) { stage_ = stage; }

	// クロスヘア設定
	void SetCrosshair(Crosshair* crosshair) { crosshair_ = crosshair; }

	// ライトの範囲設定
	void SetWisp(Wisp* wisp) { wisp_ = wisp; }

	// 接地判定
	bool IsGround(void) const { return isGround_; }

	// 接地フラグを折る
	void IsGroundFold(void) { isGround_ = false; }

private:

	// カメラレイとの当たり判定
	void CameraRayCollision(void);

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

	// クロスヘア
	Crosshair* crosshair_ = nullptr;

	// ライトの範囲
	Wisp* wisp_ = nullptr;
};

