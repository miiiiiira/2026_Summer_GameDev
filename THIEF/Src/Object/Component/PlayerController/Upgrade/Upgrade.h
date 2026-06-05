#pragma once

#include "../../Component.h"
#include "../../../../Common/Manager/PlayerStatus/PlayerStatusManager.h"
#include "UpgradeType.h"
#include "../../../../Common/Math/Vector2.h"
#include <string>
#include <vector>
#include <utility>
#include <memory>

class UIManager;
class TextureManager;
class Confirm;

class Upgrade : public Component
{
private:

	// アルファ値
	static constexpr int ALPHA = 128;

	// 基準座標
	static constexpr float POS_X = 40.0f;
	static constexpr float POS_Y = 200.0f;

	// 当たり判定を行うサイズ
	static constexpr float COL_SIZE_X = 200.0f;
	static constexpr float COL_SIZE_Y = 100.0f;

	// 画像間(余白)の大きさ
	static constexpr float SPACE_X = COL_SIZE_X + 40.0f;
	static constexpr float SPACE_Y = COL_SIZE_Y + 100.0f;

	// 描画画像の縦横数
	static constexpr int DRAW_NUM_X = 4;
	static constexpr int DRAW_NUM_Y = 2;

	// soldOutのオフセット
	static constexpr float SOLDOUT_OFFSET_X = 11.5f;
	static constexpr float SOLDOUT_OFFSET_Y = -9.5f;

	// 最大HPの強化値
	static constexpr float HP_UP_NUM = 20.0f;

	// スタミナ最大値強化値
	static constexpr float STAMINA_UP_NUM = 20.0f;

	// ダッシュスピード強化値
	static constexpr float DASHSPPED_UP_NUM = PlayerStatusManager::DASH_SPEED * 0.2f;

	// 掴み範囲強化値
	static constexpr float RANGE_UP_NUM = PlayerStatusManager::DEFAULT_RENGE * 0.2f;

	// ジャンプ数強化値
	static constexpr int JUMP_UP_NUM = 1;

	// HP回復値
	static constexpr float HEAL_HP_25 = 25.0f;
	static constexpr float HEAL_HP_50 = 50.0f;

public:
	enum class SHOP_SLOT
	{
		SHOP_SLOT_0,
		SHOP_SLOT_1,
		SHOP_SLOT_2,
		SHOP_SLOT_3,
		SHOP_SLOT_4,
		SHOP_SLOT_5,
		SHOP_SLOT_6,
		SHOP_SLOT_7,
		MAX
	};

	enum class UPGRADE_STATE
	{
		SELECT,
		CONFIRM,
		APPLY,
		NON,
	};

	// ボタンの状態
	enum BUTTON_STATE
	{
		DEFAULE,		// 通常
		HOVER,			// 選択状態
		TRIGGER_DOWN,	// ボタン押下

		MAX,		// 最大数
	};

	struct UpgradeData
	{
		PLAYER_UPGRADE_TYPE type; // タイプ
		int price; // 金額
	};

	Upgrade(void);
	~Upgrade(void);

	void Init(void)override;
	void Update(void)override;
	void Draw2D(void)override;

	// 最終的に選ばれたアップグレードの種類を返す
	UpgradeData GetFinalizeUpgrade(void)const { return finalizeUpgrade_; }
	// ステートを渡す
	UPGRADE_STATE GetState(void)const { return state_; }

	// 状態を変更する
	void ChangeState(UPGRADE_STATE state);

	// 指定のショップスロットへ変更する
	void ChangeShopSlot(SHOP_SLOT slot) { slot_ = slot; }


private:

	// 画像
	int imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::MAX)];

	// 売り切れ画像
	int soldOutImg_;

	// 選択されたアップグレードの表示座標
	Vector2 pos_[static_cast<int>(SHOP_SLOT::MAX)];

	// 選択する前のアップグレードの全種類
	std::vector<PLAYER_UPGRADE_TYPE>allUpgrades_;
	// 選択した後のアップグレード種類、決定した金額、買われたかどうか(true / 買われてない、false / 買われた)を入れる
	std::vector<std::pair<UpgradeData, bool>> selectUpgrades_;

	UpgradeData finalizeUpgrade_;

	UPGRADE_STATE state_;
	SHOP_SLOT slot_;

	// どの能力をアップグレードするか選択を行う
	void SelectUpgrade(void);
	// プレイヤーにアップグレードの指示を行う
	void ApplyUpgrade(void);

	// マウスの選択処理
	void MouseSelect(void);

	// パッドの選択処理
	void PadSelect(void);

	void SelectInit(void);

	void Confirm(void);
};

