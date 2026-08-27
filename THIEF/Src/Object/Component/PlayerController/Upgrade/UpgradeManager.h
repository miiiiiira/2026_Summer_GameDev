#pragma once

#include "Upgrade.h"

class UpgradeManager
{
public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new UpgradeManager(); } }
	static UpgradeManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

public:

	UpgradeManager(void);	// コンストラクタ

	void Load(void);		// 読み込み
	void Init(void);		// 初期化
	void Update(void);		// 更新
	void Draw(void);		// 描画
	void Release(void);		// 解放

	Upgrade* GetUpgrade(void) { return upgrade_; }	// アップデートを渡す
	
	void TrueIsUpgradeEnd(void) { isUpgradeEnd_ = true; }	// アップグレードを終わらせる

	bool GetIsUpgradeEnd(void)const { return isUpgradeEnd_; }	// アップグレードが終了したかどうかを渡す

private:

	static UpgradeManager* instance_;	// 静的インスタンス

	// コピー・ムーブ操作を禁止
	UpgradeManager(const UpgradeManager&) = delete;
	UpgradeManager& operator=(const UpgradeManager&) = delete;
	UpgradeManager(UpgradeManager&&) = delete;
	UpgradeManager& operator=(UpgradeManager&&) = delete;

private:
	
	Upgrade* upgrade_;	// アップグレード

private:

	bool isUpgradeEnd_;	// アップグレードが終了したか
};

