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
	isUpgradeEnd_ = false;

	upgrade_->Init();
}

void UpgradeManager::Update(void)
{
	if (isUpgradeEnd_)
	{
		return;
	}

	upgrade_->Update();
}

void UpgradeManager::Draw(void)
{
	if (isUpgradeEnd_)
	{
		return;
	}

	upgrade_->Draw2D();
}

void UpgradeManager::Release(void)
{
	// アップグレードクラスの削除
	delete upgrade_;
	upgrade_ = nullptr;
}

void UpgradeManager::Destroy(void)
{
	if (instance_ != nullptr)
	{
		// インスタンスのメモリ解放
		delete instance_;
		instance_ = nullptr;
	}
}

UpgradeManager::UpgradeManager(void)
{
}


