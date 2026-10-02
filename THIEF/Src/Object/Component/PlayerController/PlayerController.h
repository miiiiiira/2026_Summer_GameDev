#pragma once

#include <variant>
#include <DxLib.h>

#include "../Component.h"
#include "Upgrade/UpgradeType.h"
#include "../../../Scene/Tutorial/TutorialInfo.h"
#include "../../../Common/CameraUtility/CameraUtility.h"
#include "PlayerInfo.h"

// 前方宣言
class Transform;
class CapsuleCollider;
class StageCollider;
class Animation;
class Item;
class Cart;
class Wisp;

// プレイヤー制御コンポーネント
class PlayerController : public Component
{
public:

	// プレイヤー初期位置
	static constexpr VECTOR DEFAULT_PLAYER_POS = { 0.0f,100.0f,0.0f };
	static constexpr float PLAYER_CAPSULE_RAD = 40.0f;

	// プレイヤーのカプセルオフセット
	static constexpr VECTOR STANDING_CAP_END_OFFSET = { 0.0f,30.0f,0.0f };		// エンド位置
	static constexpr VECTOR STANDING_CAP_START_OFFSET = { 0.0f,150.0f,0.0f };	// 立ち状態スタート位置
	static constexpr VECTOR MOVE_CAP_START_OFFSET = { 0.0f,135.0f,0.0f };		// 移動状態スタート位置
	static constexpr VECTOR CROUCHING_CAP_START_OFFSET = { 0.0f,60.0f,0.0f };	// しゃがみ状態スタート位置

	// アイテム発見につかう用のプレイヤーからの範囲
	static constexpr float PLAYER_ITEM_SEARCH_RADIUS = 800.0f;

	// カートとの距離
	static constexpr float CART_DISTANCE = 300.0f;

	// プレイヤーの掴み距離の最小値
	static constexpr float MIN_RANGE = 60.0f;

	// ステージの当たり判定用の法線
	static constexpr float FLOOR_NORMAL_Y = 0.85f;	// 床
	static constexpr float WALL_NORMAL_Y = 0.20f;	// 壁
	static constexpr float SLOPE_NORMAL_Y = 0.65f;	// 坂

	// 段差の登れる量
	static constexpr float STEP_HEIGHT = 25.0f;

public:

	PlayerController(void);		// コンストラクタ

	void Init(void) override;	// 初期化
	void Update(void) override;	// 更新
	void Draw2D(void) override;	// 2D描画

public:

	// Transformを返す
	Transform* GetTransform(void) { return transform_; }

	// CapsuleColliderを返す
	CapsuleCollider* GetCapsule(void) { return capColl_; }

	// プレイヤー状態を返す
	PLAYER_STATE GetState(void) { return stateCtrl_.state_; }

	// 掴んでいるかの状態を返す
	GRABBING_STATE GetGrabbingState(void) { return grabStateCtrl_.state_; }

	// 移動速度を返す
	float GetMoveSpeed(void) { return info_.moveSpeed_; }

	// 無敵時間を返す
	int GetInvincibleTime(void) { return info_.invincibleTime_; }

	// 掴むときの開始座標を返す
	// カメラの位置をラインの初め座標とする
	VECTOR GetLineStartPos(void) { return CameraUtility::GetCameraPos(); }

	// 掴むときの終了座標返す
	VECTOR GetLineEndPos(void);

	// ライトクラスのポインタ取得
	void SetWisp(Wisp* wisp) { wisp_ = wisp; }

	// 掴んでいるオブジェクトを設定
	void SetGrabObject(Item* item) { grabObject_ = item; }	// アイテム
	void SetGrabObject(Cart* cart) { grabObject_ = cart; }	// カート

	// 掴み動作を始める
	void StartGrabbing(float range);

	// ダメージを与える
	void SetDamage(int damage);

	// 吹っ飛びリアクションをさせる
	void SetHitReact(VECTOR moveDir, float moveSpeed, float jumpPow);

private:

	// ライト
	Wisp* wisp_ = nullptr;

	// コンポーネント
	Transform* transform_ = nullptr;		// Transform
	CapsuleCollider* capColl_ = nullptr;	// CapsuleCollider
	StageCollider* stageColl_ = nullptr;	// StageCollider

	// 掴んでいるオブジェクト	カートとアイテムのポインタを入れられる
	std::variant<std::monostate, Cart*, Item*> grabObject_;

private:

	// プレイヤー情報
	playerInfo info_;

	// プレイヤーの状態情報
	playerStateCtrl stateCtrl_;

	// 掴み状態情報
	playerGrabStateCtrl grabStateCtrl_;

private:

	// 状態別更新処理
	void StateUpdate(void);

	// 状態別初期化
	static void InitIdle(PlayerController& player);			// 待機
	static void InitMove(PlayerController& player);			// 移動
	static void InitDash(PlayerController& player);			// ダッシュ
	static void InitCrouching(PlayerController& player);	// しゃがみ
	static void InitSliding(PlayerController& player);		// スライディング
	static void InitHitReact(PlayerController& player);		// ダメージ時のリアクション
	static void InitDead(PlayerController& player);			// 死亡

	// 状態別更新
	static void UpdateIdle(PlayerController& player);		// 待機
	static void UpdateMove(PlayerController& player);		// 移動
	static void UpdateDash(PlayerController& player);		// ダッシュ
	static void UpdateCrouching(PlayerController& player);	// しゃがみ
	static void UpdateSliding(PlayerController& player);	// スライディング
	static void UpdateHitReact(PlayerController& player);	// ダメージ時のリアクション
	static void UpdateDead(PlayerController& player);		// 死亡

	// 状態を変更させる
	void ChangeState(PLAYER_STATE state);

	// 掴み状態別更新処理
	void UpdateGrabState(void);

	// 状態別更新
	static void UpdateNotGrabbing(PlayerController& player);	// 掴もうとしてない
	static void UpdateTryGrabbing(PlayerController& player);	// 掴もうとしている
	static void UpdateIsGrabbing(PlayerController& player);		// 掴んでいる

	// 掴み状態を変更させる
	void ChangeGrabState(GRABBING_STATE state) { grabStateCtrl_.state_ = state; };

	// 重力
	void ApplyGravity(void);

	// スタミナ回復
	void HealStamina(void);

	// ジャンプ
	void Jump(void);

	// つかめる範囲の設定
	bool UpdateRange(void);

	// マップ表示
	void UpdateMapDraw(void);

	// 無敵時間を更新
	void UpdateInvincible(void);

	// ヒットストップ更新
	void UpdateHitStop(void);

	// ヒットストップカウンタが0じゃない場合に揺らし量を計算
	void GetShakeOffset(int& offset);

	// 何か物を掴んでいるか
	bool IsGrabbing(void);

	// 掴んでいたらそのオブジェクトのポインタを渡す
	Item* GetGrabItem(void);	// アイテムのポインタ
	Cart* GetGrabCart(void);	// カートのポインタ

	// 移動しているかを渡す		true / 移動している, false / 移動していない
	bool InputMove(void);

	// 方向×移動速度で移動量を作って、座標に足して移動させる
	void Move(void);

	// ライトの範囲を変更
	void WispRangeChange(bool flg);

	// 死亡座標へ到達しているか
	void IsReachedDeadPos(void);

	// 移動時の足音を鳴らす
	void PlayFootstep(void);

	// 描画関係
	void DrawHP(void);		// HP
	void DrawStamina(void);	// スタミナ
	void DebugDraw(void);	// デバッグ用
};