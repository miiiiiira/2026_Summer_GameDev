#pragma once

#include "../../Component.h"
#include "../../../../Manager/PlayerStatus/PlayerStatusManager.h"
#include "UpgradeType.h"
#include "../../../../Common/Math/Vector2.h"
#include "../../../../Application.h"
#include <string>
#include <vector>
#include <utility>
#include <memory>

class UIManager;
class TextureManager;
class Confirm;

class Upgrade : public Component
{
public:

	// ショップに並ぶ商品の場所
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
		NON,

		END,
	};

	enum class UPGRADE_STATE
	{
		SELECT,	// 選択
		APPLY,	// 決定
		NON,
	};

	struct UpgradeData
	{
		PLAYER_UPGRADE_TYPE type;	// タイプ
		int price;					// 金額
	};

public:

	Upgrade(void);				// コンストラクタ
	~Upgrade(void);				// デストラクタ

	void Init(void)override;	// 初期化
	void Update(void)override;	// 更新
	void Draw2D(void)override;	// 2D描画
	
	UpgradeData GetFinalizeUpgrade(void)const { return finalizeUpgrade_; }	// 最終的に選ばれたアップグレードの種類を返す

	UPGRADE_STATE GetState(void)const { return state_; }	// ステートを渡す

	void ChangeState(UPGRADE_STATE state);	// 状態を変更する

	void ChangeShopSlot(SHOP_SLOT slot) { slot_ = slot; }	// 指定のショップスロットへ変更する

private:
	
	void UpgradesInit(void);	// ショップに並べるアップグレードとその金額をランダムで設定する

	void SelectUpgrade(void);	// どの能力をアップグレードするか選択を行う

	void MouseSelect(void);	// マウスの選択処理
	
	void PadSelect(void);	// パッドの選択処理
	
	void ConfirmUpgrade(void);	// 決定処理

	void ApplyUpgrade(void);	// プレイヤーにアップグレードの指示を行う

	void SelectInit(void);	// 選択時の初期化

	void UpdateConfirm(void);	// 確認画面を出す

private:

	static constexpr int ALPHA = 128;	// アルファ値

	// 基準座標
	static constexpr float POS_X = 120.0f;
	static constexpr float POS_Y = 100.0f;

	// 当たり判定を行うサイズ
	static constexpr float COL_SIZE_X = 150.0f;
	static constexpr float COL_SIZE_Y = 200.0f;

	// 画像間(余白)の大きさ
	static constexpr float SPACE_X = COL_SIZE_X + 60.0f;
	static constexpr float SPACE_Y = COL_SIZE_Y + 55.0f;

	// 描画画像の縦横数
	static constexpr int DRAW_NUM_X = 4;
	static constexpr int DRAW_NUM_Y = 2;

	// soldOutのオフセット
	static constexpr float SOLDOUT_OFFSET_X = -9.0f;
	static constexpr float SOLDOUT_OFFSET_Y = 45.0f;

	// フレーム用オフセット
	static constexpr float OFFSET = 10.0f;

	// 終了ボタンの位置
	static constexpr float END_BUTTON_POS_X = Application::SCREEN_SIZE_X - 220.0f;
	static constexpr float END_BUTTON_POS_Y = Application::SCREEN_SIZE_Y - 60.0f;

	// 終了ボタンのサイズ
	static constexpr float ENDBUTOON_COL_SIZE_X = 200.0f;
	static constexpr float ENDBUTOON_COL_SIZE_Y = 45.0f;

	// 強化値
	static constexpr int HP_UP_NUM = 20;	// 最大HP
	static constexpr float STAMINA_UP_NUM = 20.0f;	// スタミナ
	static constexpr float DASHSPPED_UP_NUM = PlayerStatusManager::DASH_SPEED * 0.2f;	// ダッシュスピード
	static constexpr float RANGE_UP_NUM = PlayerStatusManager::DEFAULT_RENGE * 0.2f;	// 掴み距離
	static constexpr int JUMP_UP_NUM = 1;	// ジャンプ数強化値

	// HP回復値
	static constexpr int HEAL_HP_25 = 25;
	static constexpr int HEAL_HP_50 = 50;

private:
	
	std::shared_ptr<Confirm> confirm_;	// 確認画面

	// 画像ハンドル
	int imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::MAX)];	// 強化
	int soldOutImg_ = -1;	// 売り切れ
	int endButtonImg_ = -1;	// 終了ボタン
	
	std::vector<Vector2> pos_;	// 選択されたアップグレードの表示座標

	// 選択する前のアップグレードの全種類
	std::vector<PLAYER_UPGRADE_TYPE>allUpgrades_;
	// 選択した後のアップグレード種類、決定した金額、買われたかどうか(true / 買われてない、false / 買われた)を入れる
	std::vector<std::pair<UpgradeData, bool>> selectUpgrades_;

	UpgradeData finalizeUpgrade_;	// 最終的に選ばれた強化種類
	int upgradeNum_;	// 選択された種類の添え字を保持する

	UPGRADE_STATE state_;	// 現在のアップグレードのステート
	SHOP_SLOT slot_;		// 現在選択しているスロットの場所
};

