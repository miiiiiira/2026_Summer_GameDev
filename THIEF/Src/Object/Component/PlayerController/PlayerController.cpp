#include "PlayerController.h"

#include "../../../Application.h"

#include "../../../Common/Manager/Input/InputManager.h"
#include "../../../Common/Manager/System/SystemManager.h"
#include "../../../Common/Manager/PlayerStatus/PlayerStatusManager.h"
#include "../../../Common/Math/Math.h"

#include "../../Object.h"
#include "../../Component/Transform/Transform.h"
#include "../../Component/Animation/Animation.h"
#include "../../Component/Item/Item.h"
#include "../../Component/Lantern/Lantern.h"
#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/CameraUtility/CameraUtility.h"
#include "../../../Scene/SceneManager.h"

#include "../Collider/StageCollider/StageCollider.h"

// 初期化
void PlayerController::Init()
{
	// オーナーからTransform取得
	transform_ = owner_->GetComponent<Transform>();

	// 角度から方向に変換する
	moveDir_ = { 0.0f, 0.0f,0.0f };

	// プレイヤーの状態
	state_ = PLAYER_STATE::IDLE;

	// 掴み状態を表すステート
	grabState_ = GRABBING_STATE::NOT_GRABBING;

	// プレイヤーステータスマネージャー
	auto status = PlayerStatusManager::GetInstance().GetPlayerStatus();

	// HPの初期化
	hp_ = status.hp_;

	// プレイヤーの移動速度の初期化
	moveSpeed_ = DEFAULT_SPEED;

	// スライディング可能時間の初期化
	slidingInputBufferTime = 0;

	// スタミナの初期化
	stamina_ = status.staminaMax_;

	// スタミナを回復させるまでの時間カウンタの初期化
	staminaCounter_ = 0;

	// ジャンプ数の初期化
	jumpNum_ = 0;

	// 掴み距離の初期化
	range_ = status.rangeMax_;
}

// 更新
void PlayerController::Update()
{
	// 移動処理
	Move();

	// スタミナ回復処理
	HealStamina();

	// ジャンプ処理
	Jump();

	// 重力処理
	ApplyGravity();

	// 掴み動作処理
	Grabbing();

	// 一定の座標いったら
	if (transform_->pos_.y < DEAD_POS_Y)
	{
		// ゲームオーバー
		SceneManager::GetInstance()->TrueGameOver();
		return;
	}
}

void PlayerController::Draw2D()
{
	DebugDraw();
}

Transform* PlayerController::GetTransform()
{
	return owner_->GetComponent<Transform>();
}

PLAYER_STATE PlayerController::GetState(void)
{
	return state_;
}

GRABBING_STATE PlayerController::GetGrabbingState(void)
{
	return grabState_;
}

VECTOR PlayerController::GetLineStartPos(void)
{
	// 相対座標
	VECTOR LOCAL_POS = 
	{ 0.0f,0.0f, PlayerStatusManager::GetInstance().GetPlayerStatus().rangeMax_ };

	// 座標に反映
	VECTOR downPos = CameraUtility::AddCameraPosLocalPos(LOCAL_POS);

	return downPos;
}

VECTOR PlayerController::GetLineEndPos(void)
{
	// カメラの位置をラインの初め座標とする
	return CameraUtility::GetCameraPos();
}

void PlayerController::StartGrabbing(float range)
{
	// 掴み状態を始める
	grabState_ = GRABBING_STATE::IS_GRABBING;

	range_ = range;
}

void PlayerController::SetLantern(Lantern* lantern)
{
	// ランタンクラスのポインタを設定
	lantern_ = lantern;
}

void PlayerController::SetItemPoint(Item* item)
{
	// アイテムクラスのポインタを設定
	item_ = item;
}

