#include <memory>
#include <vector>

#include "MainMenu.h"

#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../../Common/Manager/System/SystemManager.h"
#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../../Application.h"
#include "../Confirm/Confirm.h"
#include "../GameScene/GameScene.h"
#include "../DebugScene.h"
#include "../Tutorial/TutorialScene.h"
#include "../Option/OptionScene.h"
#include "../TitleScene/TitleScene.h"
#include "../LightSelectScene/LightSelectScene.h"

MainMenu::MainMenu(void)
{
	handle_ = -1;
	confirm_ = nullptr;
}

MainMenu::~MainMenu(void)
{
}

void MainMenu::Init(void)
{
	ChangeSelect(MENU::NONE);

	confirm_ = std::make_shared<Confirm>();

	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_MAINMENU);
}

void MainMenu::Load(void)
{
	// メインメニューサウンド読みこみ
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::MAIN_MENU);

	handle_ = LoadGraph((Application::PATH_IMAGE + "Title.png").c_str());

	menuButtons_.clear();

	// PLAY画像
	menuButtons_.push_back({ MENU::PLAY, LoadGraph((Application::PATH_IMAGE + "play.png").c_str()),
								PLAY_POS_X, PLAY_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// OPTION画像
	menuButtons_.push_back({ MENU::OPTION, LoadGraph((Application::PATH_IMAGE + "option.png").c_str()),
							OPTION_POS_X, OPTION_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// QUIT画像
	menuButtons_.push_back({ MENU::QUIT, LoadGraph((Application::PATH_IMAGE + "quit.png").c_str()),
						QUIT_POS_X, QUIT_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });

}

void MainMenu::LoadEnd(void)
{
	Init();
}

void MainMenu::Update(void)
{
#ifdef _DEBUG
	// Lキーを押したらデバッグシーン
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::DEBUG))
	{
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<DebugScene>(),DEBUG, true);
		return;
	}

	// Lキーを押したらデバッグシーン
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TutorialScene>(),TUTORIAL, true);
		return;
	}

	// Lキーを押したらデバッグシーン
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::COLOR_CHANGE))
	{
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<LightSelectScene>(),LIGHT_SELECT, true);
		return;
	}
#endif //_DEBUG

	// メニューからタイトルに戻すボタン
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::CANCEL) || InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::CANCEL))
	{
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(),TITLE, true);
		return;
	}

	// 選択処理
	SelectUpgrade();

	// マウスを左クリックしなかったら、処理を行わない
	if (!InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE) && !InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::DECIDE)) return;

	// メニューが選択されていない場合、処理を行わない
	if (currentMenu_ == MENU::NONE) return;

	switch (currentMenu_)
	{
	case MENU::PLAY:
		UpdatePlay();
		break;

	case MENU::OPTION:
		UpdateOption();
		break;

	case MENU::QUIT:
		UpdateQuit();
		break;
	}

	// ボタン押下のSEを流す
	AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);

}

void MainMenu::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	DrawGraph(TITLE_POS_X, TITLE_POS_Y, handle_, true);

	for (const auto& button : menuButtons_)
	{
		if (button.type == currentMenu_)
		{
			FrameRenderer::Draw(button.x, button.y);
		}
		DrawGraph(button.x, button.y, button.graphHandle, true);		// メニューボタンの画像
	}
}

void MainMenu::Release(void)
{
	for (const auto& button : menuButtons_)
	{
		DeleteGraph(button.graphHandle);
	}
	menuButtons_.clear();

	DeleteGraph(handle_);
	handle_ = -1;

	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::MAIN_MENU);
}

void MainMenu::ChangeSelect(MENU menu)
{
	currentMenu_ = menu;
}


void MainMenu::UpdatePlay(void)
{
	confirm_->ChangeType(Confirm::TYPE::TUTORIAL);
	SceneManager::GetInstance()->PushScene(confirm_);
}

void MainMenu::UpdateOption(void)
{
	// オプションへ
	SceneManager::GetInstance()->PushScene(std::make_shared<OptionScene>());
}

void MainMenu::UpdateQuit(void)
{
	confirm_->ChangeType(Confirm::TYPE::QUIT);
	SceneManager::GetInstance()->PushScene(confirm_);
}

void MainMenu::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	MENU prevMenu = currentMenu_;

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
	if (currentMenu_ != MENU::NONE
		&& currentMenu_ != prevMenu)
	{
		// ボタンに乗ったサウンドを出す
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void MainMenu::MouseSelect(void)
{
	MENU nextSelect = MENU::NONE;

	// 衝突判定
	for (const auto& button : menuButtons_)
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

void MainMenu::PadSelect(void)
{
	auto prevMenu = currentMenu_;

	switch (currentMenu_)
	{
	case MainMenu::MENU::NONE:

		ChangeSelect(MENU::PLAY);

		break;
	case MainMenu::MENU::PLAY:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			ChangeSelect(MENU::OPTION);
		}

		break;
	case MainMenu::MENU::OPTION:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			ChangeSelect(MENU::PLAY);
		}

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			ChangeSelect(MENU::QUIT);
		}

		break;
	case MainMenu::MENU::QUIT:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			ChangeSelect(MENU::OPTION);
		}

		break;
	default:
		break;
	}
}
