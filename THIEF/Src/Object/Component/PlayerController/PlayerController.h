#pragma once
#include "../Component.h"
#include "Upgrade/UpgradeType.h"
#include "../../../Scene/Tutorial/TutorialInfo.h"
#include "../../../Common/CameraUtility/CameraUtility.h"
#include "PlayerInfo.h"
#include <variant>
#include <DxLib.h>

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
	static constexpr float PLAYER_CAPUSULE_RAD = 40.0f;

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
	static constexpr float MIN_RENGE = 60.0f;

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

	// リミット設定
	static constexpr float DEAD_POS_Y = -1500.0f;	// プレイヤーが死亡する座標
	static constexpr int  INVINCIBLE_TIME = 120;	// 無敵時間

	// 移動設定
	static constexpr float DEFAULT_SPEED = 7.0f;			// 通常時移動速度
	static constexpr float END_SLIDING_SPEED = 5.0f;		// この移動速度になったらスライディングを終了する
	static constexpr float SLIDING_SPEED = 3.0f;			// スライディング時移動速度
	static constexpr int SLIDING_INPUT_BUFFER_TIME = 20;	// スライディング可能時間(0.5秒数)
	static constexpr float SLIDING_FRICTION = 0.2f;			// スライディング時の摩擦
	static constexpr float RECOVERY_STAMINA = 0.05f;			// スタミナ回復量
	static constexpr int RECOVERY_STAMINA_WAIT_TIME = 3 * 60;	// スタミナ回復を行うまでの時間(秒数)

	// 重力
	static constexpr float JUMP_POW = 25.0f;	// ジャンプ力
	static constexpr float GRAVITY = -1.98f;	// 重力加速度
	static constexpr float MAX_FALL = -40.0f;	// 最大落下速度

	// ダメージ設定
	static constexpr float HIT_REACT_FRICTION = 0.5f;	// ダメージ時のリアクション時の摩擦
	static constexpr int SHAKE_TIME = 20;				// 揺らす時間
	static constexpr int DAMAGE_EFFECT_ALPHA = 64;		 // ダメージエフェクトのアルファ値
	static constexpr unsigned int DAMAGE_EFFECT_COLOR = 0xff0000;	// ダメージエフェクトのカラー値

	// 掴み
	static constexpr float EXTEND_RENGE_MOVE = 10.0f;			// 掴み距離を伸ばす時の速度
	static constexpr float  END_GRAB_CART_DISTANCE = 800.0f;	// カートを離す距離

	// 足音
	static constexpr int MOVE_SOUND_INTERVAL = 40;		// プレイヤーの足音のインターバル
	static constexpr float MOVE_SPEED_UP_MULTI = 1.2f;	// 足音のインターバル倍率

	// 描画設定
	static constexpr int  STATUS_DRAW_POS_X = 10;		// ステータス描画を始める座標
	static constexpr int  HP_DRAW_POS_Y = 50;			// HPの描画Y軸
	static constexpr int  STAMINA_DRAW_POS_Y = 90;		// STAMINAの描画Y軸
	static constexpr int STATUS_DRAW_POS_OFFSET = 10;	// オフセット
	static constexpr float COEFFICIENT = 0.2f;			// 線形補間の係数

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
	static void IdleInit(PlayerController& player);			// 待機
	static void MoveInit(PlayerController& player);			// 移動
	static void DashInit(PlayerController& player);			// ダッシュ
	static void CrouchingInit(PlayerController& player);	// しゃがみ
	static void SlidingInit(PlayerController& player);		// スライディング
	static void HitReactInit(PlayerController& player);		// ダメージ時のリアクション
	static void DeadInit(PlayerController& player);			// 死亡

	// 状態別更新
	static void IdleUpdate(PlayerController& player);		// 待機
	static void MoveUpdate(PlayerController& player);		// 移動
	static void DashUpdate(PlayerController& player);		// ダッシュ
	static void CrouchingUpdate(PlayerController& player);	// しゃがみ
	static void SlidingUpdate(PlayerController& player);	// スライディング
	static void HitReactUpdate(PlayerController& player);	// ダメージ時のリアクション
	static void DeadUpdate(PlayerController& player);		// 死亡

	// 状態を変更させる
	void ChangeState(PLAYER_STATE state);

	// 掴み状態別更新処理
	void GrabStateUpdate(void);

	// 状態別更新
	static void NotGrabbingUpdate(PlayerController& player);	// 掴もうとしてない
	static void TryGrabbingUpdate(PlayerController& player);	// 掴もうとしている
	static void IsGrabbingUpdate(PlayerController& player);		// 掴んでいる

	// 掴み状態を変更させる
	void ChangeGrabState(GRABBING_STATE state) { grabStateCtrl_.state_ = state; };

	// 重力
	void ApplyGravity(void);

	// スタミナ回復
	void HealStamina(void);

	// ジャンプ
	void Jump(void);

	// つかめる範囲の設定
	bool RangeUpdate(void);

	// マップ表示
	void MapDrawUpdate(void);

	// 無敵時間を更新
	void InvincibleUodate(void);

	// ヒットストップ更新
	void HitStopUodate(void);

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
	void wispRangeChange(bool flg);

	// 死亡座標へ到達しているか
	void IsReachedDeadPos(void);

	// 描画関係
	void DrawHP(void);		// HP
	void DrawStamina(void);	// スタミナ
	void DebugDraw(void);	// デバッグ用
};