// 移動処理
void PlayerController::Move()
{
	// Transformがなければ処理しない
	if (!transform_) return;

	// ランタンがなければ処理しない
	if (!lantern_) return;

	// スライディング処理
	InputSliding();

	// スライディングからしゃがみ処理
	if (SlidingToCrouching())return;

	// 移動量
	VECTOR dir = Math::VECTOR_ZERO;

	// ゲームパッドが接続数で処理を分ける
	if (SystemManager::GetInstance().GetIsDevice())
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

	// 光がついてなかったら付ける
	if (!lantern_->GetLight())
	{
		// ランタンの光をつける
		lantern_->SetLight(true);
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
		mat = MMult(mat, MGetRotY(CameraUtility::GetCameraAngle().y));

		// 回転行列を使用して、ベクトルを回転させる
		moveDir_ = VTransform(dir, mat);

		// 方向×スピードで移動量を作って、座標に足して移動
		transform_->pos_ = VAdd(transform_->pos_, VScale(moveDir_, moveSpeed_));
	}
	else if (state_ != PLAYER_STATE::SLIDING)
	{
		// 待機状態にする
		state_ = PLAYER_STATE::IDLE;
		// 移動速度を初期化
		moveSpeed_ = 0.0f;
	}

	// しゃがみ処理
	Crouching();

}

// 重力処理
void PlayerController::ApplyGravity()
{
	// StageCollider取得
	auto stageCol = owner_->GetComponent<StageCollider>();
	
	if (!stageCol) return;

	// Y座標へ反映
	transform_->pos_.y += velocityY_;

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

		// ジャンプした回数を初期化
		jumpNum_ = 0;
	}
}

