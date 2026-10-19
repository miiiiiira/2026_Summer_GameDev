#pragma once

class Upgrade;

class UpgradeManager
{
public:

	// シングルトン（生成・取得・削除）
	static void CreateInstance(void) { if (instance_ == nullptr) { instance_ = new UpgradeManager(); } }
	static UpgradeManager* GetInstance(void) { return instance_; }
	static void DeleteInstance(void) { if (instance_ != nullptr) { delete instance_; instance_ = nullptr; } }

private:

	static UpgradeManager* instance_;	// 静的インスタンス

	UpgradeManager(void) {};	// コンストラクタ
	~UpgradeManager(void) {};	// コンストラクタ

	// コピー・ムーブ操作を禁止
	UpgradeManager(const UpgradeManager&) = delete;
	UpgradeManager& operator=(const UpgradeManager&) = delete;
	UpgradeManager(UpgradeManager&&) = delete;
	UpgradeManager& operator=(UpgradeManager&&) = delete;

public:

	void Load(void);		// 読み込み
	void Init(void);		// 初期化
	void Update(void);		// 更新
	void Draw(void);		// 描画
	void Release(void);		// 解放

	// アップデートを渡す
	Upgrade* GetUpgrade(void) { return upgrade_; }	
	
	// アップグレードを終わらせる
	void TrueIsUpgradeEnd(void) { isUpgradeEnd_ = true; }	

	// アップグレードが終了したかどうかを渡す
	bool GetIsUpgradeEnd(void)const { return isUpgradeEnd_; }	

private:
	
	Upgrade* upgrade_ = nullptr;	// アップグレード

private:

	bool isUpgradeEnd_ = false;	// アップグレードが終了したか
};

