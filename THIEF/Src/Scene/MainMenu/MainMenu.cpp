#include <memory>
#include <vector>

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
	frameImg_ = LoadGraph((Application::PATH_IMAGE + "frame.png").c_str());

	menuButtons_.clear();

	// PLAY画像
	menuButtons_.push_back({ Menu::PLAY, LoadGraph((Application::PATH_IMAGE + "play.png").c_str()),
								PLAY_POS_X, PLAY_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// OPTION画像
	menuButtons_.push_back({ Menu::OPTION, LoadGraph((Application::PATH_IMAGE + "option.png").c_str()),
							OPTION_POS_X, OPTION_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// QUIT画像
	menuButtons_.push_back({ Menu::QUIT, LoadGraph((Application::PATH_IMAGE + "quit.png").c_str()),
						QUIT_POS_X, QUIT_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
}

void MainMenu::LoadEnd(void)
{
	Init();
}

void MainMenu::Update(void)
{
	Menu nextSelect = Menu::NONE;

	// 衝突判定
	for (const auto& button : menuButtons_)
	{
		if (Collision::HitCircleBox({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			nextSelect = button.type;
			break;
		}
	}

	ChangeSelect(nextSelect);

	// マウスを左クリックしなかったら、処理を行わない
	if (!InputManager::GetInstance()->IsClickMouseLeft()) return;

	// メニューが選択されていない場合、処理を行わない
	if (currentMenu_ == Menu::NONE) return;

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

void MainMenu::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	DrawGraph(TITLE_POS_X, TITLE_POS_Y, handle_, true);

	for (const auto& button : menuButtons_)
	{
		if (button.type == currentMenu_)
		{
			DrawGraph(button.x, button.y, frameImg_, true);				 // フレーム画像
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

	DeleteGraph(frameImg_);
	frameImg_ = -1;
}

void MainMenu::ChangeSelect(Menu menu)
{
	currentMenu_ = menu;
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
	confirm_->ChangeType(Confirm::TYPE::QUIT);
	SceneManager::GetInstance()->PushScene(confirm_);
}
