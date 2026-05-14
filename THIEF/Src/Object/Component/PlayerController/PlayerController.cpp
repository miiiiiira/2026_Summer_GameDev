#include "PlayerController.h"

#include "../../../Input/InputManager.h"
#include "../../../Common/Math/Math.h"

#include "../../Object.h"
#include "../../Component/Transform/Transform.h"
#include "../../Component/Animation/Animation.h"
#include "../../Component/Camera/Camera.h"

#include "../Collider/StageCollider/StageCollider.h"

// 初期化
void PlayerController::Init()
{
	// オーナーからTransform取得
	transform_ = owner_->GetComponent<Transform>();

	// モデルの角度
	angle_ = { 0.0f, 0.0f, 0.0f };

	// 角度から方向に変換する
	moveDir_ = { sinf(angle_.y), 0.0f, cosf(angle_.y) };

	// プレイヤーの状態
	state_ = PlayerState::IDLE;

	// プレイヤーの移動速度の初期化
	moveSpeed_ = baseMoveSpeed_ = DEFAULT_SPEED;

	// スライディング可能時間の初期化
	slidingInputBufferTime = 0;

	// スタミナの初期化
	stamina_ = staminaMax_ = DEFAULT_STAMINA;

	// スタミナを回復させるまでの時間カウンタの初期化
	staminaCounter_ = 0;

	// 掴み距離の初期化
	range_ = rangeMAX_ = DEFAULT_RENGE;
}

// 更新
void PlayerController::Update()
{
	// 移動方向から角度に変換する
	float goal = atan2f(moveDir_.x, moveDir_.z);

	// 常に最短経路で補間
	angle_.y = Math::LerpAngle(angle_.y, goal, 0.2f);

	// 移動処理
	Move();

	// スタミナ回復処理
	HealStamina();

	// 重力処理
	ApplyGravity();
}

Transform* PlayerController::GetTransform()
{
	return owner_->GetComponent<Transform>();
}

// 移動処理
void PlayerController::Move()
{
	// カメラがなければ処理しない
	if (!camera_) return;

	// Transformがなければ処理しない
	if (!transform_) return;

	// スライディング処理
	InputSliding();

	// しゃがみ状態かつ移動速度が0より大きい場合
	if (state_ == PlayerState::SLIDING && moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		moveSpeed_ -= 0.2f;

		// 0以下にならないようにする
		if (moveSpeed_ <= 0.0f)
		{
			moveSpeed_ = 0.0f;
			// しゃがみ状態にする
			state_ = PlayerState::CROUCHING;
		}

		// 方向×スピードで移動量を作って、座標に足して移動
		transform_->pos_ = VAdd(transform_->pos_, VScale(moveDir_, moveSpeed_));

		return;
	}

	// カメラ角度を取得
	VECTOR* cameraAngles = camera_->GetAngle();

	// 移動量
	VECTOR dir = Math::VECTOR_ZERO;

	// ゲームパッドが接続数で処理を分ける
	if (GetJoypadNum() == 0)
	{
		// WASDで移動する
		if (InputManager::GetInstance()->IsNew(KEY_INPUT_W)) { dir = { 0.0f, 0.0f, 1.0f }; }
		if (InputManager::GetInstance()->IsNew(KEY_INPUT_A)) { dir = { -1.0f, 0.0f, 0.0f }; }
		if (InputManager::GetInstance()->IsNew(KEY_INPUT_S)) { dir = { 0.0f, 0.0f, -1.0f }; }
		if (InputManager::GetInstance()->IsNew(KEY_INPUT_D)) { dir = { 1.0f, 0.0f, 0.0f }; }
	}
	else
	{
		// 接続されているゲームパッド１の情報を取得
		InputManager::JOYPAD_IN_STATE padState =
			InputManager::GetInstance()->GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// アナログキーの入力値から方向を取得
		dir = InputManager::GetInstance()->GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	}

	if (!Math::EqualsVZero(dir))
	{
		// 走ったかどうかの判定
		Dash();

		// 正規化
		dir = VNorm(dir);

		// XYZの回転行列
		// XZ平面移動にする場合は、XZの回転を考慮しないようにする
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(cameraAngles->y));

		// 回転行列を使用して、ベクトルを回転させる
		moveDir_ = VTransform(dir, mat);

		// 方向×スピードで移動量を作って、座標に足して移動
		transform_->pos_ = VAdd(transform_->pos_, VScale(moveDir_, moveSpeed_));
	}
	else if (state_ != PlayerState::SLIDING)
	{
		// 待機状態にする
		state_ = PlayerState::IDLE;
		// 移動速度を初期化
		moveSpeed_ = 0.0f;
	}

	// 左Ctrl押されたかつスライディング中じゃない場合
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_LCONTROL) &&
		state_ != PlayerState::SLIDING)
	{
		// しゃがみ状態にする
		state_ = PlayerState::CROUCHING;
	}
}

