#include<DxLib.h>
#include <random>
#include <algorithm>

#include "../../../../Application.h"
#include "../../../../Common/Collision/Collision.h"
#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Manager/Input/InputManager.h"
#include "../../../../Manager/Score/ScoreManager.h"
#include "../../../../Common/FrameRenderer/FrameRenderer.h"
#include "../../../../Scene/Confirm/Confirm.h"
#include "../../../../Scene/SceneManager.h"
#include "UpgradeManager.h"

#include "Upgrade.h"

Upgrade::Upgrade(void)
{
	// 確認シーンポインタ初期化
	confirm_ = nullptr;	

	// 最終的に選ばれた強化種類の初期化
	finalizeUpgrade_.type = PLAYER_UPGRADE_TYPE::MAX;
	finalizeUpgrade_.price = 0;
	upgradeNum_ = -1;

	// 現在の選択している項目の初期化
	state_ = UPGRADE_STATE::NON;
	// 選択している場所の初期化
	slot_ = SHOP_SLOT::NON;
}

Upgrade::~Upgrade(void)
{
	// 画像の解放
	for (int i = 0; i < static_cast<int>(PLAYER_UPGRADE_TYPE::MAX); i++)
	{
		DeleteGraph(imgHandle_[i]);
	}

	DeleteGraph(soldOutImg_);
	soldOutImg_ = -1;
	DeleteGraph(endButtonImg_);
	endButtonImg_ = -1;

	// 使い終わったらクリア
	allUpgrades_.clear();
	selectUpgrades_.clear();
}

void Upgrade::Init(void)
{
	// 確認画面
	confirm_ = std::make_shared<Confirm>();

	// 画像のロード
	imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::HP_UP)] = LoadGraph("Data/Image/Shop/Item/hpUp.png");
	imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::STAMINA_UP)] = LoadGraph("Data/Image/Shop/Item/staminaUp.png");
	imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::DASH_SPEED_UP)] = LoadGraph("Data/Image/Shop/Item/dashSpeedUp.png");
	imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::RANGE_UP)] = LoadGraph("Data/Image/Shop/Item/rangeUp.png");
	imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::JUMP_NUM_UP)] = LoadGraph("Data/Image/Shop/Item/jumpNumUp.png");
	imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::HEAL_HP_25)] = LoadGraph("Data/Image/Shop/Item/healHp25.png");
	imgHandle_[static_cast<int>(PLAYER_UPGRADE_TYPE::HEAL_HP_50)] = LoadGraph("Data/Image/Shop/Item/healHp50.png");

	// SoldOut画像
	soldOutImg_ = LoadGraph("Data/Image/Shop/SoldOut.png");

	// 終了ボタン画像
	endButtonImg_ = LoadGraph("Data/Image/Shop/endButton.png");

	// 座標初期化
	for (int y = 0; y < DRAW_NUM_Y; y++)
	{
		for (int x = 0; x < DRAW_NUM_X; x++)
		{
			pos_.push_back({ 
				POS_X + x * SPACE_X,
				POS_Y + y * SPACE_Y });
		}
	}

	// 全てのアップグレードの種類を保持するvector
	allUpgrades_ = {
		PLAYER_UPGRADE_TYPE::HP_UP,PLAYER_UPGRADE_TYPE::STAMINA_UP,PLAYER_UPGRADE_TYPE::DASH_SPEED_UP,
		PLAYER_UPGRADE_TYPE::RANGE_UP, PLAYER_UPGRADE_TYPE::JUMP_NUM_UP,
		PLAYER_UPGRADE_TYPE::HEAL_HP_25,PLAYER_UPGRADE_TYPE::HEAL_HP_50
	};

	// ランダムでアップグレードの種類と金額を決める
	UpgradesInit();

	// ショップスロットの初期化
	ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_0);

	// ステートの初期化(選択処理にする)
	ChangeState(Upgrade::UPGRADE_STATE::SELECT);
}

void Upgrade::Update(void)
{
	switch (state_)
	{
	case Upgrade::UPGRADE_STATE::SELECT:

		// 選択処理を行う
		SelectUpgrade();

		// 決定処理
		ConfirmUpgrade();

		break;
	case Upgrade::UPGRADE_STATE::APPLY:

		// 決定された種類の強化を行う
		ApplyUpgrade();

		// 選択へ戻す
		ChangeState(Upgrade::UPGRADE_STATE::SELECT);

		break;
	default:
		break;
	}
}

