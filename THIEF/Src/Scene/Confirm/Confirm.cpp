#include <vector>

#include "Confirm.h"

#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../MainMenu/MainMenu.h"
#include "../../Object/Component/PlayerController/Upgrade/UpgradeManager.h"
#include "../../Object/Component/PlayerController/Upgrade/Upgrade.h"
#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../LightSelectScene/LightSelectScene.h"
#include "../Tutorial/TutorialScene.h"

Confirm::Confirm(void)
{
	confirmImg_ = -1;
}

Confirm::~Confirm(void)
{
}

void Confirm::Init(void)
{
	ChangeSelect(SELECT::NONE);
}

void Confirm::Load(void)
{

	switch (confirmType_)
	{
	case TYPE::TUTORIAL:
		confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmTutorial.png").c_str());	// TUTORIALの時の確認画面
		break;
	case TYPE::QUIT:
		confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmQuit.png").c_str());		// QUITの時の確認画面
		break;
	case TYPE::MAIN_MENU:
		confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmMainMenu.png").c_str());	// MAIN MENUの時の確認画面
		break;
	case TYPE::BUY_UPGRADE:
	{
		PLAYER_UPGRADE_TYPE type = UpgradeManager::GetInstance()->GetUpgrade()->GetFinalizeUpgrade().type;

		switch (type)
		{
		case PLAYER_UPGRADE_TYPE::HP_UP:
			confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmBuyHpUp.png").c_str());
			break;
		case PLAYER_UPGRADE_TYPE::STAMINA_UP:
			confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmBuyStaminaUp.png").c_str());
			break;
		case PLAYER_UPGRADE_TYPE::DASH_SPEED_UP:
			confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmBuyDashSpeedUp.png").c_str());
			break;
		case PLAYER_UPGRADE_TYPE::RANGE_UP:
			confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmBuyRangeUp.png").c_str());
			break;
		case PLAYER_UPGRADE_TYPE::JUMP_NUM_UP:
			confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmBuyJumpNumUp.png").c_str());
			break;
		case PLAYER_UPGRADE_TYPE::HEAL_HP_25:
			confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmBuyHealHp25.png").c_str());
			break;
		case PLAYER_UPGRADE_TYPE::HEAL_HP_50:
			confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmBuyHealHp50.png").c_str());
			break;
		default:
			break;
		}
	}
	break;
	case TYPE::RESET_KEY:
		confirmImg_ = LoadGraph((Application::PATH_IMAGE + "Confirm/ConfirmResetAllKey.png").c_str());	// RESETKEYの時の確認画面
		break;
	}


	// 配列をクリアにしてから、画像を追加
	selectButtons_.clear();
	// YES画像
	selectButtons_.push_back({ SELECT::YES, LoadGraph((Application::PATH_IMAGE + "Confirm/Yes.png").c_str()),
								YES_POS_X, YES_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// NO画像
	selectButtons_.push_back({ SELECT::NO,  LoadGraph((Application::PATH_IMAGE + "Confirm/No.png").c_str()),
							NO_POS_X, NO_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
}

void Confirm::LoadEnd(void)
{
	Init();
}

void Confirm::Update(void)
{
	// 選択処理
	SelectUpgrade();

	// マウスを左クリックされなかったら、処理しない
	if (!InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE) && !InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::DECIDE)) return;
	// どの選択肢も選ばれていない場合は処理しない
	if (currentSelect_ == SELECT::NONE) return;

	switch (currentSelect_)
	{
	case Confirm::SELECT::YES:
		UpdateYes();
		break;
	case Confirm::SELECT::NO:
		UpdateNo();
		break;
	}

	// ボタン押下のSEを流す
	AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);
}

void Confirm::Draw(void)
{
	DrawGraph(CONFIRM_POS_X, CONFIRM_POS_Y, confirmImg_, true);				// 確認の文字を表示

	for (const auto& button : selectButtons_)
	{
		if (button.type == currentSelect_)
		{
			FrameRenderer::Draw(button.x,
				button.y,
				button.sizeX,
				button.sizeY,
				FRAME_OFFSET);
		}
		DrawGraph(button.x, button.y, button.graphHandle, true);
	}
}

void Confirm::Release(void)
{
	for (const auto& button : selectButtons_)
	{
		DeleteGraph(button.graphHandle);
	}
	selectButtons_.clear();

	DeleteGraph(confirmImg_);
	confirmImg_ = -1;

}

void Confirm::ChangeType(TYPE type)
{
	confirmType_ = type;
}

void Confirm::ChangeSelect(SELECT select)
{
	currentSelect_ = select;
}

void Confirm::UpdateYes(void)
{
	switch (confirmType_)
	{
	case TYPE::TUTORIAL:
		// チュートリアルへ遷移
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TutorialScene>(), TUTORIAL, true);
		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		break;
	case TYPE::QUIT:
		Application::GetInstance()->SetEnd(true);
		break;
	case TYPE::MAIN_MENU:
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<MainMenu>(),MAINMANU, true);
		// ゲームを終了したため、ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		break;
	case TYPE::BUY_UPGRADE:
		UpgradeManager::GetInstance()->GetUpgrade()->ChangeState(Upgrade::UPGRADE_STATE::APPLY);
		// 確認シーンを閉じる
		SceneManager::GetInstance()->PopScene();
		break;
	case TYPE::RESET_KEY:
		InputManager::GetInstance()->DefaultKey();
		// 確認シーンを閉じる
		SceneManager::GetInstance()->PopScene();
		break;
	default:
		break;
	}
}

void Confirm::UpdateNo(void)
{
	// チュートリアルを断っていたら
	if (confirmType_ == TYPE::TUTORIAL)
	{
		// ライトセレクトシーンへ遷移
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<LightSelectScene>(), LIGHT_SELECT, true);
		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		return;
	}

	// 確認シーンを閉じる
	SceneManager::GetInstance()->PopScene();
}

void Confirm::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	SELECT prevSelect = currentSelect_;

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
	if (currentSelect_ != SELECT::NONE
		&& currentSelect_ != prevSelect)
	{
		// ボタンに乗ったサウンドを出す
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void Confirm::MouseSelect(void)
{
	// 衝突判定
	SELECT nextSelect = SELECT::NONE;

	for (const auto& button : selectButtons_)
	{
		if (Collision::HitMouseImg2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			nextSelect = button.type;
			break;
		}
	}

	ChangeSelect(nextSelect);
}

void Confirm::PadSelect(void)
{
	switch (currentSelect_)
	{
	case Confirm::SELECT::NONE:

		// NOボタンへ
		ChangeSelect(SELECT::NO);

		break;
	case Confirm::SELECT::YES:

		// 右押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_RIGHT))
		{
			// NOボタンへ
			ChangeSelect(SELECT::NO);
		}

		break;
	case Confirm::SELECT::NO:

		// 左押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_LEFT))
		{
			// YESボタンへ
			ChangeSelect(SELECT::YES);
		}

		break;
	default:
		break;
	}
}