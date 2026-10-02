#include "../../../Application.h"
#include "../../Object.h"
#include "../../../Manager/Input/InputManager.h"
#include "../../../Manager/PlayerStatus/PlayerStatusManager.h"
#include "../../../Manager/PlayerActionCounter/PlayerActionCounter.h"
#include "../../../Manager/Audio/AudioManager.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Transform/MatrixUtility.h"
#include "../../../Common/Shader/Shader.h"
#include "../Transform/Transform.h"
#include "../Collider/StageCollider/StageCollider.h"
#include "../Animation/Animation.h"
#include "../Item/Item.h"
#include "../Wisp/Wisp.h"
#include "../Cart/Cart.h"
#include "../Effect/FlashEffect.h"
#include "Map/Map.h"
#include "../../../Scene/SceneManager.h"
#include "../../../Scene/Tutorial/TutorialScene.h"

#include "PlayerController.h"

PlayerController::PlayerController(void)
{
	// テーブルに関数のポインタを格納
	// 初期化関数
	stateCtrl_.initTable_[PLAYER_STATE_IDLE] = InitIdle;
	stateCtrl_.initTable_[PLAYER_STATE_MOVE] = InitMove;
	stateCtrl_.initTable_[PLAYER_STATE_DASH] = InitDash;
	stateCtrl_.initTable_[PLAYER_STATE_CROUCHING] = InitCrouching;
	stateCtrl_.initTable_[PLAYER_STATE_SLIDING] = InitSliding;
	stateCtrl_.initTable_[PLAYER_STATE_HIT_REACT] = InitHitReact;
	stateCtrl_.initTable_[PLAYER_STATE_DEAD] = InitDead;
	// 更新関数
	stateCtrl_.updateTable_[PLAYER_STATE_IDLE] = UpdateIdle;
	stateCtrl_.updateTable_[PLAYER_STATE_MOVE] = UpdateMove;
	stateCtrl_.updateTable_[PLAYER_STATE_DASH] = UpdateDash;
	stateCtrl_.updateTable_[PLAYER_STATE_CROUCHING] = UpdateCrouching;
	stateCtrl_.updateTable_[PLAYER_STATE_SLIDING] = UpdateSliding;
	stateCtrl_.updateTable_[PLAYER_STATE_HIT_REACT] = UpdateHitReact;
	stateCtrl_.updateTable_[PLAYER_STATE_DEAD] = UpdateDead;

	// 掴み状態更新関数
	grabStateCtrl_.updateTable_[NOT_GRABBING] = UpdateNotGrabbing;
	grabStateCtrl_.updateTable_[TRY_GRABBING] = UpdateTryGrabbing;
	grabStateCtrl_.updateTable_[IS_GRABBING] = UpdateIsGrabbing;
}

// 初期化
void PlayerController::Init(void)
{
	// オーナーからTransform取得
	transform_ = owner_->GetComponent<Transform>();

	// オーナーからCapsuleCollider取得
	capColl_ = owner_->GetComponent<CapsuleCollider>();

	// オーナーからStageCollider取得
	stageColl_ = owner_->GetComponent<StageCollider>();

	// プレイヤーステータスマネージャー
	auto& status = PlayerStatusManager::GetInstance()->GetPlayerStatus();

	// 移動速度
	info_.moveSpeed_ = DEFAULT_SPEED;

	// HPの初期化
	info_.hp_ = status.hp_;

	// スタミナの初期化
	info_.stamina_ = status.staminaMax_;

	// 掴み距離の初期化
	info_.range_ = status.rangeMax_;

	// 足音を連続再生するインターバル
	info_.moveSoundInterval_ = MOVE_SOUND_INTERVAL;

	// プレイヤーの状態初期化
	ChangeState(PLAYER_STATE_IDLE);

	// 掴み状態を表すステート
	ChangeGrabState(NOT_GRABBING);
}

// 更新
void PlayerController::Update(void)
{
	// 体力が0以下になっていたら
	if (info_.hp_ <= 0)
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

	// ステージコライダーがあれば
	if (stageColl_)
	{
		// ステージの当たり判定の計算処理
		stageColl_->StageColl(info_.velocityY_);
	}

	// 掴み状態更新
	UpdateGrabState();

	// マップの表示処理
	UpdateMapDraw();

	// 無敵時間を更新
	UpdateInvincible();

	// ヒットストップ更新
	UpdateHitStop();

	// 死亡座標へ到達しているか
	IsReachedDeadPos();
}