void Upgrade::Draw2D(void)
{
	// フォントハンドルを取得
	int font = Application::GetInstance()->GetFont(FONT_SIZE_20);

	for (int i = 0; i < selectUpgrades_.size(); ++i)
	{
		// 画像の描画
		DrawGraphF(
			pos_[i].x, 
			pos_[i].y,
			imgHandle_[static_cast<int>(selectUpgrades_[i].first.type)], 
			true);

		// 金額の描画
		DrawFormatStringFToHandle(
			pos_[i].x + OFFSET / 2,
			pos_[i].y, 
			0x000000, 
			font,
			"%d",
			selectUpgrades_[i].first.price);

		// 売り切れていたら
		if (!selectUpgrades_[i].second)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);

			// 画像に薄いボックスを重ねる
			DrawBox(
				static_cast<int>(pos_[i].x),
				static_cast<int>(pos_[i].y),
				static_cast<int>(pos_[i].x + COL_SIZE_X),
				static_cast<int>(pos_[i].y + COL_SIZE_Y),
				0x000000, 
				true);

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			// 売り切れ画像描画
			DrawGraphF(
				pos_[i].x + SOLDOUT_OFFSET_X, 
				pos_[i].y + SOLDOUT_OFFSET_Y, 
				soldOutImg_, 
				true);
		}

		// フレームの表示
		// 終了ボタンの場合
		if (slot_ == SHOP_SLOT::END)
		{
			FrameRenderer::DrawF(
				END_BUTTON_POS_X,
				END_BUTTON_POS_Y,
				ENDBUTOON_COL_SIZE_X,
				ENDBUTOON_COL_SIZE_Y,
				OFFSET);
			continue;
		}

		// 終了ボタン以外のアップグレードの場合
		if (slot_ != SHOP_SLOT::NON && static_cast<int>(slot_) == i)
		{
			FrameRenderer::DrawF(
				pos_[i].x,
				pos_[i].y,
				COL_SIZE_X,
				COL_SIZE_Y,
				OFFSET);
		}
	}

	// 終了ボタンの描画
	DrawGraphF(END_BUTTON_POS_X, END_BUTTON_POS_Y, endButtonImg_, true);

	// 持っている金額を取得
	int price = ScoreManager::GetInstance()->GetTotalPrice();
	// フォントでの文字の横幅を取得
	int strWidth = GetDrawFormatStringWidthToHandle(font, "%d", price);
	// 金額を表示
	DrawFormatStringToHandle((Application::SCREEN_SIZE_X - strWidth) / 2, 50, 0xffffff, font,
		"%d", price);

#ifdef _DEBUG
	/*auto status = PlayerStatusManager::GetInstance().GetPlayerStatus();

		DrawFormatStringF(10, 30,
			0xffffff, "HpMax : %.0f", status.hpMax_, true);

		DrawFormatStringF(10, 50,
			0xffffff, "StaminaMax : %f", status.staminaMax_, true);

		DrawFormatStringF(10, 70,
			0xffffff, "dashMoveSpeed : %f", status.dashMoveSpeed_, true);

		DrawFormatString(10, 90,
			0xffffff, "jumpNumMax : %d", status.jumpNumMax_, true);

		DrawFormatStringF(10, 110,
			0xffffff, "rangeMax : %f", status.rangeMax_, true);*/
#endif // _DEBUG
}

void Upgrade::UpgradesInit(void)
{
	// 中身があるかもしれないため、クリア
	selectUpgrades_.clear();

	// 乱数生成器のセットアップ
	std::random_device rd;   // ハードウェア乱数からシードを生成
	std::mt19937 gen(rd());  // メルセンヌ・ツイスタ乱数生成器

	// 0 から (全アップグレード数 - 1) までのインデックスを等確率で生成する
	std::uniform_int_distribution<size_t> dist(0, allUpgrades_.size() - 1);

	// デフォルト状態
	std::uniform_int_distribution<size_t> priceDist;

	// ステージ数によってお金の範囲（最小値、最大値）を決める
	size_t minPrice = 0;
	size_t maxPrice = 0;

	// 前回のステージ数によって値段を変更
	switch (SceneManager::GetInstance()->GetPrevStage())
	{
	case STAGE_1:

		minPrice = 3000;
		maxPrice = 3500;

		break;
	case STAGE_2:

		minPrice = 2000;
		maxPrice = 3000;

		break;
	default:
		break;
	}

	// 決まった範囲を分配器にセットする
	priceDist.param(std::uniform_int_distribution<size_t>::param_type(minPrice, maxPrice));

	// HP回復の出現数の最高値を決めておく(最大2つ)
	int healNum = 2;

	// 選択されたアップグレード数がスロットの数分になるまで回す
	while (selectUpgrades_.size() < static_cast<int>(SHOP_SLOT::NON))
	{
		int randomIndex = static_cast<int>(dist(gen)); // ランダムなインデックスを生成
		int price = static_cast<int>(priceDist(gen));     // ランダムな金額（数値）を生成

		// 構造体に「アップグレードの種類」と「決定した金額」と買われたかのフラグをセットして追加
		if (allUpgrades_[randomIndex] == PLAYER_UPGRADE_TYPE::HEAL_HP_25
			|| allUpgrades_[randomIndex] == PLAYER_UPGRADE_TYPE::HEAL_HP_50)
		{
			// 回復の出現可能数が0でなかったら
			if (healNum > 0)
			{
				// 選択されたアップグレードに追加
				selectUpgrades_.push_back({ {allUpgrades_[randomIndex], price}, true });
				// 出現可能数を減らす
				--healNum;
			}
		}
		else
		{
			// 選択されたアップグレードに追加
			selectUpgrades_.push_back({ {allUpgrades_[randomIndex], price}, true });
		}
	}
}

