#pragma once

#include "Upgrade.h"
#include "UpgradeType.h"

class UpgradeManager
{
public:

	// 最大HPの強化値
	static constexpr float HP_UP_NUM = 20.0f;

	// スタミナ最大値強化値
	static constexpr float STAMINA_UP_NUM = 20.0f;

	// ダッシュスピード強化値
	static constexpr float DASHSPPED_UP_NUM = PlayerStatusManager::DASH_SPEED * 0.5f;

	// 掴み範囲強化値
	static constexpr float RANGE_UP_NUM = PlayerStatusManager::DEFAULT_RENGE * 0.5f;

	// ジャンプ数強化値
	static constexpr int JUMP_UP_NUM = 1;

	// HP回復値
	static constexpr float HEAL_HP_25 = 25.0f;
	static constexpr float HEAL_HP_50 = 50.0f;

	// 明示的にインステンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static UpgradeManager& GetInstance(void);

	// 初期化
	void Load(void);
	void Init(void);
	// 更新
	void Update(void);
	// 描画
	void Draw(void);

	// 解放処理
	void Destroy(void);

	void StartIsUpgrade(void);
	void StopIsUpgrade(void);
	bool GetIsUpgradeEnd(void)const { return isUpgradeEnd_; }

private:

	// 静的インスタンス
	static UpgradeManager* instance_;

	// アップグレード
	Upgrade* upgrade_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない様にする
	UpgradeManager(void);

	// コピーコンストラクタも同様
	UpgradeManager(const UpgradeManager& instance) = default;

	// デストラクタも同様
	~UpgradeManager(void) = default;

	bool isUpgradeEnd_;

	// プレイヤーにアップグレードの指示を行う
	void ApplyUpgrade(void);

};

