#include "PlayerController.h"

#include "../../../Application.h"

#include "../../../Common/Manager/Input/InputManager.h"
#include "../../../Common/Manager/System/SystemManager.h"
#include "../../../Common/Manager/PlayerStatus/PlayerStatusManager.h"
#include "../../../Common/Manager/PlayerActionCounter/PlayerActionCounter.h"
#include "../../../Common/Math/Math.h"

#include "../../Object.h"
#include "../../Component/Transform/Transform.h"
#include "../../Component/Animation/Animation.h"
#include "../../Component/Item/Item.h"
#include "../../Component/Wisp/Wisp.h"
#include "../../Component/Cart/Cart.h"
#include "Map/Map.h"
#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/CameraUtility/CameraUtility.h"
#include "../../../Scene/SceneManager.h"
#include "../../../Common/Shader/Shader.h"

#include "../Collider/StageCollider/StageCollider.h"
#include "../../../Common/Manager/Audio/AudioManager.h"
#include "../../../Scene/Tutorial/TutorialScene.h"

PlayerController::PlayerController(void)
{
	// テーブルに関数のポインタを格納
	// 初期化関数
	stateCtrl_.initTable_[PLAYER_STATE_IDLE] = IdleInit;
	stateCtrl_.initTable_[PLAYER_STATE_MOVE] = MoveInit;
	stateCtrl_.initTable_[PLAYER_STATE_DASH] = DashInit;
	stateCtrl_.initTable_[PLAYER_STATE_CROUCHING] = CrouchingInit;
	stateCtrl_.initTable_[PLAYER_STATE_SLIDING] = SlidingInit;
	stateCtrl_.initTable_[PLAYER_STATE_HIT_REACT] = HitReactInit;
	stateCtrl_.initTable_[PLAYER_STATE_DEAD] = DeadInit;
	// 更新関数
	stateCtrl_.updateTable_[PLAYER_STATE_IDLE] = IdleUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_MOVE] = MoveUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_DASH] = DashUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_CROUCHING] = CrouchingUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_SLIDING] = SlidingUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_HIT_REACT] = HitReactUpdate;
	stateCtrl_.updateTable_[PLAYER_STATE_DEAD] = DeadUpdate;
}

// 初期化
void PlayerController::Init()
{
	// オーナーからTransform取得
	transform_ = owner_->GetComponent<Transform>();

	// 角度から方向に変換する
	moveDir_ = { 0.0f, 0.0f,0.0f };

	// プレイヤーの状態初期化
	ChangeState(PLAYER_STATE_IDLE);
	
	// 掴み状態を表すステート
	grabState_ = GRABBING_STATE::NOT_GRABBING;

	// プレイヤーステータスマネージャー
	auto status = PlayerStatusManager::GetInstance().GetPlayerStatus();

	// HPの初期化
	hp_ = status.hp_;

	// 無敵時間の初期化
	invincibleTime_ = 0;

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

	// 足音のサウンドインターバル
	moveSoundInterval_ = MOVE_SOUND_INTERVAL;

	// 前回しゃがみ状態かのフラグ
	prevCrouching_ = false;
}

// 更新
void PlayerController::Update()
{
	// 体力が0以下になっていたら
	if (hp_ <= 0)
	{
		// 死亡状態へ
		ChangeState(PLAYER_STATE_DEAD);
		return;
	}

	// 移動処理
	StateUpdate();

	// スタミナ回復処理
	HealStamina();

	// ジャンプ処理
	Jump();

	// 重力処理
	ApplyGravity();

	// ステージコライダー取得
	auto stageCol = owner_->GetComponent<StageCollider>();

	// ステージコライダーがあれば
	if (stageCol)
	{
		// ステージの当たり判定の計算処理
		stageCol->StageColl(velocityY_);
	}

	// 掴み動作処理
	Grabbing();

	// マップの表示処理
	MapDrawUpdate();

	// 無敵時間を減らす
	if (invincibleTime_ > 0)
	{
		--invincibleTime_;
	}
	else
	{
		SceneManager::GetInstance()->GetShader()->SetVignettePower(0.5f);
	}

	// ヒットストップ更新処理
	if (hitStopCounter_ > 0) {
		hitStopCounter_--;
	}

	// 一定の座標いったら
	if (transform_->pos_.y < DEAD_POS_Y)
	{
		hp_ = 0;
		return;
	}
}