// 重力処理
void PlayerController::ApplyGravity()
{
	// StageCollider取得
	auto stageCol = owner_->GetComponent<StageCollider>();
	
	if (!stageCol) return;

	// 接地判定
	
	// 空中
	if (!stageCol->IsGround())
	{
		// 重力加算
		velocityY_ += GRAVITY;
		
		// 最大落下速度
		if (velocityY_ < MAX_FALL)
			velocityY_ = MAX_FALL;
	}
	else
	{
		// 地面上なら少し下方向に押す
		// 0だと浮く場合があるため
		velocityY_ = -0.1f;
	}

	// Y座標へ反映
	transform_->pos_.y += velocityY_;
}

void PlayerController::Dash(void)
{
	// もし走るボタンを押された場合
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_LSHIFT)
		&& state_ != PlayerState::CROUCHING
		&& stamina_ >= 0.1f)
	{
		// プレイヤーの状態を走り状態にする
		state_ = PlayerState::DASH;

		// スタミナを減らす
		stamina_ -= 0.1f;
		if (stamina_ <= 0.0f)
		{
			// 0を超えないようにする
			stamina_ = 0.0f;
		}

		// カウンターリセット
		staminaCounter_ = 0;

		// プレイヤーのデフォルト移動速度にダッシュ分の移動速度を加算
		moveSpeed_ = baseMoveSpeed_ + DASH_SPEED;

		// スライディング可能時間(秒数)を設定
		slidingInputBufferTime = SLIDING_INPUT_BUFFER_TIME;
	}
	else
	{
		// プレイヤーの状態を普通の移動状態にする
		state_ = PlayerState::MOVE;

		// 走るボタンを押されなかった場合
		// 移動速度はデフォルトに設定
		moveSpeed_ = baseMoveSpeed_;
	}
}

void PlayerController::InputSliding(void)
{
	// スライディングの可能時間が無かったら処理を行わない
	if (slidingInputBufferTime <= 0)return;

	// スライディング可能時間を減らす
	slidingInputBufferTime--;

	if (slidingInputBufferTime < 0)
	{
		// 0以下にならないようにする
		slidingInputBufferTime = 0;
	}

	// しゃがみボタン押されたら
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_LCONTROL))
	{
		// スライディング状態にする
		state_ = PlayerState::SLIDING;
		// プレイヤーのデフォルト移動速度にダッシュ分の移動速度を加算
		moveSpeed_ = baseMoveSpeed_ + DASH_SPEED;

		// スライディング可能時間を初期化
		slidingInputBufferTime = 0;
	}
}

void PlayerController::HealStamina(void)
{
	// スタミナがMaxだったら処理を飛ばす
	if (stamina_ >= staminaMax_)return;

	// カウンターを進める
	staminaCounter_++;

	// スタミナ回復を行うまでの制限時間を超えたら入る
	if (staminaCounter_ >= RECOVERY_STAMINA_WAIT_TIME)
	{
		// スタミナ回復させる
		stamina_ += RECOVERY_STAMINA;

		if (stamina_ > staminaMax_)
		{
			// 最大スタミナを超えないようにする
			stamina_ = staminaMax_;
		}
	}
}