void PlayerController::Draw2D(void)
{
	// HP描画
	DrawHP();

	// スタミナ描画
	DrawStamina();

#ifdef _DEBUG

	// デバッグ表示
	DebugDraw();

#endif // _DEBUG

}

VECTOR PlayerController::GetLineEndPos(void)
{
	// 掴みの最長距離を取得
	float rangeMax = PlayerStatusManager::GetInstance()->GetPlayerStatus().rangeMax_;

	// 相対座標
	VECTOR LOCAL_POS = { 0.0f,0.0f, rangeMax };

	// 座標に反映
	VECTOR downPos = CameraUtility::AddCameraPosLocalPos(LOCAL_POS);

	return downPos;
}

void PlayerController::StartGrabbing(float range)
{
	// 掴み状態を始める
	ChangeGrabState(IS_GRABBING);

	// 指定された距離を設定
	info_.range_ = range;

	// 掴んだ音
	AudioManager::GetInstance()->PlaySE(SoundID::SE_GRAB);
}

void PlayerController::SetDamage(int damage)
{
	// 無敵時間があればダメージを与えない
	if (info_.invincibleTime_ > 0)return;

	// HPにダメージを与える
	info_.hp_ -= damage;

	// HPが0以下になったら
	if (info_.hp_ <= 0)
	{
		// 0初期化しておく
		info_.hp_ = 0;
	}
	else
	{
		// 無敵時間を設ける
		info_.invincibleTime_ = INVINCIBLE_TIME;

		// HP描画を揺らす
		info_.hitStopCounter_ = SHAKE_TIME;
	}
	
	// ダメージ用の値にして画面周辺を暗くする
	SceneManager::GetInstance()->GetShader()->
				SetVignettePower(Shader::DAMAGE_VIGNETTE_POWER);

	// プレイヤーのダメージ音
	AudioManager::GetInstance()->PlaySE(SoundID::SE_DAMAGE);

	// プレイヤーステータスに反映
	PlayerStatusManager::GetInstance()->SetHp(info_.hp_);

	// 画面を赤くするエフェクトを付ける
	auto effect = owner_->GetComponent<FlashEffect>();
	if (effect)
	{
		effect->SetEffect(DAMAGE_EFFECT_ALPHA, DAMAGE_EFFECT_COLOR);
	}
}

void PlayerController::SetHitReact(VECTOR moveDir, float moveSpeed, float jumpPow)
{
	// 指定された移動向きを設定
	info_.moveDir_ = moveDir;

	// 指定された移動速度を設定
	info_.moveSpeed_ = moveSpeed;

	// 指定されたジャンプ力を設定
	info_.velocityY_ = jumpPow;

	// 当たった状態へ
	ChangeState(PLAYER_STATE_HIT_REACT);
}

void PlayerController::ChangeState(PLAYER_STATE state)
{
	// 指定されたステートへ変更
	stateCtrl_.state_ = state;

	// nullチェック
	if (stateCtrl_.initTable_[stateCtrl_.state_])
	{
		// 状態別初期化
		stateCtrl_.initTable_[stateCtrl_.state_](*this);
	}
}

void PlayerController::UpdateGrabState(void)
{
	// nullチェック
	if (grabStateCtrl_.updateTable_[grabStateCtrl_.state_])
	{
		// 状態別更新
		grabStateCtrl_.updateTable_[grabStateCtrl_.state_](*this);
	}
}

void PlayerController::UpdateNotGrabbing(PlayerController& player)
{
	// 掴もうとしていたら
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::GRAB))
	{
		// 掴もうとしている状態へ
		player.ChangeGrabState(TRY_GRABBING);
	}
}

void PlayerController::UpdateTryGrabbing(PlayerController& player)
{
	// 掴もうとしていなくなったら
	if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::GRAB))
	{
		// 掴もうとしていない状態へ
		player.ChangeGrabState(NOT_GRABBING);
	}
}