void PlayerController::Draw2D()
{
	int shake = 0;
	// ヒットストップカウンタが0じゃない場合に揺らし量を計算
	GetShakeOffset(shake);

	int HPWidth = GetDrawStringWidthToHandle("HP: ", 4, Application::GetInstance()->GetFont(FONT_SIZE_20));
	int playerHpWidth = GetDrawFormatStringWidthToHandle(Application::GetInstance()->GetFont(FONT_SIZE_30), "%d", hp_);

	// HPの表示
	DrawStringToHandle(STATUS_DRAW_POS_X, HP_DRAW_POS_Y, "HP:", 0x00fa9a, Application::GetInstance()->GetFont(FONT_SIZE_20));

	if (hitStopCounter_ > 0)
	{
		// プレイヤーのhpの表示 赤
		DrawFormatStringToHandle(
			STATUS_DRAW_POS_X + HPWidth + shake,
			(HP_DRAW_POS_Y- STATUS_DRAW_POS_OFFSET) + shake,
			0xff0000,
			Application::GetInstance()->GetFont(FONT_SIZE_30),
			"%d",
			hp_);

	}
	else
	{
		// プレイヤーのhpの表示 緑
		DrawFormatStringToHandle(
			STATUS_DRAW_POS_X + HPWidth,
			HP_DRAW_POS_Y - STATUS_DRAW_POS_OFFSET,
			0x00fa9a, 
			Application::GetInstance()->GetFont(FONT_SIZE_30),
			"%d",
			hp_);
	}

	// プレイヤーのhpMaxの表示
	DrawFormatStringToHandle(
		STATUS_DRAW_POS_X + HPWidth + playerHpWidth, 
		HP_DRAW_POS_Y, 
		0x00fa9a,
		Application::GetInstance()->GetFont(FONT_SIZE_20),
		" / %d",
		PlayerStatusManager::GetInstance().GetPlayerStatus().hpMax_);



	int STAMINAWidth = GetDrawStringWidthToHandle("STAMINA: ", 9, Application::GetInstance()->GetFont(FONT_SIZE_20));
	int playerStaminaWidth = GetDrawFormatStringWidthToHandle(Application::GetInstance()->GetFont(FONT_SIZE_30), "%.f", stamina_);

	// STAMINAの表示
	DrawStringToHandle(
		STATUS_DRAW_POS_X, 
		STAMINA_DRAW_POS_Y, 
		"STAMINA:", 
		0xffc800, 
		Application::GetInstance()->GetFont(FONT_SIZE_20));

	// プレイヤースタミナの表示
	DrawFormatStringToHandle(
		STATUS_DRAW_POS_X + STAMINAWidth, 
		STAMINA_DRAW_POS_Y - STATUS_DRAW_POS_OFFSET,
		0xffc800,
		Application::GetInstance()->GetFont(FONT_SIZE_30),
		"%.f",
		stamina_);

	// スタミナMaxの表示
	DrawFormatStringToHandle(
		STATUS_DRAW_POS_X + STAMINAWidth + playerStaminaWidth,
		STAMINA_DRAW_POS_Y, 
		0xffc800,
		Application::GetInstance()->GetFont(FONT_SIZE_20),
		" / %.f",
		PlayerStatusManager::GetInstance().GetPlayerStatus().staminaMax_);

#ifdef _DEBUG

	// デバッグ表示
	DebugDraw();

#endif // _DEBUG

}

Transform* PlayerController::GetTransform()
{
	return owner_->GetComponent<Transform>();
}

CapsuleCollider* PlayerController::GetCapsule(void)
{
	return owner_->GetComponent<CapsuleCollider>();
}

int PlayerController::GetInvincibleTime(void)
{
	return invincibleTime_;
}

float PlayerController::GetMoveSpeed(void)
{
	return moveSpeed_;
}

PLAYER_STATE PlayerController::GetState(void)
{
	return stateCtrl_.state_;
}

GRABBING_STATE PlayerController::GetGrabbingState(void)
{
	return grabState_;
}

VECTOR PlayerController::GetLineStartPos(void)
{
	// カメラの位置をラインの初め座標とする
	return CameraUtility::GetCameraPos();
}