void Upgrade::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	SHOP_SLOT prevSlot = slot_;

	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::KEY_MOUSE)
	{
		// マウス選択
		MouseSelect();
	}
	else
	{
		// パッド選択
		PadSelect();
	}

	// 中身がNONじゃないかつ、選択物が変わっていたら
	if (slot_ != SHOP_SLOT::NON
		&& slot_ != prevSlot)
	{
		// ボタンに乗ったサウンドを出す
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void Upgrade::MouseSelect(void)
{
	slot_ = SHOP_SLOT::NON;

	// 当たり判定取る
	for (int i = 0; i < selectUpgrades_.size(); i++)
	{
		// 当たっていなかったら次の処理へ
		if (!Collision::HitMouseImg2Box(pos_[i], COL_SIZE_X, COL_SIZE_Y))continue;

		// スロットの変更
		ChangeShopSlot(static_cast<SHOP_SLOT>(i));

		// 選択されたアップグレードの添え字を保存
		upgradeNum_ = i;
	}

	// 終了ボタンに当たっていたら
	if (Collision::HitMouseImg2Box({ END_BUTTON_POS_X ,END_BUTTON_POS_Y }, COL_SIZE_X, COL_SIZE_Y))
	{
		ChangeShopSlot(SHOP_SLOT::END);
	}
}

void Upgrade::PadSelect(void)
{
	switch (slot_)
	{
	case Upgrade::SHOP_SLOT::SHOP_SLOT_0:

		// 下を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			// スロット4へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_4);
		}

		// 右を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_RIGHT))
		{
			// スロット1へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_1);
		}

		break;
	case Upgrade::SHOP_SLOT::SHOP_SLOT_1:

		// 下を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			// スロット5へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_5);
		}

		// 左を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_LEFT))
		{
			// スロット0へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_0);
		}

		// 右を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_RIGHT))
		{
			// スロット2へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_2);
		}

		break;
	case Upgrade::SHOP_SLOT::SHOP_SLOT_2:

		// 下を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			// スロット6へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_6);
		}

		// 左を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_LEFT))
		{
			// スロット1へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_1);
		}

		// 右を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_RIGHT))
		{
			// スロット3へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_3);
		}

		break;
	case Upgrade::SHOP_SLOT::SHOP_SLOT_3:

		// 下を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			// スロット7へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_7);
		}

		// 左を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_LEFT))
		{
			// スロット2へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_2);
		}

		break;
	case Upgrade::SHOP_SLOT::SHOP_SLOT_4:

		// 上を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			// スロット0へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_0);
		}

		// 右を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_RIGHT))
		{
			// スロット5へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_5);
		}

		break;
	case Upgrade::SHOP_SLOT::SHOP_SLOT_5:

		// 上を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			// スロット1へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_1);
		}

		// 右を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_RIGHT))
		{
			// スロット6へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_6);
		}

		// 左を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_LEFT))
		{
			// スロット4へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_4);
		}

		break;
	case Upgrade::SHOP_SLOT::SHOP_SLOT_6:

		// 上を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			// スロット2へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_2);
		}

		// 右を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_RIGHT))
		{
			// スロット7へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_7);
		}

		// 左を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_LEFT))
		{
			// スロット5へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_5);
		}

		break;
	case Upgrade::SHOP_SLOT::SHOP_SLOT_7:

		// 上を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			// スロット3へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_3);
		}

		// 左を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_LEFT))
		{
			// スロット6へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_6);
		}

		// 下を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			// 終了ボタンへ
			ChangeShopSlot(SHOP_SLOT::END);
		}

		break;
	case Upgrade::SHOP_SLOT::NON:

		// スロット0へ
		ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_0);

		break;
	case Upgrade::SHOP_SLOT::END:

		// 上を押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			// スロット7へ
			ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_7);
		}

		break;
	default:
		break;
	}

	// 選択している物の添え字を保存しておく
	upgradeNum_ = static_cast<int>(slot_);
}