void PlayerController::UpdateIsGrabbing(PlayerController& player)
{
	// マウスの左クリックを押されていなかったら
	if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::GRAB))
	{
		// 掴み動作を終わる
		player.ChangeGrabState(NOT_GRABBING);

		// 中身がアイテムだったら
		// 持っている状態を終了させる
		if (player.GetGrabItem())
		{
			// アイテムを離した状態にする
			player.GetGrabItem()->EndGrabbed();

			// 空状態にする
			player.grabObject_ = std::monostate{};
		}
		// 中身がカートだったら
		else if (player.GetGrabCart())
		{
			// カートを離した状態にする
			player.GetGrabCart()->EndGrabbed();

			// 空状態にする
			player.grabObject_ = std::monostate{};
		}
	}

	// 中身が空では無かったら(アイテム)
	if (player.GetGrabItem())
	{
		// つかめる範囲に変更があったら
		if (player.UpdateRange())
		{
			// アイテムに反映させる
			player.GetGrabItem()->SetLocalPosZ(player.info_.range_);

			// チュートリアル時にカウンタに加算される
			SceneManager::GetInstance()->TutorialCounter(Tutorial::RANGE);
		}
	}
	// 中身が空では無かったら(カート)
	else if (player.GetGrabCart())
	{
		// カートとプレイヤーの距離を取る
		float distance = 
			VSize(VSub(player.transform_->pos_,
				player.GetGrabCart()->GetTransform()->pos_));

		// カートとプレイヤーの距離が一定距離超えたら
		if (distance > END_GRAB_CART_DISTANCE)
		{
			// 掴み動作を終わる
			player.ChangeGrabState(NOT_GRABBING);

			// 強制的にカートを離させる
			player.GetGrabCart()->EndGrabbed();

			// 空状態にする
			player.grabObject_ = std::monostate{};
			return;
		}
	}
}

void PlayerController::StateUpdate(void)
{
	// Transformがなければ処理しない
	if (!transform_) return;

	// nullチェック
	if (stateCtrl_.updateTable_[stateCtrl_.state_])
	{
		// 状態別更新
		stateCtrl_.updateTable_[stateCtrl_.state_](*this);
	}
}

void PlayerController::InitIdle(PlayerController& player)
{
	// 移動速度を初期化
	player.info_.moveSpeed_ = 0.0f;

	// カプセルのオフセットを初期化する
	if (player.capColl_ != nullptr)
	{
		player.capColl_->SetStartOffset(STANDING_CAP_START_OFFSET);
	}

	// ライトの範囲設定が最大値でなければ
	if (!player.wisp_->GetIsRangeMax())
	{
		// 最大値設定にする
		player.WispRangeChange(true);
	}
}

void PlayerController::InitMove(PlayerController& player)
{
	// プレイヤーの移動速度を普通の移動速度にする
	player.info_.moveSpeed_ = DEFAULT_SPEED;
}

void PlayerController::InitDash(PlayerController& player)
{
	// プレイヤーの移動速度をダッシュの移動速度にする
	player.info_.moveSpeed_ =
		PlayerStatusManager::GetInstance()->GetPlayerStatus().dashMoveSpeed_;
}

void PlayerController::InitCrouching(PlayerController& player)
{
	// プレイヤーの移動速度を普通の移動速度にする
	player.info_.moveSpeed_ = DEFAULT_SPEED;

	// カプセルのオフセットを初期化する
	if (player.capColl_ != nullptr)
	{
		player.capColl_->SetStartOffset(CROUCHING_CAP_START_OFFSET);
	}

	// ライトの範囲設定が最大値だったら
	if (player.wisp_->GetIsRangeMax())
	{
		// 最小値設定にする
		player.WispRangeChange(false);
	}

	// しゃがみサウンド
	AudioManager::GetInstance()->PlaySE(SoundID::SE_CROUCH);
}