VECTOR PlayerController::GetLineEndPos(void)
{
	// 相対座標
	VECTOR LOCAL_POS =
	{ 0.0f,0.0f, PlayerStatusManager::GetInstance().GetPlayerStatus().rangeMax_ };

	// 座標に反映
	VECTOR downPos = CameraUtility::AddCameraPosLocalPos(LOCAL_POS);

	return downPos;
}

void PlayerController::StartGrabbing(float range)
{
	// 掴んだ音
	AudioManager::GetInstance()->PlaySE(SoundID::SE_GRAB);

	// 掴み状態を始める
	grabState_ = GRABBING_STATE::IS_GRABBING;

	range_ = range;
}

void PlayerController::SetWisp(Wisp* wisp)
{
	// ランタンクラスのポインタを設定
	wisp_ = wisp;
}

void PlayerController::SetGrabObject(Item* item)
{
	// アイテムクラスのポインタを設定
	grabObject_ = item;
}

void PlayerController::SetGrabObject(Cart* cart)
{
	// カートクラスのポインタを設定
	grabObject_ = cart;
}

void PlayerController::SetDamage(int damage)
{
	// 無敵時間があればダメージを与えない
	if (invincibleTime_ > 0)return;

	hp_ -= damage;

	// ビネット
	SceneManager::GetInstance()->GetShader()->SetVignettePower(1.0f);

	// プレイヤーのダメージ音
	AudioManager::GetInstance()->PlaySE(SoundID::SE_DAMAGE);

	if (hp_ <= 0)
	{
		hp_ = 0;
	}
	else
	{
		// 無敵時間を設ける
		invincibleTime_ = INVINCIBLE_TIME;

		// HP描画を揺らす
		hitStopCounter_ = SHAKE_TIME;
	}

	// プレイヤーステータスに反映
	PlayerStatusManager::GetInstance().SetHp(hp_);
}

void PlayerController::SetHitReact(VECTOR moveDir, float moveSpeed, float jumpPow)
{
	moveDir_ = moveDir;

	moveSpeed_ = moveSpeed;

	// ジャンプ力を設定
	velocityY_ = jumpPow;

	ChangeState(PLAYER_STATE_HIT_REACT);
}

void PlayerController::ChangeState(PLAYER_STATE state)
{
	// 指定されたステートへ変更
	stateCtrl_.state_ = state;

	// nullチェック
	if (stateCtrl_.initTable_[stateCtrl_.state_])
	{
		stateCtrl_.initTable_[stateCtrl_.state_](*this);
	}
}

void PlayerController::StateUpdate(void)
{
	// Transformがなければ処理しない
	if (!transform_) return;

	// nullチェック
	if (stateCtrl_.updateTable_[stateCtrl_.state_])
	{
		// 状態別更新処理
		stateCtrl_.updateTable_[stateCtrl_.state_](*this);
	}
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

void PlayerController::HealStamina(void)
{
	float staminaMax = PlayerStatusManager::GetInstance().GetPlayerStatus().staminaMax_;

	// スタミナがMaxだったら処理を飛ばす
	if (stamina_ >= staminaMax)return;

	// カウンターを進める
	staminaCounter_++;

	// しゃがみ状態だったら
	if (stateCtrl_.state_ == PLAYER_STATE_CROUCHING)
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
	if (stateCtrl_.state_ == PLAYER_STATE_HIT_REACT)return;

	// StageCollider取得
	auto stageCol = owner_->GetComponent<StageCollider>();

	if (!stageCol) return;

	// ジャンプボタンを押されたかつ、ジャンプ中では無いかつ、ジャンプ回数がMaxまで到達していなかったら
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::JUMP)
		&& jumpNum_ < PlayerStatusManager::GetInstance().GetPlayerStatus().jumpNumMax_)
	{
		// ジャンプ音
		AudioManager::GetInstance()->PlaySE(SoundID::SE_JUMP);

		// ジャンプした回数を加算
		++jumpNum_;

		// ジャンプ力を設定
		velocityY_ = JUMP_POW;

		// 接地フラグを折る
		stageCol->IsGroundFold();

		// チュートリアル時にカウンタに加算される
		SceneManager::GetInstance()->TutorialCounter(Tutorial::JUMP);
	}
}

