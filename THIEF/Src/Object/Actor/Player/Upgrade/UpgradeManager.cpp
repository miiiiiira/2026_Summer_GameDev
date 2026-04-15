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

void UpgradeManager::Load(Player* player)
{
	player_ = player;

	upgrade_ = new Upgrade();
	upgrade_->Load();
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

	if (upgrade_->GetState() == Upgrade::STATE::APPLY)
	{
		auto finalizeUpgrade = upgrade_->GetFinalizeUpgrade();

		ApplyUpgrade(finalizeUpgrade);
		isUpgradeEnd_ = true;
	}
}

void UpgradeManager::Draw(void)
{
	if (isUpgradeEnd_)
	{
		return;
	}

	upgrade_->Draw();
}

void UpgradeManager::Destroy(void)
{
	// アップグレードクラスの削除
	upgrade_->Release();
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
	upgrade_->ChangeState(Upgrade::STATE::SELECT);
}

void UpgradeManager::StopIsUpgrade(void)
{
	isUpgradeEnd_ = true;

	// アップグレード内容を強制適用
	upgrade_->ChangeState(Upgrade::STATE::APPLY);
}

UpgradeManager::UpgradeManager(void)
{
}

void UpgradeManager::ApplyUpgrade(PLAYER_UPGRADE finalizeUpgrade)
{
	// プレイヤーに強化指示を出し能力強化を反映する
	//player_->Upgrade(finalizeUpgrade, upgrade_->GetUpNum(finalizeUpgrade));
}

