#include "UpgradeManager.h"

UpgradeManager* UpgradeManager::instance_ = nullptr;

void UpgradeManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new UpgradeManager();
	}
}

UpgradeManager& UpgradeManager::GetInstance(void)
{
	if (instance_ == nullptr)
	{
		UpgradeManager::CreateInstance();
	}
	return *instance_;
}

void UpgradeManager::Load()
{
	upgrade_ = new Upgrade();
}

void UpgradeManager::Init(void)
{
	isUpgradeEnd_ = true;

	upgrade_->Init();
}

void UpgradeManager::Update(void)
{
	if (isUpgradeEnd_)
	{
		return;
	}

	upgrade_->Update();

	if (upgrade_->GetState() == Upgrade::UPGRADE_STATE::APPLY)
	{
		ApplyUpgrade();
		isUpgradeEnd_ = true;
	}
}

void UpgradeManager::Draw(void)
{
	if (isUpgradeEnd_)
	{
		return;
	}

	upgrade_->Draw2D();
}

void UpgradeManager::Destroy(void)
{
	// アップグレードクラスの削除
	delete upgrade_;
	upgrade_ = nullptr;

	if (instance_ != nullptr)
	{
		// インスタンスのメモリ解放
		delete instance_;
		instance_ = nullptr;
	}
}

void UpgradeManager::StartIsUpgrade(void)
{
	isUpgradeEnd_ = false;

	// アップグレード内容を選択する
	upgrade_->ChangeState(Upgrade::UPGRADE_STATE::SELECT);
}

void UpgradeManager::StopIsUpgrade(void)
{
	isUpgradeEnd_ = true;

	// アップグレード内容を強制適用
	upgrade_->ChangeState(Upgrade::UPGRADE_STATE::APPLY);
}

UpgradeManager::UpgradeManager(void)
{
}

void UpgradeManager::ApplyUpgrade(void)
{
	switch (upgrade_->GetFinalizeUpgrade())
	{
	case PLAYER_UPGRADE_TYPE::HP_UP:

		PlayerStatusManager::GetInstance().HpUp(HP_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::STAMINA_UP:

		PlayerStatusManager::GetInstance().StaminaUp(STAMINA_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::DASH_SPEED_UP:

		PlayerStatusManager::GetInstance().DashSpeedUp(DASHSPPED_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::RANGE_UP:

		PlayerStatusManager::GetInstance().RangeUp(RANGE_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::JUMP_NUM_UP:

		PlayerStatusManager::GetInstance().JumpNumUp(JUMP_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::HEAL_HP_25:

		PlayerStatusManager::GetInstance().HealHp(HEAL_HP_25);

		break;
	case PLAYER_UPGRADE_TYPE::HEAL_HP_50:

		PlayerStatusManager::GetInstance().HealHp(HEAL_HP_50);

		break;
	default:
		break;
	}
	
}

