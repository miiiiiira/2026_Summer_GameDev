#include "Upgrade.h"

#include<DxLib.h>
#include <random>
#include <algorithm>

#include "../../../../Common/Collision/Collision.h"
#include "../../../../Common/Manager/System/SystemManager.h"
#include "../../../../Common/Manager/Input/InputManager.h"
#include "../../../../Application.h"

Upgrade::Upgrade(void)
{
	finalizeUpgrade_.type = PLAYER_UPGRADE_TYPE::MAX;
	finalizeUpgrade_.price = 0;

	state_ = UPGRADE_STATE::NON;
	slot_ = SHOP_SLOT::MAX;
}

Upgrade::~Upgrade(void)
{
	// 使い終わったらクリア
	allUpgrades_.clear();
	selectUpgrades_.clear();
}

void Upgrade::Init(void)
{
	//imgHandle_ = LoadGraph();

	// 全てのアップグレードの種類を保持するvector
	allUpgrades_ = {
		PLAYER_UPGRADE_TYPE::HP_UP,PLAYER_UPGRADE_TYPE::STAMINA_UP,PLAYER_UPGRADE_TYPE::DASH_SPEED_UP,
		PLAYER_UPGRADE_TYPE::RANGE_UP, PLAYER_UPGRADE_TYPE::JUMP_NUM_UP,
		PLAYER_UPGRADE_TYPE::HEAL_HP_25,PLAYER_UPGRADE_TYPE::HEAL_HP_50
	};

	// vectorの上限値を決める
	selectUpgrades_.reserve(static_cast<int>(SHOP_SLOT::MAX));

	selectUpgrades_.clear();

	// 乱数生成器のセットアップ
	std::random_device rd;   // ハードウェア乱数からシードを生成
	std::mt19937 gen(rd());  // メルセンヌ・ツイスタ乱数生成器

	// 0 から (全アップグレード数 - 1) までのインデックスを等確率で生成するディストリビューション
	// ※allUpgrades_ が空でないことを前提としています
	std::uniform_int_distribution<size_t> dist(0, allUpgrades_.size() - 1);


	// ランダムの範囲(ステージ数によってランダムの範囲を高めにする)でお金を決める
	std::uniform_int_distribution<size_t> priceDist(500, 1500);

	// スロットの数分回
	for (int i = 0; i < static_cast<int>(SHOP_SLOT::MAX); ++i) {
		int randomIndex = dist(gen); // ランダムなインデックスを生成
		int price = priceDist(gen);     // ランダムな金額（数値）を生成

		// 構造体に「アップグレードのタイプ」と「決定した金額」をセットして追加
		selectUpgrades_.push_back({ allUpgrades_[randomIndex], price });
	}

	// 座標初期化
	int i = static_cast<int>(SHOP_SLOT::SHOP_SLOT_0);
	for (int y = 0; y < DRAW_NUM_Y; y++)
	{
		for (int x = 0; x < DRAW_NUM_X; x++)
		{
			pos_[i].x = POS_X + x * SPACE_X;
			pos_[i].y = POS_Y + y * SPACE_Y;
			i++;
		}
	}

	ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_0);

	// アップグレード内容を選択する
	ChangeState(Upgrade::UPGRADE_STATE::SELECT);
}