void PlayerController::Dash(void)
{
	// もし走るボタンを押されたかつ、しゃがみ状態じゃないかつ、スタミナがあった場合
	if (InputManager::GetInstance()->DashButtons()
		&& state_ != PLAYER_STATE::CROUCHING
		&& stamina_ >= 0.1f)
	{
		// プレイヤーの状態を走り状態にする
		state_ = PLAYER_STATE::DASH;

		// スタミナを減らす
		stamina_ -= 0.1f;
		if (stamina_ <= 0.0f)
		{
			// 0を超えないようにする
			stamina_ = 0.0f;
		}

		// カウンターリセット
		staminaCounter_ = 0;

		// プレイヤーの移動速度をダッシュの移動速度にする
		moveSpeed_ = PlayerStatusManager::GetInstance().GetPlayerStatus().dashMoveSpeed_;

		// スライディング可能時間(秒数)を設定
		slidingInputBufferTime = SLIDING_INPUT_BUFFER_TIME;
	}
	else
	{
		// プレイヤーの状態を普通の移動状態にする
		state_ = PLAYER_STATE::MOVE;

		// 走るボタンを押されなかった場合
		// 移動速度はデフォルトに設定
		moveSpeed_ = DEFAULT_SPEED;
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
	if (InputManager::GetInstance()->CrouchingButtons())
	{
		// スライディング状態にする
		state_ = PLAYER_STATE::SLIDING;
		// プレイヤーのスライディングの移動速度とダッシュ移動速度を加算
		moveSpeed_ = SLIDING_SPEED+PlayerStatusManager::GetInstance().GetPlayerStatus().dashMoveSpeed_;

		// スライディング可能時間を初期化
		slidingInputBufferTime = 0;

		// ランタンの光を消す
		lantern_->SetLight(false);
	}
}

bool PlayerController::SlidingToCrouching(void)
{
	// スライディング状態かつ移動速度が0より大きく移動している場合
	if (state_ == PLAYER_STATE::SLIDING && moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		moveSpeed_ -= 0.2f;

		// 0以下にならないようにする
		if (moveSpeed_ <= 0.0f)
		{
			moveSpeed_ = 0.0f;
			// しゃがみ状態にする
			state_ = PLAYER_STATE::CROUCHING;
		}

		// 方向×スピードで移動量を作って、座標に足して移動
		transform_->pos_ = VAdd(transform_->pos_, VScale(moveDir_, moveSpeed_));

		return true;
	}

	return false;
}

void PlayerController::Crouching(void)
{
	// 左Ctrl押されたかつスライディング中じゃない場合
	if (InputManager::GetInstance()->CrouchingButtons() &&
		state_ != PLAYER_STATE::SLIDING)
	{
		// しゃがみ状態にする
		state_ = PLAYER_STATE::CROUCHING;

		// ランタンの光を消す
		lantern_->SetLight(false);
	}
}

void PlayerController::HealStamina(void)
{
	float staminaMax = PlayerStatusManager::GetInstance().GetPlayerStatus().staminaMax_;

	// スタミナがMaxだったら処理を飛ばす
	if (stamina_ >= staminaMax)return;

	// カウンターを進める
	staminaCounter_++;

	// しゃがみ状態だったら
	if (state_ == PLAYER_STATE::CROUCHING)
	{
		// スタミナ回復させる
		stamina_ += RECOVERY_STAMINA;

		if (stamina_ > staminaMax)
		{
			// 最大スタミナを超えないようにする
			stamina_ = staminaMax;
		}

		return;
	}

	// スタミナ回復を行うまでの制限時間を超えたら入る
	if (staminaCounter_ >= RECOVERY_STAMINA_WAIT_TIME)
	{
		// スタミナ回復させる
		stamina_ += RECOVERY_STAMINA;

		if (stamina_ > staminaMax)
		{
			// 最大スタミナを超えないようにする
			stamina_ = staminaMax;
		}
	}
}

void PlayerController::Jump(void)
{

	// StageCollider取得
	auto stageCol = owner_->GetComponent<StageCollider>();

	if (!stageCol) return;

	// ジャンプボタンを押されたかつ、ジャンプ中では無いかつ、ジャンプ回数がMaxまで到達していなかったら
	if (InputManager::GetInstance()->JumpButtons()
		&& jumpNum_ < PlayerStatusManager::GetInstance().GetPlayerStatus().jumpNumMax_)
	{
		// ジャンプした回数を加算
		++jumpNum_;

		// ジャンプ力を設定
		velocityY_ = JUMP_POW;

		// 接地フラグを折る
		stageCol->IsGroundFold();
	}
}

void PlayerController::Grabbing(void)
{
	switch (grabState_)
	{
	case GRABBING_STATE::NOT_GRABBING:

		// 掴もうとしていたら
		if (InputManager::GetInstance()->IsTrgDownGrabbingButtons())
		{
			// 状態を変更
			grabState_ = GRABBING_STATE::TRY_GRABBING;
		}

		break;
	case GRABBING_STATE::TRY_GRABBING:

		// 掴もうとしていなくなったら
		if (!InputManager::GetInstance()->IsNewGrabbingButtons())
		{
			// 状態を変更
			grabState_ = GRABBING_STATE::NOT_GRABBING;
		}

		break;
	case GRABBING_STATE::IS_GRABBING:

		// アイテムの中身がなかったら
		if (item_ == nullptr)return;

		// つかめる範囲に変更があったら
		if (RangeUpdate())
		{
			// アイテムに反映させる
			item_->SetLocalPosZ(range_);
		}

		// マウスの左クリックが今離されたか、マウスの左クリックを押されていなかったら
		if (InputManager::GetInstance()->IsUpGrabbingButtons()
			||!InputManager::GetInstance()->IsNewGrabbingButtons())
		{
			// 掴み動作を終わる
			grabState_ = GRABBING_STATE::NOT_GRABBING;
			// アイテムの追従を終わる
			item_->EndGrabbed();
			item_ = nullptr;
		}

		break;
	default:
		break;
	}
}


bool PlayerController::RangeUpdate(void)
{
	float rangeMax = PlayerStatusManager::GetInstance().GetPlayerStatus().rangeMax_;

	// 物との距離を大きくする操作が行われていたら
	if (InputManager::GetInstance()->PushItemButtons())
	{
		// 物との距離を大きくする
		range_ += EXTEND_RENGE_MOVE;

		// 最大値が超えないようにする
		if (range_ > rangeMax)
		{
			range_ = rangeMax;
		}

		// 変更があったらtrueを返す
		return true;
	}
	// 物との距離を小さくする操作が行われていたら
	else if (InputManager::GetInstance()->PullItemButtons())
	{
		// 物との距離を小さくする
		range_ -= EXTEND_RENGE_MOVE;

		// 最小値が超えないようにする
		if (range_ < MIN_RENGE)
		{
			range_ = MIN_RENGE;
		}

		// 変更があったらtrueを返す
		return true;
	}

	// 変更がなかったらfalseを返す
	return false;
}

void PlayerController::DebugDraw(void)
{
	// HPの表示
	DrawFormatStringToHandle(10, 50, 0x00fa9a, Application::GetInstance()->GetFont(),
		"HP : %.0f / %.0f", 
		hp_, PlayerStatusManager::GetInstance().GetPlayerStatus().hpMax_);

	// スタミナの表示
	DrawFormatStringToHandle(10, 90, 0xffc800, Application::GetInstance()->GetFont(),
		"STAMINA : %.0f / %.0f",
		stamina_, PlayerStatusManager::GetInstance().GetPlayerStatus().staminaMax_);
}