void PlayerController::Grabbing(void)
{
	switch (grabState_)
	{
	case GRABBING_STATE::NOT_GRABBING:

		// 掴もうとしていたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::GRAB))
		{
			// 状態を変更
			grabState_ = GRABBING_STATE::TRY_GRABBING;
		}

		break;
	case GRABBING_STATE::TRY_GRABBING:

		// 掴もうとしていなくなったら
		if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::GRAB))
		{
			// 状態を変更
			grabState_ = GRABBING_STATE::NOT_GRABBING;
		}

		break;
	case GRABBING_STATE::IS_GRABBING:

		// マウスの左クリックを押されていなかったら
		if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::GRAB))
		{
			// 掴み動作を終わる
			grabState_ = GRABBING_STATE::NOT_GRABBING;

			// 中身がアイテムだったら
			// 持っている状態を終了させる
			if (GetGrabItem() != nullptr)
			{
				// アイテムを離した状態にする
				GetGrabItem()->EndGrabbed();

				// 空状態にする
				grabObject_ = std::monostate{};
			}
			// 中身がカートだったら
			else if (GetGrabCart() != nullptr)
			{
				// カートを離した状態にする
				GetGrabCart()->EndGrabbed();

				// 空状態にする
				grabObject_ = std::monostate{};
			}
		}


		// 中身が空では無かったら(アイテム)
		if (GetGrabItem() != nullptr)
		{
			// つかめる範囲に変更があったら
			if (RangeUpdate())
			{
				// アイテムに反映させる
				GetGrabItem()->SetLocalPosZ(range_);

				// チュートリアル時にカウンタに加算される
				SceneManager::GetInstance()->TutorialCounter(Tutorial::RANGE);
			}
		}
		// 中身が空では無かったら(カート)
		else if (GetGrabCart() != nullptr)
		{
			// カートとプレイヤーの距離を取る
			float distance = VSize(VSub(transform_->pos_, GetGrabCart()->GetTransform()->pos_));

			// カートとプレイヤーの距離が一定距離超えたら
			if (distance > END_GRAB_CART_DISTANCE)
			{
				// 掴み動作を終わる
				grabState_ = GRABBING_STATE::NOT_GRABBING;

				// 強制的にカートを離させる
				GetGrabCart()->EndGrabbed();

				// 空状態にする
				grabObject_ = std::monostate{};
				return;
			}

			auto capdistance = 
				VSize(VSub(owner_->GetComponent<CapsuleCollider>()->GetStart() ,
					owner_->GetComponent<CapsuleCollider>()->GetEnd()));

			capdistance += (owner_->GetComponent<CapsuleCollider>()->GetRadius()+5.0f);

			GetGrabCart()->SetLocalPos({ 0.0f,capdistance ,CART_DISTANCE });
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
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::ITEM_PUSH))
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
	else if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::ITEM_PULL))
	{
		// 物との距離を小さくする
		range_ -= EXTEND_RENGE_MOVE;

		// 最小値が超えないようにする
		if (range_ < MIN_RENGE + GetGrabItem()->GetInfo().collisionRadiusX_)
		{
			range_ = MIN_RENGE + GetGrabItem()->GetInfo().collisionRadiusX_;
		}

		// 変更があったらtrueを返す
		return true;
	}

	// 変更がなかったらfalseを返す
	return false;
}

void PlayerController::MapDrawUpdate(void)
{
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::MAP))
	{
		auto* map = owner_->GetComponent<Map>();

		// 中身が無かったら処理しない
		if (map == nullptr)	return;

		// マップが表示中なら
		if (map->GetIsDraw())
		{
			// マップを非表示にする
			map->SetIsDraw(false);
		}
		// マップが非表示中なら
		else
		{
			// マップを表示する
			map->SetIsDraw(true);

			// チュートリアル時にカウンタに加算される
			SceneManager::GetInstance()->TutorialCounter(Tutorial::MAP);
		}
	}
}

void PlayerController::DebugDraw(void)
{
	DrawFormatString(20,
		300,
		0xff0000, 
		"プレイヤー座標 : %.f,%.f,%.f",
		transform_->pos_.x, transform_->pos_.y, transform_->pos_.z);
}

void PlayerController::IdleUpdate(PlayerController& player)
{
	// 移動していたら
	if (player.InputMove())
	{
		// 移動状態にする
		player.ChangeState(PLAYER_STATE_MOVE);
	}

	// しゃがみボタンを押された
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CROUCH))
	{
		player.ChangeState(PLAYER_STATE_CROUCHING);
	}

}

