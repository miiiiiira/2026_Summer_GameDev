#include "Upgrade.h"

#include<DxLib.h>
#include <random>
#include <algorithm>

//#include "../../../Utility/Collision/CollisionUtility.h"
//#include "../../../Manager/SoundManager.h"
#include "../../../../System/SystemManager.h"
#include "../../../../Application.h"

Upgrade::Upgrade(void)
{
	// 選択決定内容をNONに初期化する
	finalizeUpgrade_ = PLAYER_UPGRADE::MAX;

	state_ = STATE::NON;
	place_ = PLACE::MAX;

	for (int i = 0; i < static_cast<int>(PLAYER_UPGRADE::MAX); i++)
	{
		buttonState_[i] = BUTTON_STATE::DEFAULE;
		isTrgDown_[i] = false;
	}
}

Upgrade::~Upgrade(void)
{
}

void Upgrade::Load(void)
{
	//// ベースロード
	//baseHandle_[static_cast<int>(BUTTON_STATE::DEFAULE)] = LoadGraph("Data/Image/UI/Upgrade/Base_0.png");
	//baseHandle_[static_cast<int>(BUTTON_STATE::HOVER)] = LoadGraph("Data/Image/UI/Upgrade/Base_1.png");
	//baseHandle_[static_cast<int>(BUTTON_STATE::TRIGGER_DOWN)] = LoadGraph("Data/Image/UI/Upgrade/Base_2.png");

	//// テキストロード
	//textHandle_[static_cast<int>(PLAYER_UPGRADE::HP_UP)] = LoadGraph("Data/Image/UI/Upgrade/HP_UP.png");
	//textHandle_[static_cast<int>(PLAYER_UPGRADE::HEAL_HP)] = LoadGraph("Data/Image/UI/Upgrade/HP_HEAL.png");
	//textHandle_[static_cast<int>(PLAYER_UPGRADE::SPEED_UP)] = LoadGraph("Data/Image/UI/Upgrade/SPEED_UP.png");
	//textHandle_[static_cast<int>(PLAYER_UPGRADE::STAMINA_UP)] = LoadGraph("Data/Image/UI/Upgrade/STAMINA_UP.png");
	//textHandle_[static_cast<int>(PLAYER_UPGRADE::RESTOCK_POTION)] = LoadGraph("Data/Image/UI/Upgrade/POTION_RESTOCK.png");
}

void Upgrade::Init(void)
{
	for (int i = 0; i < static_cast<int>(PLAYER_UPGRADE::MAX); i++)
	{
		switch (i)
		{
		case 0:
			upNum_[i] = RESTOCK_POTION_NUM;
			break;
		case 1:
			upNum_[i] = SPPED_UP_NUM;
			break;
		case 2:
			upNum_[i] = STAMINA_UP_NUM;
			break;
		case 3:
			upNum_[i] = HP_UP_NUM;
			break;
		case 4:
			upNum_[i] = HEAL_HP_NUM;
			break;
		default:
			break;
		}
	}

	// 全ての本の種類を保持するvector
	allUpgrades_ = {
		PLAYER_UPGRADE::RESTOCK_POTION, PLAYER_UPGRADE::SPEED_UP, PLAYER_UPGRADE::STAMINA_UP,
		PLAYER_UPGRADE::HP_UP, PLAYER_UPGRADE::HEAL_HP,
	};

	// vectorの上限値を決める
	selectUpgrades_.reserve(static_cast<int>(PLACE::MAX));

	// 選択決定内容をNONに初期化する
	finalizeUpgrade_ = PLAYER_UPGRADE::MAX;

	// 座標初期化
	int i = static_cast<int>(PLACE::TOP_LEFT);
	for (int y = 0; y < DRAW_NUM_XY; y++)
	{
		for (int x = 0; x < DRAW_NUM_XY; x++)
		{
			posX_[i] = POS_X + x * SPACE_X;
			posY_[i] = POS_Y + y * SPACE_Y;
			i++;
		}
	}

	ChangeState(STATE::NON);
	ChangePlace(PLACE::MAX);
}

void Upgrade::Update(void)
{
	switch (state_)
	{
	case Upgrade::STATE::SELECT:

		// アップグレードを4つ選択
		SelectUpgrade();

		break;
	case Upgrade::STATE::CONFIRM:

		// 確認を行う
		ConfirmUpgrade();

		break;
	case Upgrade::STATE::APPLY:
		break;
	default:
		break;
	}

}

void Upgrade::Draw(void)
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

void Upgrade::Release(void)
{
	// 使い終わったらクリア
	allUpgrades_.clear();
	selectUpgrades_.clear();
}

void Upgrade::SelectUpgrade(void)
{
	selectUpgrades_.clear();
	// 乱数生成器
	std::random_device rd; // ハードウェア乱数からシードを生成
	std::mt19937 gen(rd()); // メルセンヌ・ツイスタ乱数生成器

	// allUpgrades_をシャッフル
	std::shuffle(allUpgrades_.begin(), allUpgrades_.end(), gen);

	const int max_choices = static_cast<int>(PLACE::MAX);
	const int available_upgrades = static_cast<int>(allUpgrades_.size());

	// std::min を std:: のスコープで明示的に呼び出す(基本max_choicesのほうが大きくなるが、vectorのsize外に出ないように比較しておく)
	int limit = (std::min)(max_choices, available_upgrades);

	// 先頭の４つをselectUpgrades_にコピーする
	for (int i = 0; i < limit; ++i) {
		selectUpgrades_.push_back(allUpgrades_[i]);
	}

	// ステートを確認に移行
	ChangeState(STATE::CONFIRM);
	// 選択を左上にする
	ChangePlace(PLACE::TOP_LEFT);
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

//
//void Upgrade::MouseSelect(void)
//{
//	auto prevPlace = place_;
//
//	// 当たり判定取る
//	for (int i = 0; i < static_cast<int>(PLACE::MAX); i++)
//	{
//		// 全て初期化する
//		buttonState_[i] = BUTTON_STATE::DEFAULE;
//
//		if (CollisionUtility::RectangleAndMouse(pos_[i], COL_SIZE_X, COL_SIZE_Y))
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
//	if (place_ != prevPlace && place_ != PLACE::MAX)
//	{
//		// 何も選択されていない状態から選択されたらSEを流す
//		SoundManager::GetInstance().Play(SoundManager::SE::SELECT);
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

void Upgrade::ChangeState(STATE state)
{
	state_ = state;

	switch (state_)
	{
	case Upgrade::STATE::SELECT:
		SelectInit();
		break;
	case Upgrade::STATE::CONFIRM:
		ConfirmInit();
		break;
	case Upgrade::STATE::APPLY:
		ApplyInit();
		break;
	case Upgrade::STATE::NON:
		break;
	default:
		break;
	}

}

void Upgrade::SelectInit(void)
{
	// 選択決定内容をNONに初期化する
	finalizeUpgrade_ = PLAYER_UPGRADE::MAX;

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
