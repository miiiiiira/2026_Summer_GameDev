#include "PlayerStatusManager.h"

PlayerStatusManager* PlayerStatusManager::instance_ = nullptr;

void PlayerStatusManager::Destroy()
{
	// インスタンス削除
	DeleteInstance();
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

void PlayerStatusManager::HealHp(int upNum)
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