void PlayerController::InitSliding(PlayerController& player)
{
	// プレイヤーのスライディングの移動速度とダッシュ移動速度を加算
	player.info_.moveSpeed_ = SLIDING_SPEED + PlayerStatusManager::GetInstance()->GetPlayerStatus().dashMoveSpeed_;

	// カプセルのオフセットを初期化する
	if (player.capColl_ != nullptr)
	{
		player.capColl_->SetStartOffset(CROUCHING_CAP_START_OFFSET);
	}

	// ライトの範囲設定が最大値だったら
	if (player.wisp_->GetIsRangeMax())
	{
		// 最小値設定にする
		player.WispRangeChange(false);
	}

	// スライディング可能時間を初期化
	player.info_.slidingInputBufferTime = 0;

	// チュートリアル時にカウンタに加算される
	SceneManager::GetInstance()->TutorialCounter(Tutorial::SLIDING);

	// スライディングのサウンド再生
	AudioManager::GetInstance()->PlaySE(SoundID::SE_SLIDING);

}

void PlayerController::InitHitReact(PlayerController& player)
{
	if (player.stageColl_)
	{
		// 接地フラグを折る
		player.stageColl_->IsGroundFold();
	}

	// ライトの範囲設定が最大値だったら
	if (player.wisp_->GetIsRangeMax())
	{
		// 最小値設定にする
		player.WispRangeChange(false);
	}
}

void PlayerController::InitDead(PlayerController& player)
{
	// ゲームオーバーへ
	SceneManager::GetInstance()->TrueGameOver();
}

void PlayerController::UpdateIdle(PlayerController& player)
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

