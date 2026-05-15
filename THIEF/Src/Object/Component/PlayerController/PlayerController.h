#pragma once
#include "../Component.h"
#include <DxLib.h>

// 前方宣言
class Camera;
class Transform;
class Animation;
class ItemBase;

// プレイヤーの状態
enum class PlayerState
{
	IDLE,
	MOVE,
	DASH,
	CROUCHING,
	SLIDING,
};

enum class GraspingState
{
	NOT_GRAPING,
	IS_GRAPING,
};

// プレイヤー制御コンポーネント
class PlayerController : public Component
{
private:
	// 通常時移動速度
	static constexpr float DEFAULT_SPEED = 7.0f;

	// ダッシュ時の移動速度
	static constexpr float DASH_SPEED = 10.0f;

	// スライディング可能時間(秒数)
	static constexpr int SLIDING_INPUT_BUFFER_TIME = 2 * 60;

	// スタミナ
	static constexpr float DEFAULT_STAMINA = 10.0f;

	// スタミナ回復量
	static constexpr float RECOVERY_STAMINA = 0.05f;

	// スタミナ回復を行うまでの時間(秒数)
	static constexpr int RECOVERY_STAMINA_WAIT_TIME = 1 * 60;

	// プレイヤーの掴み距離
	static constexpr float DEFAULT_RENGE = 50.0f;

public:
	void Init() override;		// 初期化
	void Update() override;		// 更新

	// Transformを返す
	Transform* GetTransform();

	// カメラ設定
	void SetCamera(Camera* camera) { camera_ = camera; }

	// 指定された掴み動作を設定
	void ChangeGrapingState(const GraspingState& grapState);

	// アイテムクラスのポインタ取得
	void SetItemClassPoint(ItemBase* item) { item_ = item; }

	// 現在の状態表すステート
	PlayerState state_ = PlayerState::IDLE;

	// 掴み状態を表すステート
	GraspingState grapState_ = GraspingState::NOT_GRAPING;
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

	// スタミナ回復処理
	void HealStamina(void);

	// 掴み判定処理
	void Grasping(void);

private:

	// アイテム
	ItemBase* item_ = nullptr;

	// 現在の落下速度
	float velocityY_ = 0.0f;

	// 重力加速度
	const float GRAVITY = -0.8f;

	// 最大落下速度
	const float MAX_FALL = -20.0f;

	// Transformコンポーネント
	Transform* transform_ = nullptr;

	// 向き
	VECTOR angle_;

	// 移動方向
	VECTOR moveDir_;

	// カメラ
	Camera* camera_ = nullptr;

	// 移動速度
	float moveSpeed_;
	float baseMoveSpeed_;

	// スライディング可能時間
	int slidingInputBufferTime;

	// スタミナ
	float stamina_;
	float staminaMax_;

	// スタミナを回復させるまでの時間カウンタ
	int staminaCounter_;

	// 掴み距離
	float range_;
	float rangeMAX_;
};
