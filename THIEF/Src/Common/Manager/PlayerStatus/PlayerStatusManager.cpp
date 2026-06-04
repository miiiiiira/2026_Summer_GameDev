#include "PlayerStatusManager.h"
#include <DxLib.h>

PlayerStatusManager* PlayerStatusManager::instance_ = nullptr;

void PlayerStatusManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new PlayerStatusManager();
	}
}

PlayerStatusManager& PlayerStatusManager::GetInstance(void)
{
	return *instance_;
}

void PlayerStatusManager::Update(void)
{
	
}

void PlayerStatusManager::Destroy()
{
	delete instance_;
	instance_ = nullptr;
}

void PlayerStatusManager::ResetStatus()
{
	// HPの初期化
	status_.hp_ = status_.hpMax_ = DEFAULT_HP;

	// プレイヤーのダッシュ時の移動速度の初期化
	status_.dashMoveSpeed_ = DASH_SPEED;

	// スタミナの初期化
	status_.staminaMax_ = DEFAULT_STAMINA;

	// ジャンプ可能数の初期化
	status_.jumpNumMax_ = DEFAULT_JUMP_NUM;

	// 掴み距離の初期化
	status_.rangeMax_ = DEFAULT_RENGE;
}

void PlayerStatusManager::HpUp(float upNum)
{
	// HPの最大値を上げる
	status_.hp_ += upNum;
	status_.hpMax_ += upNum;
}

void PlayerStatusManager::StaminaUp(float upNum)
{
	// スタミナの最大値を上げる
	status_.staminaMax_ += upNum;
}

void PlayerStatusManager::DashSpeedUp(float upNum)
{
	// ダッシュ時のスピードを上げる
	status_.dashMoveSpeed_ += upNum;
}

void PlayerStatusManager::RangeUp(float upNum)
{
	// 掴みの範囲を大きくする
	status_.rangeMax_ += upNum;
}

void PlayerStatusManager::JumpNumUp(int upNum)
{
	// ジャンプの最大値を上げる
	status_.jumpNumMax_ += upNum;
}

void PlayerStatusManager::HealHp(float upNum)
{
	// HPを回復する
	status_.hp_ += upNum;

	// HPの最大値を超えないようにする
	if (status_.hp_ > status_.hpMax_)
	{
		status_.hp_ = status_.hpMax_;
	}
}

PlayerStatusManager::PlayerStatusManager(void)
{
	ResetStatus();
}
