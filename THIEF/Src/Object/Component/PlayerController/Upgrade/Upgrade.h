#pragma once

#include <string>
#include <vector>
#include <utility>
#include <memory>

#include "../../Component.h"
#include "../../../../Manager/PlayerStatus/PlayerStatusManager.h"
#include "UpgradeType.h"
#include "../../../../Common/Math/Vector2.h"
#include "../../../../Application.h"

class UIManager;
class TextureManager;
class Confirm;

class Upgrade : public Component
{
public:

	// ショップに並ぶ商品の場所
	enum class SHOP_SLOT
	{
		SHOP_SLOT_0,	// 一番左の上
		SHOP_SLOT_1,	// 左から2番目の上
		SHOP_SLOT_2,	// 左から３番目の上
		SHOP_SLOT_3,	// 一番右の上
		SHOP_SLOT_4,	// 一番左の下
		SHOP_SLOT_5,	// 左から2番目の下
		SHOP_SLOT_6,	// 左から３番目の下
		SHOP_SLOT_7,	// 一番右の下
		NONE,

		END,			// ショップ終了ボタン
	};

	// 操作ステート
	enum class UPGRADE_STATE
	{
		SELECT,	// 選択
		APPLY,	// 決定
		NONE,
	};


	// アップグレードクラスの情報
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

public:

	// 最終的に選ばれたアップグレードの種類を返す
	UpgradeData GetFinalizeUpgrade(void)const { return finalizeUpgrade_; }	
	
	// ステートを渡す
	UPGRADE_STATE GetState(void)const { return state_; }	
	
	// 状態を変更する
	void ChangeState(UPGRADE_STATE state);	
	
	// 指定のショップスロットへ変更する
	void ChangeShopSlot(SHOP_SLOT slot) { slot_ = slot; }	

private:

	// 確認画面
	std::shared_ptr<Confirm> confirm_ = nullptr;	

private:

	// 画像ハンドル
	int imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::MAX)];	// 強化
	int soldOutImg_ = -1;	// 売り切れ
	int endButtonImg_ = -1;	// 終了ボタン

	// 選択されたアップグレードの表示座標
	std::vector<Vector2> pos_;	

	// 選択する前のアップグレードの全種類
	std::vector<PLAYER_UPGRADE_TYPE>allUpgrades_;

	// 選択した後のアップグレード種類、決定した金額、買われたかどうか(true / 買われてない、false / 買われた)を入れる
	std::vector<std::pair<UpgradeData, bool>> selectUpgrades_;
	
	// 最終的に選ばれた強化種類
	UpgradeData finalizeUpgrade_;
	
	// 選択された種類の添え字を保持する
	int upgradeNum_ = -1;	

	// 現在のアップグレードのステート
	UPGRADE_STATE state_ = UPGRADE_STATE::NONE;

	// 現在選択しているスロットの場所
	SHOP_SLOT slot_ = SHOP_SLOT::NONE;

private:

	// 画像ロード
	void LoadImg(void);

	// ショップに並べるアップグレードとその金額をランダムで設定する
	void InitUpgrades(void);	

	// どの能力をアップグレードするか選択を行う
	void SelectUpgrade(void);	

	// 選択処理
	void SelectMouse(void);	// マウス
	void SelectPad(void);	// パッド

	// 決定処理
	void ConfirmUpgrade(void);	

	// プレイヤーにアップグレードの指示を行う
	void ApplyUpgrade(void);	

	// 選択時の初期化
	void SelectInit(void);	

	// 確認画面を出す
	void UpdateConfirm(void);	

	// 描画系
	void DrawUpgrade(void);		// アップグレード商品
	void DrawEndButton(void);	// 終了ボタン
	void DrawPrice(void);		// 持っている金額
};