void Upgrade::ConfirmUpgrade(void)
{
	// 決定ボタンが押されていなかったら処理を行わない
	if (!InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE) && !InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::DECIDE))return;

	// 何も選択されていないなら処理を行わない
	if (slot_ == SHOP_SLOT::NON)return;

	// 終了ボタンが選択されていたら
	if (slot_ == SHOP_SLOT::END)
	{
		// アップグレードを終了させる
		UpgradeManager::GetInstance()->TrueIsUpgradeEnd();

		return;
	}

	// 買われていたら処理を行わない
	if (!selectUpgrades_[upgradeNum_].second)return;

	// お金が足りているなら
	if (ScoreManager::GetInstance()->GetTotalPrice() >= selectUpgrades_[upgradeNum_].first.price)
	{
		// 決定したアップグレードの種類を保存
		finalizeUpgrade_.type = selectUpgrades_[upgradeNum_].first.type;
		finalizeUpgrade_.price = selectUpgrades_[upgradeNum_].first.price;

		// 選択サウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);

		// 確認画面を表示
		UpdateConfirm();
	}
	else
	{
		// お金が足りないサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_NOT_MONEY);
	}
}

void Upgrade::ApplyUpgrade(void)
{
	// 本当に買うならお金を減らす
	ScoreManager::GetInstance()->SubTotalPrice(finalizeUpgrade_.price);
	// 買われたことにする
	selectUpgrades_[upgradeNum_].second = false;

	switch (finalizeUpgrade_.type)
	{
	case PLAYER_UPGRADE_TYPE::HP_UP:

		// SEを再生
		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_BUY_HP_UP);
		// プレイヤーのステータスに反映
		PlayerStatusManager::GetInstance()->HpUp(HP_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::STAMINA_UP:

		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_BUY_STAMINA_UP);
		// プレイヤーのステータスに反映
		PlayerStatusManager::GetInstance()->StaminaUp(STAMINA_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::DASH_SPEED_UP:

		// SEを再生
		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_BUY_DASH_UP);
		// プレイヤーのステータスに反映
		PlayerStatusManager::GetInstance()->DashSpeedUp(DASHSPPED_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::RANGE_UP:

		// SEを再生
		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_BUY_RANGE_UP);
		// プレイヤーのステータスに反映
		PlayerStatusManager::GetInstance()->RangeUp(RANGE_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::JUMP_NUM_UP:

		// SEを再生
		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_BUY_JUMP_UP);
		// プレイヤーのステータスに反映
		PlayerStatusManager::GetInstance()->JumpNumUp(JUMP_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::HEAL_HP_25:

		// SEを再生
		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_BUY_HEAL_25);
		// プレイヤーのステータスに反映
		PlayerStatusManager::GetInstance()->HealHp(HEAL_HP_25);

		break;
	case PLAYER_UPGRADE_TYPE::HEAL_HP_50:

		// SEを再生
		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_BUY_HEAL_50);
		// プレイヤーのステータスに反映
		PlayerStatusManager::GetInstance()->HealHp(HEAL_HP_50);

		break;
	default:
		break;
	}
}

void Upgrade::ChangeState(UPGRADE_STATE state)
{
	// 指定のステータスを設定
	state_ = state;

	switch (state_)
	{
	case Upgrade::UPGRADE_STATE::SELECT:

		// 初期化処理をする
		SelectInit();

		break;
	case Upgrade::UPGRADE_STATE::APPLY:
		break;
	case Upgrade::UPGRADE_STATE::NON:
		break;
	default:
		break;
	}

}

void Upgrade::SelectInit(void)
{
	// 最終的に選ばれた強化種類の初期化
	finalizeUpgrade_.type = PLAYER_UPGRADE_TYPE::MAX;
	finalizeUpgrade_.price = 0;
	upgradeNum_ = -1;
}

void Upgrade::UpdateConfirm(void)
{
	// 確認シーンをセット
	confirm_->ChangeType(Confirm::TYPE::BUY_UPGRADE);
	SceneManager::GetInstance()->PushScene(confirm_);
}

