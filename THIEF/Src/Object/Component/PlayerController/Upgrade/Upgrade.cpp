#include "Upgrade.h"

#include<DxLib.h>
#include <random>
#include <algorithm>

#include "../../../../Common/Collision/Collision.h"
#include "../../../../Common/Manager/System/SystemManager.h"
#include "../../../../Application.h"

Upgrade::Upgrade(void)
{
	// 選択決定内容をNONに初期化する
	finalizeUpgrade_ = PLAYER_UPGRADE_TYPE::MAX;

	state_ = UPGRADE_STATE::NON;
	slot_ = SHOP_SLOT::MAX;

	for (int i = 0; i < static_cast<int>(PLAYER_UPGRADE_TYPE::MAX); i++)
	{
		buttonState_[i] = BUTTON_STATE::DEFAULE;
		isTrgDown_[i] = false;
	}
}

Upgrade::~Upgrade(void)
{
	// 使い終わったらクリア
	allUpgrades_.clear();
	selectUpgrades_.clear();
}

void Upgrade::Init(void)
{
	// 全てのアップグレードの種類を保持するvector
	allUpgrades_ = {
		PLAYER_UPGRADE_TYPE::HP_UP, PLAYER_UPGRADE_TYPE::STAMINA_UP, PLAYER_UPGRADE_TYPE::DASH_SPEED_UP,
		PLAYER_UPGRADE_TYPE::RANGE_UP, PLAYER_UPGRADE_TYPE::JUMP_NUM_UP,
		PLAYER_UPGRADE_TYPE::HEAL_HP_25,PLAYER_UPGRADE_TYPE::HEAL_HP_50
	};

	// vectorの上限値を決める
	selectUpgrades_.reserve(static_cast<int>(SHOP_SLOT::MAX));

	// 選択決定内容をNONに初期化する
	finalizeUpgrade_ = PLAYER_UPGRADE_TYPE::MAX;

	// 座標初期化
	int i = static_cast<int>(SHOP_SLOT::SHOP_SLOT_0);
	for (int y = 0; y < DRAW_NUM_XY; y++)
	{
		for (int x = 0; x < DRAW_NUM_XY; x++)
		{
			posX_[i] = POS_X + x * SPACE_X;
			posY_[i] = POS_Y + y * SPACE_Y;
			i++;
		}
	}

	ChangeState(UPGRADE_STATE::NON);
	ChangeShopSlot(SHOP_SLOT::MAX);
}

void Upgrade::Update(void)
{
	switch (state_)
	{
	case Upgrade::UPGRADE_STATE::SELECT:

		// アップグレードを4つ選択
		SelectUpgrade();

		break;
	case Upgrade::UPGRADE_STATE::CONFIRM:

		// 確認を行う
		ConfirmUpgrade();

		break;
	case Upgrade::UPGRADE_STATE::APPLY:
		break;
	default:
		break;
	}

}

void Upgrade::Draw2D(void)
{
	// 背景
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, ALPHA);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0xa9a9a9, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	for (int i = 0; i < selectUpgrades_.size(); ++i)
	{
		DrawGraph(posX_[i], posY_[i],
			baseHandle_[static_cast<int>(buttonState_[i])], true);

		DrawGraph(posX_[i], posY_[i],
			textHandle_[static_cast<int>(selectUpgrades_[i])], true);
	}
}

void Upgrade::SelectUpgrade(void)
{
	selectUpgrades_.clear();
	// 乱数生成器
	std::random_device rd; // ハードウェア乱数からシードを生成
	std::mt19937 gen(rd()); // メルセンヌ・ツイスタ乱数生成器

	// allUpgrades_をシャッフル
	std::shuffle(allUpgrades_.begin(), allUpgrades_.end(), gen);

	const int max_choices = static_cast<int>(SHOP_SLOT::MAX);
	const int available_upgrades = static_cast<int>(allUpgrades_.size());

	// 先頭の４つをselectUpgrades_にコピーする
	for (int i = 0; i < max_choices; ++i) {
		selectUpgrades_.push_back(allUpgrades_[i]);
	}

	// ステートを確認に移行
	ChangeState(UPGRADE_STATE::CONFIRM);
	// 選択を左上にする
	ChangeShopSlot(SHOP_SLOT::SHOP_SLOT_0);
}

void Upgrade::ConfirmUpgrade(void)
{

	if (SystemManager::GetInstance().GetIsDevice())
	{
		// 引数の座標によって選択中のものを変化させる
		//MouseSelect();
	}
	else
	{
		// 選択処理
		//PadSelect();
	}

}


//void Upgrade::MouseSelect(void)
//{
//	auto prevPlace = slot_;
//
//	// 当たり判定取る
//	for (int i = 0; i < static_cast<int>(SHOP_SLOT::MAX); i++)
//	{
//		// 全て初期化する
//		buttonState_[i] = BUTTON_STATE::DEFAULE;
//
//		if (Collision::HitMouse2Box(pos_[i], COL_SIZE_X, COL_SIZE_Y))
//		{
//			ChangePlace(static_cast<PLACE>(i));
//
//			// 何か選択していて、確定ボタンが押されたら処理を行う
//			if(InputManager::GetInstance().ConfirmUp() && isTrgDown_[i])
//			{
//				finalizeUpgrade_ = selectUpgrades_[static_cast<int>(place_)];
//
//				// 確定に移行
//				ChangeState(STATE::APPLY);
//
//				// 決定SEをながす
//				SoundManager::GetInstance().Play(SoundManager::SE::DECIDE);
//
//			}
//			else if (InputManager::GetInstance().Confirm() && !isTrgDown_[i])
//			{
//				isTrgDown_[i] = true;
//			}
//
//			if (isTrgDown_[i])
//			{
//				buttonState_[i] = BUTTON_STATE::TRIGGER_DOWN;
//			}
//			else
//			{
//				buttonState_[i] = BUTTON_STATE::HOVER;
//			}
//
//			break;
//		}
//		else
//		{
//			ChangePlace(PLACE::MAX);
//			isTrgDown_[i] = false;
//		}
//	}
//
//}
//
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
	case Upgrade::UPGRADE_STATE::CONFIRM:
		ConfirmInit();
		break;
	case Upgrade::UPGRADE_STATE::APPLY:
		ApplyInit();
		break;
	case Upgrade::UPGRADE_STATE::NON:
		break;
	default:
		break;
	}

}

void Upgrade::SelectInit(void)
{
	// 選択決定内容をNONに初期化する
	finalizeUpgrade_ = PLAYER_UPGRADE_TYPE::MAX;

	// マウスを表示させる
	SetMouseDispFlag(true);
}

void Upgrade::ConfirmInit(void)
{
}

void Upgrade::ApplyInit(void)
{
	// マウスを表示させる
	SetMouseDispFlag(false);

	// マウスの位置を真ん中に初期化する
	SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);
}
