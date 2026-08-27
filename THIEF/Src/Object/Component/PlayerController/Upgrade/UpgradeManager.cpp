#include "UpgradeManager.h"

UpgradeManager* UpgradeManager::instance_ = nullptr;

void UpgradeManager::Load()
{
	// アップグレードクラスの生成
	upgrade_ = new Upgrade();
}

void UpgradeManager::Init(void)
{
	// 終了フラグの初期化
	isUpgradeEnd_ = false;

	// アップグレードクラスの初期化
	upgrade_->Init();
}

void UpgradeManager::Update(void)
{
	// 終了フラグが立っていたら処理を行わない
	if (isUpgradeEnd_)
	{
		return;
	}

	// アップグレードクラスの更新
	upgrade_->Update();
}

void UpgradeManager::Draw(void)
{
	// アップグレードクラスの描画
	upgrade_->Draw2D();
}

void UpgradeManager::Release(void)
{
	// アップグレードクラスの削除
	delete upgrade_;
	upgrade_ = nullptr;
}

UpgradeManager::UpgradeManager(void)
{
}


