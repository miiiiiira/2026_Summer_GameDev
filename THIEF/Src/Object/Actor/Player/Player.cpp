#include "Player.h"

#include "../../../Application.h"
#include "../../../Input/InputManager.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Transform/MatrixUtility.h"
#include "../../Common/AnimationController.h"

#include "../../../Camera/Camera.h"

Player::Player(Camera* camera)
{
	camera_ = camera;
}

Player::~Player(void)
{
}

void Player::InitLoad(void)
{
}

void Player::InitTransform(void)
{
	// モデルの角度
	angle_ = { 0.0f, 0.0f, 0.0f };
	localAngle_ = { 0.0f, Math::Deg2Rad(180.0f), 0.0f };

	// 角度から方向に変換する
	moveDir_ = { sinf(angle_.y), 0.0f, cosf(angle_.y) };
	preInputDir_ = moveDir_;

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = Matrix::Multiplication(localAngle_, angle_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// モデルの位置設定
	pos_ = Math::VECTOR_ZERO;
	MV1SetPosition(modelId_, pos_);

	// 当たり判定を作成
	startCapsulePos_ = { 0.0f,110,0.0f };
	endCapsulePos_ = { 0.0f,30.0f,0.0f };
	capsuleRadius_ = 20.0f;
	
	// 当たり判定を取るか
	isCollision_ = true;
}

void Player::InitAnimation(void)
{
}

void Player::InitPost(void)
{
	// プレイヤーの状態
	state_ = STATE::IDLE;

	// プレイヤーの移動速度の初期化
	moveSpeed_ = 0.0f;

	// ダッシュ速度の初期化
	runSpeedMax_ = DEFAULET_DASH_SPEED;

	// スライディング可能時間の初期化
	slidingInputBufferTime = 0;

	// スタミナの初期化
	stamina_ = staminaMax_ = DEFAULT_STAMINA;

	// スタミナを回復させるまでの時間カウンタの初期化
	staminaCounter_ = 0;
}

void Player::Update(void)
{
	ActorBase::Update();

	// スタミナ回復処理
	HealStamina();
}

void Player::Draw(void)
{
	ActorBase::Draw();

	DrawFormatString(
		0, 50, 0xffffff,
		"キャラ角度　 ：(%.1f, %.1f, %.1f)",
		Math::Rad2Deg(angle_.x),
		Math::Rad2Deg(angle_.y),
		Math::Rad2Deg(angle_.z)
	);

	DrawFormatString(
		0, 200, 0xFFC800,
		"スタミナ : %.0f / %.0f",
		stamina_,
		staminaMax_
	);
}

void Player::Release(void)
{
	ActorBase::Release();
}

void Player::Upgrade(PLAYER_UPGRADE type, float upNum)
{
	// アップデートしたい能力の情報を得る
	upgradeType_ = type;

	// 種類によって変更する
	switch (upgradeType_)
	{
	case PLAYER_UPGRADE::RESTOCK_POTION:

		// ポーションの補充を行う

		break;
	case PLAYER_UPGRADE::SPEED_UP:

		// スピードを上げる

		break;
	case PLAYER_UPGRADE::STAMINA_UP:

		// スタミナの最大値を上げる

		break;
	case PLAYER_UPGRADE::HP_UP:

		// HPの最大値を上げる

		break;
	case PLAYER_UPGRADE::HEAL_HP:

		// HPを回復させる

		break;
	default:
		break;
	}

	// 強化し終わったらNONへ戻す
	upgradeType_ = PLAYER_UPGRADE::MAX;
}

void Player::Move(void)
{
	// スライディング処理
	InputSliding();

	// しゃがみ状態かつ移動速度が0より大きい場合
	if (state_ == STATE::SLIDING && moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		moveSpeed_ -= 0.2f;

		// 0以下にならないようにする
		if (moveSpeed_ <= 0.0f)
		{
			moveSpeed_ = 0.0f;
			// しゃがみ状態にする
			state_ = STATE::CROUCHING;
		}

		// 方向×スピードで移動量を作って、座標に足して移動
		pos_ = VAdd(pos_, VScale(moveDir_, moveSpeed_));

		return;
	}

	// カメラ角度を取得
	VECTOR cameraAngles = camera_->GetAngle();

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
		Run();

		// 正規化
		dir = VNorm(dir);

		// XYZの回転行列
		// XZ平面移動にする場合は、XZの回転を考慮しないようにする
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(cameraAngles.y));

		// 回転行列を使用して、ベクトルを回転させる
		moveDir_ = VTransform(dir, mat);

		// 方向×スピードで移動量を作って、座標に足して移動
		pos_ = VAdd(pos_, VScale(moveDir_, moveSpeed_));
	}
	else if(state_ != STATE::SLIDING)
	{
		// 待機状態にする
		state_ = STATE::IDLE;
		// 移動速度を初期化
		moveSpeed_ = 0.0f;
	}

	// 左Ctrl押されたかつスライディング中じゃない場合
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_LCONTROL) &&
		state_ != STATE::SLIDING)
	{
		// しゃがみ状態にする
		state_ = STATE::CROUCHING;
	}

}

void Player::Run(void)
{

	// もし走るボタンを押された場合
	if (InputManager::GetInstance()->IsNew(KEY_INPUT_LSHIFT)
		&& state_ != STATE::CROUCHING
		&& stamina_ >= 0.1f)
	{
		// プレイヤーの状態を走り状態にする
		state_ = STATE::RUN;

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
		moveSpeed_ =  DEFAULT_SPEED + runSpeedMax_;

		// スライディング可能時間(秒数)を設定
		slidingInputBufferTime = SLIDING_INPUT_BUFFER_TIME;
	}
	else
	{
		// プレイヤーの状態を普通の移動状態にする
		state_ = STATE::MOVE;

		// 走るボタンを押されなかった場合
		// 移動速度はデフォルトに設定
		moveSpeed_ = DEFAULT_SPEED;

	}

}

void Player::InputSliding(void)
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
		state_ = STATE::SLIDING;
		// プレイヤーのデフォルト移動速度にダッシュ分の移動速度を加算
		moveSpeed_ = DEFAULT_SPEED + runSpeedMax_;
		slidingInputBufferTime = 0;
	}
}

void Player::HealStamina(void)
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
