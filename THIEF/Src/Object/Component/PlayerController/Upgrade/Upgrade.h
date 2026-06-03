#pragma once

#include "../../Component.h"
#include "../../../../Common/Manager/PlayerStatus/PlayerStatusManager.h"
#include "UpgradeType.h"
#include <string>
#include <vector>

class UIManager;
class TextureManager;

class Upgrade : public Component
{
private:

	// アルファ値
	static constexpr int ALPHA = 128;

	// 基準座標
	static constexpr int POS_X = 300;
	static constexpr int POS_Y = 200;

	// 当たり判定を行うサイズ
	static constexpr int COL_SIZE_X = 256;
	static constexpr int COL_SIZE_Y = 128;

	// 画像間(余白)の大きさ
	static constexpr int SPACE_X = COL_SIZE_X + 150;
	static constexpr int SPACE_Y = COL_SIZE_Y + 50;

	// 描画画像の縦横数
	static constexpr int DRAW_NUM_XY = 2;

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

	Upgrade(void);
	~Upgrade(void);

	void Init(void)override;
	void Update(void)override;
	void Draw2D(void)override;

	// 最終的に選ばれたアップグレードの種類を返す
	PLAYER_UPGRADE_TYPE GetFinalizeUpgrade(void)const { return finalizeUpgrade_; }
	// ステートを渡す
	UPGRADE_STATE GetState(void)const { return state_; }

	// 状態を変更する
	void ChangeState(UPGRADE_STATE state);

	// 指定のショップスロットへ変更する
	void ChangeShopSlot(SHOP_SLOT slot) { slot_ = slot; }

private:

	// 下地
	int baseHandle_[static_cast<int>(BUTTON_STATE::MAX)];
	// テキスト
	int textHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::MAX)];
	// ボタンの状態
	BUTTON_STATE buttonState_[static_cast<int>(PLAYER_UPGRADE_TYPE::MAX)];
	// 今押されたか
	bool isTrgDown_[static_cast<int>(PLAYER_UPGRADE_TYPE::MAX)];

	// 選択されたアップグレードの表示座標
	int posX_[static_cast<int>(SHOP_SLOT::MAX)];
	int posY_[static_cast<int>(SHOP_SLOT::MAX)];

	// 4つ選択する前のアップグレードの全種類
	std::vector<PLAYER_UPGRADE_TYPE>allUpgrades_;
	// 4つ選択した後のアップグレードの全種類
	std::vector<PLAYER_UPGRADE_TYPE>selectUpgrades_;

	PLAYER_UPGRADE_TYPE finalizeUpgrade_;

	UPGRADE_STATE state_;
	SHOP_SLOT slot_;

	// どの能力をアップグレードするか選択を行う
	void SelectUpgrade(void);
	void ConfirmUpgrade(void);

	// マウスの選択処理
	void MouseSelect(void);

	// パッドの選択処理
	void PadSelect(void);

	void SelectInit(void);
	void ConfirmInit(void);
	void ApplyInit(void);
};