void Upgrade::Update(void)
{
	switch (state_)
	{
	case Upgrade::UPGRADE_STATE::SELECT:

		// 選択処理を行う
		SelectUpgrade();

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
	// 画像描画
	for (int i = 0; i < selectUpgrades_.size(); ++i)
	{
		DrawBox(pos_[i].x, pos_[i].y,
			pos_[i].x+COL_SIZE_X, pos_[i].y+COL_SIZE_Y,0xff0000, true);
	}
	//for (int i = 0; i < selectUpgrades_.size(); ++i)
	//{
	//	DrawGraph(pos_[i].x, pos_[i].y,
	//		imgHandle_[static_cast<int>(selectUpgrades_[i].type)], true);
	//}
}

void Upgrade::SelectUpgrade(void)
{

	if (SystemManager::GetInstance().GetIsDevice())
	{
		// 引数の座標によって選択中のものを変化させる
		MouseSelect();
	}
	else
	{
		// 選択処理
		//PadSelect();
	}
}

void Upgrade::ApplyUpgrade(void)
{
	switch (finalizeUpgrade_.type)
	{
	case PLAYER_UPGRADE_TYPE::HP_UP:

		PlayerStatusManager::GetInstance().HpUp(HP_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::STAMINA_UP:

		PlayerStatusManager::GetInstance().StaminaUp(STAMINA_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::DASH_SPEED_UP:

		PlayerStatusManager::GetInstance().DashSpeedUp(DASHSPPED_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::RANGE_UP:

		PlayerStatusManager::GetInstance().RangeUp(RANGE_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::JUMP_NUM_UP:

		PlayerStatusManager::GetInstance().JumpNumUp(JUMP_UP_NUM);

		break;
	case PLAYER_UPGRADE_TYPE::HEAL_HP_25:

		PlayerStatusManager::GetInstance().HealHp(HEAL_HP_25);

		break;
	case PLAYER_UPGRADE_TYPE::HEAL_HP_50:

		PlayerStatusManager::GetInstance().HealHp(HEAL_HP_50);

		break;
	default:
		break;
	}
}

void Upgrade::MouseSelect(void)
{
	auto prevPlace = slot_;

	// 当たり判定取る
	for (int i = 0; i < static_cast<int>(SHOP_SLOT::MAX); i++)
	{

		// 当たっていたら何かを選択しているとする
		if (Collision::HitMouse2Box(pos_[i], COL_SIZE_X, COL_SIZE_Y))
		{
			ChangeShopSlot(static_cast<SHOP_SLOT>(i));

			// 確定ボタンが押されたら処理を行う
			if(InputManager::GetInstance()->ConfirmButton())
			{
				// ここで持ってるお金が足りるかどうか確認

				// 決定したアップグレードの種類を保存
				finalizeUpgrade_.type = selectUpgrades_[static_cast<int>(slot_)].type;
				finalizeUpgrade_.price = selectUpgrades_[static_cast<int>(slot_)].price;

				// 確定に移行
				ChangeState(UPGRADE_STATE::APPLY);
			}

			break;
		}
		else
		{
			ChangeShopSlot(SHOP_SLOT::MAX);
		}
	}

}

//void Upgrade::PadSelect(void)
//{
//	auto& ins = InputManager::GetInstance();
//
//	auto prevPlace = place_;
//
//	for (int i = 0; i < static_cast<int>(PLACE::MAX); i++)
//	{
//		// 全て初期化する
//		buttonState_[i] = BUTTON_STATE::DEFAULE;
//	}
//
//	switch (place_)
//	{
//	case Upgrade::PLACE::TOP_LEFT:
//
//		if (ins.SelectDown())
//		{
//			ChangePlace(PLACE::BOTTOM_LEFT);
//		}
//
//		if (ins.SelectRightIsTrgDown())
//		{
//			ChangePlace(PLACE::TOP_RIGHT);
//		}
//
//		break;
//	case Upgrade::PLACE::TOP_RIGHT:
//
//		if (ins.SelectDown())
//		{
//			ChangePlace(PLACE::BOTTOM_RIGHT);
//		}
//
//		if (ins.SelectLeftIsTrgDown())
//		{
//			ChangePlace(PLACE::TOP_LEFT);
//		}
//
//		break;
//	case Upgrade::PLACE::BOTTOM_LEFT:
//
//		if (ins.SelectUp())
//		{
//			ChangePlace(PLACE::TOP_LEFT);
//		}
//
//		if (ins.SelectRightIsTrgDown())
//		{
//			ChangePlace(PLACE::BOTTOM_RIGHT);
//		}
//
//		break;
//	case Upgrade::PLACE::BOTTOM_RIGHT:
//
//		if (ins.SelectUp())
//		{
//			ChangePlace(PLACE::TOP_RIGHT);
//		}
//
//		if (ins.SelectLeftIsTrgDown())
//		{
//			ChangePlace(PLACE::BOTTOM_LEFT);
//		}
//
//		break;
//	case Upgrade::PLACE::MAX:
//
//		ChangePlace(PLACE::TOP_LEFT);
//		break;
//	default:
//		break;
//	}
//
//	// 見た目を選択中にする
//	buttonState_[static_cast<int>(place_)] = BUTTON_STATE::HOVER;
//
//	if (place_ != prevPlace && place_ != PLACE::MAX)
//	{
//		// 何も選択されていない状態から選択されたらSEを流す
//		SoundManager::GetInstance().Play(SoundManager::SE::SELECT);
//	}
//
//	// 何か選択していて、確定ボタンが押されたら処理を行う
//	if (ins.Confirm() && place_ != PLACE::MAX)
//	{
//		finalizeUpgrade_ = selectUpgrades_[static_cast<int>(place_)];
//
//		// 確定
//		ChangeState(STATE::APPLY);
//
//		// 決定SEをながす
//		SoundManager::GetInstance().Play(SoundManager::SE::DECIDE);
//
//	}
//}

void Upgrade::ChangeState(UPGRADE_STATE state)
{
	state_ = state;

	switch (state_)
	{
	case Upgrade::UPGRADE_STATE::SELECT:
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
	// 初期化
	finalizeUpgrade_.type = PLAYER_UPGRADE_TYPE::MAX;
	finalizeUpgrade_.price = 0;
}