void PlayerController::MoveUpdate(PlayerController& player)
{
	// スライディングの可能時間があれば
	if (player.slidingInputBufferTime > 0)
	{
		// スライディング可能時間を減らす
		player.slidingInputBufferTime--;

		if (player.slidingInputBufferTime < 0)
		{
			// 0以下にならないようにする
			player.slidingInputBufferTime = 0;
		}
	}

	// しゃがみボタンを押された
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CROUCH))
	{
		if (player.slidingInputBufferTime > 0)
		{
			// スライディング状態へ
			player.ChangeState(PLAYER_STATE_SLIDING);
		}
		else
		{
			// しゃがみ状態へ
			player.ChangeState(PLAYER_STATE_CROUCHING);
		}
		return;
	}

	// 走るボタンを押されたかつスタミナがあったら
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::DASH)
		&& player.stamina_ >= 0.1f)
	{
		// 走り状態へ
		player.ChangeState(PLAYER_STATE_DASH);
		return;
	}

	// 移動していたら
	if (player.InputMove())
	{
		// 方向×スピードで移動量を作って、座標に足して移動
		player.transform_->pos_ =
			VAdd(player.transform_->pos_,
				VScale(player.moveDir_, player.moveSpeed_));

		auto stageCol = player.owner_->GetComponent<StageCollider>();

		if (!stageCol) return;

		// 足音がなる間隔
		if (player.moveSoundInterval_ > MOVE_SOUND_INTERVAL - (player.moveSpeed_ * MOVE_SPEED_UP_MULTI))
		{
			// 接地している場合
			if (player.velocityY_ <= 0)
			{
				// 移動サウンドの再生
				AudioManager::GetInstance()->PlaySE(SoundID::SE_MOVE);
				player.moveSoundInterval_ = 0;
			}
		}
		else
		{
			player.moveSoundInterval_++;
		}
	}
	else
	{
		// 待機状態へ
		player.ChangeState(PLAYER_STATE_IDLE);
	}

	// チュートリアル時にカウンタに加算される
	SceneManager::GetInstance()->TutorialCounter(Tutorial::MOVE);
}

void PlayerController::DashUpdate(PlayerController& player)
{
	// スタミナを減らす
	player.stamina_ -= 0.1f;
	if (player.stamina_ <= 0.0f)
	{
		// 0を超えないようにする
		player.stamina_ = 0.0f;
	}

	// カウンターリセット
	player.staminaCounter_ = 0;

	// スライディング可能時間(秒数)を設定
	player.slidingInputBufferTime = SLIDING_INPUT_BUFFER_TIME;

	// しゃがみボタンを押された
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CROUCH))
	{
		// しゃがみ状態へ
		player.ChangeState(PLAYER_STATE_SLIDING);
		return;
	}

	// 走るボタンを押されていないかスタミナがなくなったら
	if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::DASH)
		|| player.stamina_ < 0.1f)
	{
		// 普通の移動状態へ
		player.ChangeState(PLAYER_STATE_MOVE);
		return;
	}

	// 移動していたら
	if (player.InputMove())
	{
		// 方向×スピードで移動量を作って、座標に足して移動
		player.transform_->pos_ =
			VAdd(player.transform_->pos_,
				VScale(player.moveDir_, player.moveSpeed_));

		auto stageCol = player.owner_->GetComponent<StageCollider>();

		if (!stageCol) return;

		// 足音がなる間隔
		if (player.moveSoundInterval_ > MOVE_SOUND_INTERVAL - (player.moveSpeed_ * MOVE_SPEED_UP_MULTI))
		{
			// 接地している場合
			if (player.velocityY_ <= 0)
			{
				// 移動サウンドの再生
				AudioManager::GetInstance()->PlaySE(SoundID::SE_MOVE);
				player.moveSoundInterval_ = 0;
			}
		}
		else
		{
			player.moveSoundInterval_++;
		}
	}
	else
	{
		// 待機状態へ
		player.ChangeState(PLAYER_STATE_IDLE);
	}

	// チュートリアル時にカウンタに加算される
	SceneManager::GetInstance()->TutorialCounter(Tutorial::DASH);
}

