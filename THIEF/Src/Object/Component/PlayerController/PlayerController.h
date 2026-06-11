#pragma once
#include "../Component.h"
#include "Upgrade/UpgradeType.h"
#include <DxLib.h>

// 前方宣言
class Transform;
class Animation;
class Item;
class Lantern;

// プレイヤーの状態
enum class PLAYER_STATE
{
	IDLE,		// 待機
	MOVE,		// 移動
	DASH,		// ダッシュ	
	CROUCHING,	// しゃがみ
	SLIDING,	// スライディング
};

enum class GRABBING_STATE
{
	NOT_GRABBING,	// 掴もうとしてない
	TRY_GRABBING,	// 掴もうとしている
	IS_GRABBING,	// 掴んでいる
};

// プレイヤー制御コンポーネント
class PlayerController : public Component
{
public:

	// プレイヤーの掴み距離の最小値
	static constexpr float MIN_RENGE = 50.0f;

	// 通常時移動速度
	static constexpr float DEFAULT_SPEED = 7.0f;

	// スライディング時移動速度
	static constexpr float SLIDING_SPEED = 3.0f;

	// スライディング可能時間(0.5秒数)
	static constexpr int SLIDING_INPUT_BUFFER_TIME = 20;

	// スタミナ回復量
	static constexpr float RECOVERY_STAMINA = 0.05f;

	// スタミナ回復を行うまでの時間(秒数)
	static constexpr int RECOVERY_STAMINA_WAIT_TIME = 3 * 60;

	// 掴み距離を伸ばす時の速度
	static constexpr float EXTEND_RENGE_MOVE = 10.0f;

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.2f;
	
	// プレイヤーが死ぬ座標
	static constexpr float DEAD_POS_Y = -1000.0f;

	// プレイヤーの足音のインターバル
	static constexpr int MOVE_SOUND_INTERVAL = 40;
	static constexpr float MOVE_SPEED_UP_MULTI = 1.2f;

	// プレイヤーのカプセルオフセット
	// 立ち状態
	// スタート位置
	static constexpr VECTOR STANDING_CAP_START_OFFSET = { 0.0f,150.0f,0.0f };
	// エンド位置
	static constexpr VECTOR STANDING_CAP_END_OFFSET = { 0.0f,30.0f,0.0f };

	// しゃがみ状態
	// スタート位置
	static constexpr VECTOR CROUCHING_CAP_START_OFFSET = { 0.0f,60.0f,0.0f };

public:
	void Init() override;		// 初期化
	void Update() override;		// 更新
	void Draw2D() override;		// 2D描画

	// Transformを返す
	Transform* GetTransform();

	// 掴んでいるかの状態を取得
	PLAYER_STATE GetState(void);

	// 掴んでいるかの状態を取得
	GRABBING_STATE GetGrabbingState(void);

	// 掴むときの線分の初め座標を渡す
	VECTOR GetLineStartPos(void);

	// 掴むときの線分の終わり座標を渡す
	VECTOR GetLineEndPos(void);

	// 指定された掴み動作を設定
	void StartGrabbing(float range);

	// ランタンクラスのポインタ取得
	void SetLantern(Lantern* lantern);

	// アイテムを取得
	void SetItemPoint(Item* item);

private:
	// 移動処理
	void Move();

	// 重力処理
	void ApplyGravity();

	// ダッシュ処理	(走るボタンを押されたら : デフォルト移動速度＋ダッシュ速度、
	//					   押されなかったら : デフォルト移動速度)
	void Dash(void);

	// スライディング
	void InputSliding(void);

	// スライディングからしゃがみ処理
	bool SlidingToCrouching(void);
	
	// しゃがみ処理
	void Crouching(void);

	// スタミナ回復処理
	void HealStamina(void);

	// ジャンプ処理
	void Jump(void);

	// 掴み判定処理
	void Grabbing(void);

	// つかめる範囲の設定
	bool RangeUpdate(void);

	// デバッグ用描画
	void DebugDraw(void);

private:

	// アイテム
	Item* item_ = nullptr;

	// ランタン
	Lantern* lantern_ = nullptr;

	// 現在の落下速度
	float velocityY_ = 0.0f;

	// ジャンプ力
	const float JUMP_POW = 25.0f;

	// 重力加速度
	const float GRAVITY = -1.98f;

	// 最大落下速度
	const float MAX_FALL = -40.0f;

	// Transformコンポーネント
	Transform* transform_ = nullptr;

	// 移動方向
	VECTOR moveDir_;

	// 今現在のHP
	float hp_;

	// 実際に移動させる移動量
	float moveSpeed_;

	// スライディング可能時間
	int slidingInputBufferTime;

	// 今現在のスタミナ
	float stamina_;

	// スタミナを回復させるまでの時間カウンタ
	int staminaCounter_;

	// ジャンプ数
	int jumpNum_;

	// 実際に持っている掴み距離
	float range_;

	// 現在の状態表すステート
	PLAYER_STATE state_;

	// 掴み状態を表すステート
	GRABBING_STATE grabState_;

	// 足音を連続再生するインターバル
	int moveSoundInterval_;

	// 動いてない状態での、初期化処理
	void IdleInit(void);

	// しゃがみ状態での、初期化処理
	void CrouchingInit(void);
};
