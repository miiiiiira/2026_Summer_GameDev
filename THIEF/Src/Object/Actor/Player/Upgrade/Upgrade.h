#pragma once

#include <string>
#include <vector>

class UIManager;
class TextureManager;

namespace
{
	enum class PLAYER_UPGRADE
	{
		RESTOCK_POTION,
		HP_UP,
		SPEED_UP,
		STAMINA_UP,
		HEAL_HP,
		MAX,
	};

}

class Upgrade
{
public:

	// ポーションの補充値
	static constexpr float RESTOCK_POTION_NUM = 5.0f;
	// スピード強化値
	static constexpr float SPPED_UP_NUM = 5.0f;
	// スタミナ強化値
	static constexpr float STAMINA_UP_NUM = 5.0f;
	// HP強化値
	static constexpr float HP_UP_NUM = 2.0f;
	// HP回復値
	static constexpr float HEAL_HP_NUM = 2.0f;

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

	enum class PLACE
	{
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT,
		MAX
	};

	enum class STATE
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

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	float GetUpNum(PLAYER_UPGRADE upgradeType)const { return upNum_[static_cast<int>(upgradeType)]; }
	PLAYER_UPGRADE GetFinalizeUpgrade(void)const { return finalizeUpgrade_; }
	STATE GetState(void)const { return state_; }

	void ChangeState(STATE state);
	void ChangePlace(PLACE place) { place_ = place; }

private:

	// 下地
	int baseHandle_[static_cast<int>(BUTTON_STATE::MAX)];
	// テキスト
	int textHandle_[static_cast<int>(PLAYER_UPGRADE::MAX)];
	// ボタンの状態
	BUTTON_STATE buttonState_[static_cast<int>(PLAYER_UPGRADE::MAX)];
	// 今押されたか
	bool isTrgDown_[static_cast<int>(PLAYER_UPGRADE::MAX)];

	// 強化数値
	float upNum_[static_cast<int>(PLAYER_UPGRADE::MAX)];

	// 選択されたアップグレードの表示座標
	int posX_[static_cast<int>(PLACE::MAX)];
	int posY_[static_cast<int>(PLACE::MAX)];

	// 4つ選択する前のアップグレードの全種類
	std::vector<PLAYER_UPGRADE>allUpgrades_;
	// 4つ選択した後のアップグレードの全種類
	std::vector<PLAYER_UPGRADE>selectUpgrades_;

	PLAYER_UPGRADE finalizeUpgrade_;

	STATE state_;
	PLACE place_;

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