void PlayerController::CrouchingUpdate(PlayerController& player)
{
	auto stageCol = player.owner_->GetComponent<StageCollider>();

	// しゃがみボタンを押されてないかつ、頭に障害物がなかった場合にしゃがみを解除
	if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CROUCH)
		&& !stageCol->CeilingColl())
	{
		// 待機状態へ
		player.ChangeState(PLAYER_STATE_IDLE);
		return;
	}

	// 移動していたら
	if (player.InputMove())
	{
		// 方向×スピードで移動量を作って、座標に足して移動
		player.transform_->pos_ =
			VAdd(player.transform_->pos_,
				VScale(player.moveDir_, player.moveSpeed_));
	}
	
	// チュートリアル時にカウンタに加算される
	SceneManager::GetInstance()->TutorialCounter(Tutorial::CROUCH);
}

void PlayerController::SlidingUpdate(PlayerController& player)
{
	// スライディング状態かつ移動速度が0より大きく移動している場合
	if (player.moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		player.moveSpeed_ -= SLIDING_FRICTION;

		// END_SLIDING_SPEED以下になったら
		if (player.moveSpeed_ <= END_SLIDING_SPEED)
		{
			// しゃがみ状態にする
			player.ChangeState(PLAYER_STATE_CROUCHING);
			return;
		}

		// 方向×スピードで移動量を作って、座標に足して移動
		player.transform_->pos_ =
			VAdd(player.transform_->pos_,
				VScale(player.moveDir_, player.moveSpeed_));
	}
}

void PlayerController::HitReactUpdate(PlayerController& player)
{
	// 移動速度が0より大きく移動している場合
	if (player.moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		player.moveSpeed_ -= HIT_REACT_FRICTION;
		auto stageCol = player.owner_->GetComponent<StageCollider>();
		if (!stageCol) return;

		// スピードがゼロになるか、接地していたら
		if (player.moveSpeed_ <= 0.0f || stageCol->IsGround())
		{
			// しゃがみ状態へ
			player.ChangeState(PLAYER_STATE_CROUCHING);
			return;
		}

		// 方向×スピードで移動量を作って、座標に足して移動
		player.transform_->pos_ =
			VAdd(player.transform_->pos_,
				VScale(player.moveDir_, player.moveSpeed_));
	}
}

void PlayerController::DeadUpdate(PlayerController& player)
{
}

Item* PlayerController::GetGrabItem(void)
{
	if (auto item = std::get_if<Item*>(&grabObject_)) {
		// アイテムポインタを取り出す
		return *item;
	}

	return nullptr;
}

Cart* PlayerController::GetGrabCart(void)
{
	if (auto cart = std::get_if<Cart*>(&grabObject_)) {
		// アイテムポインタを取り出す
		return *cart;
	}

	return nullptr;
}

bool PlayerController::IsGrabbing(void)
{
	if (std::holds_alternative<std::monostate>(grabObject_)) 
	{
		// 中身がない
		return false;
	}

	// 中身がある
	return true;
}

bool PlayerController::InputMove(void)
{
	// 移動量
	VECTOR dir = Math::VECTOR_ZERO;

	// WASDで移動する
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_FORWARD)) { dir = VAdd(dir, { 0.0f, 0.0f, 1.0f }); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_LEFT)) { dir = VAdd(dir, { -1.0f, 0.0f, 0.0f }); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_BACK)) { dir = VAdd(dir, { 0.0f, 0.0f, -1.0f }); }
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::MOVE_RIGHT)) { dir = VAdd(dir, { 1.0f, 0.0f, 0.0f }); }

	if (!Math::EqualsVZero(dir))
	{
		// 正規化
		dir = VNorm(dir);

		// XYZの回転行列
		// XZ平面移動にする場合は、XZの回転を考慮しないようにする
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(CameraUtility::GetCameraAngle().y));

		// 回転行列を使用して、ベクトルを回転させる
		moveDir_ = VTransform(dir, mat);

		// 移動している
		return true;
	}

	// 移動していない
	return false;
}

void PlayerController::GetShakeOffset(int& offset)
{
	if (hitStopCounter_ > 0) {
		// 振動先をカウンターから計算する----------
		// 0 or 1
		offset = (hitStopCounter_ / 5) % 2;
		// 0 or 2　中心を作る
		offset *= 2;
		// -1 or 1　0を中心にする
		offset -= 1;
		// -3 or 3　振れ幅を付ける
		offset *= 5;
		// ----------------------------------------
	}
}