void PlayerController::UpdateMove(PlayerController& player)
{
	// スライディングの可能時間があれば
	if (player.info_.slidingInputBufferTime > 0)
	{
		// スライディング可能時間を減らす
		player.info_.slidingInputBufferTime--;

		if (player.info_.slidingInputBufferTime < 0)
		{
			// 0以下にならないようにする
			player.info_.slidingInputBufferTime = 0;
		}
	}

	// しゃがみボタンを押された
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CROUCH))
	{
		if (player.info_.slidingInputBufferTime > 0)
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
		&& player.info_.stamina_ >= 0.1f)
	{
		// 走り状態へ
		player.ChangeState(PLAYER_STATE_DASH);
		return;
	}

	// 移動していたら
	if (player.InputMove())
	{
		// 指定された方向と移動速度を使用し座標に反映
		player.Move();

		if (!player.stageColl_) return;

		// 足音がなる間隔
		if (player.info_.moveSoundInterval_ > MOVE_SOUND_INTERVAL - (player.info_.moveSpeed_ * MOVE_SPEED_UP_MULTI))
		{
			// 接地している場合
			if (player.info_.velocityY_ <= 0)
			{
				// 移動サウンドの再生
				AudioManager::GetInstance()->PlaySE(SoundID::SE_MOVE);
				player.info_.moveSoundInterval_ = 0;
			}
		}
		else
		{
			player.info_.moveSoundInterval_++;
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

void PlayerController::UpdateDash(PlayerController& player)
{
	// スタミナを減らす
	player.info_.stamina_ -= 0.1f;
	if (player.info_.stamina_ <= 0.0f)
	{
		// 0を超えないようにする
		player.info_.stamina_ = 0.0f;
	}

	// カウンターリセット
	player.info_.staminaCounter_ = 0;

	// スライディング可能時間(秒数)を設定
	player.info_.slidingInputBufferTime = SLIDING_INPUT_BUFFER_TIME;

	// しゃがみボタンを押された
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CROUCH))
	{
		// しゃがみ状態へ
		player.ChangeState(PLAYER_STATE_SLIDING);
		return;
	}

	// 走るボタンを押されていないかスタミナがなくなったら
	if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::DASH)
		|| player.info_.stamina_ < 0.1f)
	{
		// 普通の移動状態へ
		player.ChangeState(PLAYER_STATE_MOVE);
		return;
	}

	// 移動していたら
	if (player.InputMove())
	{
		// 指定された方向と移動速度を使用し座標に反映
		player.Move();

		if (!player.stageColl_) return;

		// 足音がなる間隔
		if (player.info_.moveSoundInterval_ > MOVE_SOUND_INTERVAL - (player.info_.moveSpeed_ * MOVE_SPEED_UP_MULTI))
		{
			// 接地している場合
			if (player.info_.velocityY_ <= 0)
			{
				// 移動サウンドの再生
				AudioManager::GetInstance()->PlaySE(SoundID::SE_MOVE);
				player.info_.moveSoundInterval_ = 0;
			}
		}
		else
		{
			player.info_.moveSoundInterval_++;
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

void PlayerController::UpdateCrouching(PlayerController& player)
{
	// しゃがみボタンを押されてないかつ、頭に障害物がなかった場合にしゃがみを解除
	if (!InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::CROUCH)
		&& player.stageColl_
		&& !player.stageColl_->CeilingColl())
	{
		// 待機状態へ
		player.ChangeState(PLAYER_STATE_IDLE);
		return;
	}

	// 移動していたら
	if (player.InputMove())
	{
		// 指定された方向と移動速度を使用し座標に反映
		player.Move();
	}

	// チュートリアル時にカウンタに加算される
	SceneManager::GetInstance()->TutorialCounter(Tutorial::CROUCH);
}

void PlayerController::UpdateSliding(PlayerController& player)
{
	// スライディング状態かつ移動速度が0より大きく移動している場合
	if (player.info_.moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		player.info_.moveSpeed_ -= SLIDING_FRICTION;

		// END_SLIDING_SPEED以下になったら
		if (player.info_.moveSpeed_ <= END_SLIDING_SPEED)
		{
			// しゃがみ状態にする
			player.ChangeState(PLAYER_STATE_CROUCHING);
			return;
		}

		// 指定された方向と移動速度を使用し座標に反映
		player.Move();
	}
}

void PlayerController::UpdateHitReact(PlayerController& player)
{
	// 移動速度が0より大きく移動している場合
	if (player.info_.moveSpeed_ > 0.0f)
	{
		// 移動速度を減算
		player.info_.moveSpeed_ -= HIT_REACT_FRICTION;

		if (!player.stageColl_) return;

		// スピードがゼロになるか、接地していたら
		if (player.info_.moveSpeed_ <= 0.0f || player.stageColl_->IsGround())
		{
			// しゃがみ状態へ
			player.ChangeState(PLAYER_STATE_CROUCHING);
			return;
		}

		// 指定された方向と移動速度を使用し座標に反映
		player.Move();
	}
}

void PlayerController::UpdateDead(PlayerController& player)
{
}

void PlayerController::ApplyGravity(void)
{
	// ステージコライダが無ければ処理を行わない
	if (!stageColl_) return;

	// Y座標へ反映
	transform_->pos_.y += info_.velocityY_;

	// 接地判定
	
	// 空中
	if (!stageColl_->IsGround())
	{
		// 重力加算
		info_.velocityY_ += GRAVITY;
		
		// 最大落下速度
		if (info_.velocityY_ < MAX_FALL)
			info_.velocityY_ = MAX_FALL;
	}
	else
	{
		// 地面上なら少し下方向に押す
		// 0だと浮く場合があるため
		info_.velocityY_ = -0.1f;

		// ジャンプした回数を初期化
		info_.jumpNum_ = 0;
	}
}

void PlayerController::HealStamina(void)
{
	// スタミナの最大値を取得
	float staminaMax = 
		PlayerStatusManager::GetInstance()->GetPlayerStatus().staminaMax_;

	// スタミナがMaxだったら処理を飛ばす
	if (info_.stamina_ >= staminaMax)return;

	// カウンターを進める
	info_.staminaCounter_++;

	// しゃがみ状態かスタミナ回復を行うまでの制限時間を超えたら入る
	if (stateCtrl_.state_ == PLAYER_STATE_CROUCHING
		|| info_.staminaCounter_ >= RECOVERY_STAMINA_WAIT_TIME)
	{
		// スタミナ回復させる
		info_.stamina_ += RECOVERY_STAMINA;

		if (info_.stamina_ > staminaMax)
		{
			// 最大スタミナを超えないようにする
			info_.stamina_ = staminaMax;
		}
	}
}

void PlayerController::Jump(void)
{
	// ダメージ時は処理を行わない
	if (stateCtrl_.state_ == PLAYER_STATE_HIT_REACT)return;

	if (!stageColl_) return;

	// ジャンプボタンを押されたかつ、ジャンプ中では無いかつ、ジャンプ回数がMaxまで到達していなかったら
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::JUMP)
		&& info_.jumpNum_ < PlayerStatusManager::GetInstance()->GetPlayerStatus().jumpNumMax_)
	{
		// ジャンプした回数を加算
		++info_.jumpNum_;

		// ジャンプ力を設定
		info_.velocityY_ = JUMP_POW;

		// 接地フラグを折る
		stageColl_->IsGroundFold();

		// ジャンプ音
		AudioManager::GetInstance()->PlaySE(SoundID::SE_JUMP);

		// チュートリアル時にカウンタに加算される
		SceneManager::GetInstance()->TutorialCounter(Tutorial::JUMP);
	}
}

