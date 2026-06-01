#pragma once

#include "../PlayerController.h"
#include "Upgrade.h"

class UpgradeManager
{
public:

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

	// プレイヤー
	void SetPlayer(PlayerController* player);

private:

	// 静的インスタンス
	static UpgradeManager* instance_;

	// プレイヤー
	PlayerController* player_;

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
	void ApplyUpgrade(PLAYER_UPGRADE finalizeUpgrade);

};