void PlayerController::IdleInit(PlayerController& player)
{
	// 移動速度を初期化
	player.moveSpeed_ = 0.0f;

	// カプセルのオフセットを初期化する
	auto cap = player.owner_->GetComponent<CapsuleCollider>();
	if (cap != nullptr)
	{
		cap->SetStartOffset(STANDING_CAP_START_OFFSET);
	}

	// ライトの範囲設定が最大値でなければ
	if (!player.wisp_->GetIsRangeMax())
	{
		// 最大値設定にする
		player.wisp_->SetIsRangeMax(true);
		// ライトアニメーションの火を元に戻す
		player.wisp_->SetAnimation(Wisp::ANIM::NORMAL);
		// ライトONサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SE_LANTERN_ON);
	}
}

void PlayerController::MoveInit(PlayerController& player)
{
	// プレイヤーの移動速度を普通の移動速度にする
	player.moveSpeed_ = DEFAULT_SPEED;
}

void PlayerController::DashInit(PlayerController& player)
{
	// プレイヤーの移動速度をダッシュの移動速度にする
	player.moveSpeed_ = PlayerStatusManager::GetInstance().GetPlayerStatus().dashMoveSpeed_;
}

void PlayerController::CrouchingInit(PlayerController& player)
{
	// プレイヤーの移動速度を普通の移動速度にする
	player.moveSpeed_ = DEFAULT_SPEED;

	// カプセルのオフセットを初期化する
	auto cap = player.owner_->GetComponent<CapsuleCollider>();
	if (cap != nullptr)
	{
		cap->SetStartOffset(CROUCHING_CAP_START_OFFSET);
	}

	// ライトの範囲設定が最大値だったら
	if (player.wisp_->GetIsRangeMax())
	{
		// ライトOFFサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SE_LANTERN_OFF);
		// ライトの範囲を最小値設定にする
		player.wisp_->SetIsRangeMax(false);
		// ライトアニメーションの火を小さく
		player.wisp_->SetAnimation(Wisp::ANIM::SMALL);
	}

	// しゃがみサウンド
	AudioManager::GetInstance()->PlaySE(SoundID::SE_CROUCH);
}

void PlayerController::SlidingInit(PlayerController& player)
{
	// プレイヤーのスライディングの移動速度とダッシュ移動速度を加算
	player.moveSpeed_ = SLIDING_SPEED + PlayerStatusManager::GetInstance().GetPlayerStatus().dashMoveSpeed_;

	// カプセルのオフセットを初期化する
	auto cap = player.owner_->GetComponent<CapsuleCollider>();
	if (cap != nullptr)
	{
		cap->SetStartOffset(CROUCHING_CAP_START_OFFSET);
	}

	// ライトの範囲設定が最大値だったら
	if (player.wisp_->GetIsRangeMax())
	{
		// ライトOFFサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SE_LANTERN_OFF);
		// ライトの範囲を最小値設定にする
		player.wisp_->SetIsRangeMax(false);
		// ライトアニメーションの火を小さく
		player.wisp_->SetAnimation(Wisp::ANIM::SMALL);
	}

	// スライディング可能時間を初期化
	player.slidingInputBufferTime = 0;

	// チュートリアル時にカウンタに加算される
	SceneManager::GetInstance()->TutorialCounter(Tutorial::SLIDING);

	// スライディングのサウンド再生
	AudioManager::GetInstance()->PlaySE(SoundID::SE_SLIDING);

}

void PlayerController::HitReactInit(PlayerController& player)
{
	auto stageCol = player.owner_->GetComponent<StageCollider>();

	if (stageCol != nullptr)
	{
		// 接地フラグを折る
		stageCol->IsGroundFold();
	}

	// ライトの範囲設定が最大値だったら
	if (player.wisp_->GetIsRangeMax())
	{
		// ライトOFFサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SE_LANTERN_OFF);
		// ライトの範囲を最小値設定にする
		player.wisp_->SetIsRangeMax(false);
		// ライトアニメーションの火を小さく
		player.wisp_->SetAnimation(Wisp::ANIM::SMALL);
	}
}

void PlayerController::DeadInit(PlayerController& player)
{
	SceneManager::GetInstance()->TrueGameOver();
}