bool PlayerController::UpdateRange(void)
{
	// 掴む距離の最大値を取得
	float rangeMax = 
		PlayerStatusManager::GetInstance()->GetPlayerStatus().rangeMax_;

	// 物との距離を大きくする操作が行われていたら
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::ITEM_PUSH))
	{
		// 物との距離を大きくする
		info_.range_ += EXTEND_RENGE_MOVE;

		// 最大値を超えないようにする
		if (info_.range_ > rangeMax)
		{
			info_.range_ = rangeMax;
		}

		// 変更があったらtrueを返す
		return true;
	}
	// 物との距離を小さくする操作が行われていたら
	else if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::ITEM_PULL))
	{
		// 物との距離を小さくする
		info_.range_ -= EXTEND_RENGE_MOVE;

		// 最小値を超えないようにする
		if (info_.range_ < MIN_RENGE + GetGrabItem()->GetInfo().collisionRadiusX_)
		{
			info_.range_ = MIN_RENGE + GetGrabItem()->GetInfo().collisionRadiusX_;
		}

		// 変更があったらtrueを返す
		return true;
	}

	// 変更がなかったらfalseを返す
	return false;
}

void PlayerController::UpdateMapDraw(void)
{
	// マップボタンを押されたら
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::MAP))
	{
		// マップを取得
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

void PlayerController::UpdateInvincible(void)
{
	// 無敵時間を減らす
	if (info_.invincibleTime_ > 0)
	{
		--info_.invincibleTime_;
	}
	else
	{
		// 無敵時間が終わったら、画面周辺の暗さを元に戻す
		SceneManager::GetInstance()->GetShader()->
				SetVignettePower(Shader::DEFAULT_VIGNETTE_POWER);
	}
}

void PlayerController::UpdateHitStop(void)
{
	// ヒットストップ更新処理
	if (info_.hitStopCounter_ > 0) {
		info_.hitStopCounter_--;
	}
}

void PlayerController::GetShakeOffset(int& offset)
{
	if (info_.hitStopCounter_ > 0) {
		// 振動先をカウンターから計算する----------
		// 0 or 1
		offset = (info_.hitStopCounter_ / 5) % 2;
		// 0 or 2　中心を作る
		offset *= 2;
		// -1 or 1　0を中心にする
		offset -= 1;
		// -3 or 3　振れ幅を付ける
		offset *= 5;
		// ----------------------------------------
	}
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
		// カートポインタを取り出す
		return *cart;
	}

	return nullptr;
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
		info_.moveDir_ = VTransform(dir, mat);

		// 移動している
		return true;
	}

	// 移動していない
	return false;
}

void PlayerController::Move(void)
{
	// 方向×スピードで移動量を作って、座標に足して移動
	transform_->pos_ =
		VAdd(transform_->pos_,
			VScale(info_.moveDir_, info_.moveSpeed_));
}

void PlayerController::WispRangeChange(bool flg)
{
	// 範囲を設定する
	wisp_->SetIsRangeMax(flg);

	if (flg)
	{
		// ライトアニメーションの火を元に戻す
		wisp_->SetAnimation(Wisp::ANIM::NORMAL);
		// ライトONサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SE_LANTERN_ON);
	}
	else
	{
		// ライトアニメーションの火を小さく
		wisp_->SetAnimation(Wisp::ANIM::SMALL);
		// ライトOFFサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SE_LANTERN_OFF);
	}
}

