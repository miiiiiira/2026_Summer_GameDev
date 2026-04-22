#include <memory>

#include "MainMenu.h"

#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../../Application.h"
#include "../Confirm/Confirm.h"
#include "../GameScene/GameScene.h"

MainMenu::MainMenu(void)
{
	handle_ = -1;
	playImg_ = -1;
	optionsImg_ = -1;
	quitImg_ = -1;
	confirm_ = nullptr;
}

MainMenu::~MainMenu(void)
{
}

void MainMenu::Init(void)
{
	ChangeSelect(Menu::NONE);

	confirm_ = std::make_shared<Confirm>();
}

void MainMenu::Load(void)
{
	handle_ = LoadGraph((Application::PATH_IMAGE + "Title.png").c_str());
	playImg_ = LoadGraph((Application::PATH_IMAGE + "play.png").c_str());
	optionsImg_ = LoadGraph((Application::PATH_IMAGE + "option.png").c_str());
	quitImg_ = LoadGraph((Application::PATH_IMAGE + "quit.png").c_str());
	frameImg_ = LoadGraph((Application::PATH_IMAGE + "frame.png").c_str());
}

void MainMenu::LoadEnd(void)
{
	Init();
}

void MainMenu::Update(void)
{
	// 衝突判定
	if (Collision::HitCircleBox({ PLAY_POS_X, PLAY_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::PLAY);
	}
	else if (Collision::HitCircleBox({ OPTION_POS_X, OPTION_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::OPTION);
	}
	else if (Collision::HitCircleBox({ QUIT_POS_X, QUIT_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::QUIT);
	}
	else
	{
		ChangeSelect(Menu::NONE);
	}

	// マウスを左クリックしたら
	if (InputManager::GetInstance()->IsClickMouseLeft())
	{
		switch (currentMenu_)
		{
		case Menu::PLAY:
			UpdatePlay();
			break;

		case Menu::OPTION:
			UpdateOption();
			break;

		case Menu::QUIT:
			UpdateQuit();
			break;
		}
	}
}

void MainMenu::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	DrawGraph(TITLE_POS_X, TITLE_POS_Y, handle_, true);
	DrawGraph(currentMenuPos_.x, currentMenuPos_.y, frameImg_, true);	// フレームの画像を表示
	DrawGraph(PLAY_POS_X, PLAY_POS_Y, playImg_, true);					// PLAYの文字を表示
	DrawGraph(OPTION_POS_X, OPTION_POS_Y, optionsImg_, true);			// OPTIONの文字を表示
	DrawGraph(QUIT_POS_X, QUIT_POS_Y, quitImg_, true);					// QUITの文字を表示
}

void MainMenu::Release(void)
{
	DeleteGraph(handle_);
	DeleteGraph(playImg_);
	DeleteGraph(optionsImg_);
	DeleteGraph(quitImg_);
	DeleteGraph(frameImg_);
}

void MainMenu::ChangeSelect(Menu menu)
{
	currentMenu_ = menu;

	switch (menu)
	{
	case Menu::NONE:
		currentMenuPos_ = { 0, -100 };
		break;

	case Menu::PLAY:
		currentMenuPos_ = { PLAY_POS_X, PLAY_POS_Y };
		break;

	case Menu::OPTION:
		currentMenuPos_ = { OPTION_POS_X, OPTION_POS_Y };
		break;

	case Menu::QUIT:
		currentMenuPos_ = { QUIT_POS_X, QUIT_POS_Y };
		break;
	}
}


void MainMenu::UpdatePlay(void)
{
	// ゲームシーンへ
	SceneManager::GetInstance()->JumpScene(std::make_shared<GameScene>());
}

void MainMenu::UpdateOption(void)
{

}

void MainMenu::UpdateQuit(void)
{
	// 確認シーンへ
	currentMenuPos_ = { 0, -100 };
	confirm_->ChangeResult(Confirm::RESULT::QUIT);
	SceneManager::GetInstance()->PushScene(confirm_);
}