void PlayerController::IsReachedDeadPos(void)
{
	// 一定の座標いったら
	if (transform_->pos_.y < DEAD_POS_Y)
	{
		info_.hp_ = 0;
		return;
	}
}

void PlayerController::DrawHP(void)
{
	// 指定フォントでの文字の横幅を取得
	int HPWidth = 
		GetDrawStringWidthToHandle(
			"HP: ", 
			4, 
			Application::GetInstance()->GetFont(FONT_SIZE_20));
	int playerHpWidth = 
		GetDrawFormatStringWidthToHandle(
			Application::GetInstance()->GetFont(FONT_SIZE_30),
			"%d",
			info_.hp_);

	// HPの表示
	DrawStringToHandle(
		STATUS_DRAW_POS_X,
		HP_DRAW_POS_Y, 
		"HP:", 
		HP_COLOR,
		Application::GetInstance()->GetFont(FONT_SIZE_20));

	// ヒットストップカウンタが0じゃない場合(ダメージを受けている状態)
	if (info_.hitStopCounter_ > 0)
	{
		int shake = 0;
		// ヒットストップカウンタが0じゃない場合に揺らし量を計算
		GetShakeOffset(shake);

		// プレイヤーのhpの表示 赤
		DrawFormatStringToHandle(
			STATUS_DRAW_POS_X + HPWidth + shake,
			(HP_DRAW_POS_Y - STATUS_DRAW_POS_OFFSET) + shake,
			DAMAGE_HP_COLOR,
			Application::GetInstance()->GetFont(FONT_SIZE_30),
			"%d",
			info_.hp_);
	}
	else
	{
		// プレイヤーのhpの表示 緑
		DrawFormatStringToHandle(
			STATUS_DRAW_POS_X + HPWidth,
			HP_DRAW_POS_Y - STATUS_DRAW_POS_OFFSET,
			HP_COLOR,
			Application::GetInstance()->GetFont(FONT_SIZE_30),
			"%d",
			info_.hp_);
	}

	// プレイヤーのhpMaxの表示
	DrawFormatStringToHandle(
		STATUS_DRAW_POS_X + HPWidth + playerHpWidth,
		HP_DRAW_POS_Y,
		HP_COLOR,
		Application::GetInstance()->GetFont(FONT_SIZE_20),
		" / %d",
		PlayerStatusManager::GetInstance()->GetPlayerStatus().hpMax_);
}

void PlayerController::DrawStamina(void)
{
	// 指定フォントでの文字の横幅を取得
	int STAMINAWidth = 
		GetDrawStringWidthToHandle(
			"STAMINA: ",
			9, 
			Application::GetInstance()->GetFont(FONT_SIZE_20));
	int playerStaminaWidth = 
		GetDrawFormatStringWidthToHandle(
			Application::GetInstance()->GetFont(FONT_SIZE_30),
			"%.f",
			info_.stamina_);

	// STAMINAの表示
	DrawStringToHandle(
		STATUS_DRAW_POS_X,
		STAMINA_DRAW_POS_Y,
		"STAMINA:",
		STAMINA_COLOR,
		Application::GetInstance()->GetFont(FONT_SIZE_20));

	// プレイヤースタミナの表示
	DrawFormatStringToHandle(
		STATUS_DRAW_POS_X + STAMINAWidth,
		STAMINA_DRAW_POS_Y - STATUS_DRAW_POS_OFFSET,
		STAMINA_COLOR,
		Application::GetInstance()->GetFont(FONT_SIZE_30),
		"%.f",
		info_.stamina_);

	// スタミナMaxの表示
	DrawFormatStringToHandle(
		STATUS_DRAW_POS_X + STAMINAWidth + playerStaminaWidth,
		STAMINA_DRAW_POS_Y,
		STAMINA_COLOR,
		Application::GetInstance()->GetFont(FONT_SIZE_20),
		" / %.f",
		PlayerStatusManager::GetInstance()->GetPlayerStatus().staminaMax_);
}

void PlayerController::DebugDraw(void)
{
	// プレイヤー座標表示
	DrawFormatString(
		20,
		300,
		0xff0000,
		"プレイヤー座標 : %.f,%.f,%.f",
		transform_->pos_.x,
		transform_->pos_.y,
		transform_->pos_.z);
}