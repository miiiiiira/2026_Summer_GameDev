#include <memory>
#include <vector>

#include "Pause.h"

#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../../Application.h"
#include "../Confirm/Confirm.h"

Pause::Pause(void)
{
	handle_ = -1;
	frameImg_ = -1;
	confirm_ = nullptr;
}

Pause::~Pause(void)
{
}

void Pause::Init(void)
{
	ChangeSelect(Menu::NONE);

	confirm_ = std::make_shared<Confirm>();
}

void Pause::Load(void)
{
	handle_ = LoadGraph((Application::PATH_IMAGE + "pause.png").c_str());		// PAUSEの文字画像
	frameImg_ = LoadGraph((Application::PATH_IMAGE + "frame.png").c_str());		// フレーム画像

	menuButtons_.clear();

	// CONTINUE画像
	menuButtons_.push_back({ Menu::CONTINUE, LoadGraph((Application::PATH_IMAGE + "continue.png").c_str()),
								CONTINUE_POS_X, CONTINUE_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// OPTION画像
	menuButtons_.push_back({ Menu::OPTION, LoadGraph((Application::PATH_IMAGE + "option.png").c_str()),
								OPTION_POS_X, OPTION_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// MAIN MENU画像
	menuButtons_.push_back({ Menu::MAINMENU,  LoadGraph((Application::PATH_IMAGE + "mainMenu.png").c_str()),
								MAINMENU_POS_X, MAINMENU_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// QUIT画像
	menuButtons_.push_back({ Menu::QUIT, LoadGraph((Application::PATH_IMAGE + "quit.png").c_str()),
								QUIT_POS_X, QUIT_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });

}

void Pause::LoadEnd(void)
{
	Init();
}

void Pause::Update(void)
{

	// Escape押されると次のシーンへ
	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_ESCAPE))
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->PopScene();
	}

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

	// 最後に一回だけ状態を更新する
	ChangeSelect(nextSelect);

	// マウスを左クリックされていなかったら、ここで終了
	if (!InputManager::GetInstance()->IsClickMouseLeft()) return;

	// 選択されているメニューがない場合も、ここで終了
	if (currentMenu_ == Menu::NONE) return;

	// クリックされていて、かつメニューが選ばれている場合
	switch (currentMenu_)
	{
	case Menu::CONTINUE:
		UpdateContinue();
		break;

	case Menu::OPTION:
		UpdateOption();
		break;

	case Menu::MAINMENU:
		UpdateMainMenu();
		break;

	case Menu::QUIT:
		UpdateQuit();
		break;
	}
}

void Pause::Draw(void)
{
	// 背景色を半透明で表示
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawGraph(PAUSE_POS_X, PAUSE_POS_Y, handle_, true);					// PAUSEの文字を表示


	for (const auto& button : menuButtons_)
	{
		if (button.type == currentMenu_)
		{
			DrawGraph(button.x, button.y, frameImg_, true);				// フレームを表示
		}

		DrawGraph(button.x, button.y, button.graphHandle, true);		// メニューの文字を表示
	}
}

void Pause::Release(void)
{
	for (const auto& button : menuButtons_)
	{
		DeleteGraph(button.graphHandle);
	}
	menuButtons_.clear();

	DeleteGraph(frameImg_);
	frameImg_ = -1;

	DeleteGraph(handle_);
	handle_ = -1;
}

void Pause::ChangeSelect(Menu menu)
{
	currentMenu_ = menu;
}

void Pause::UpdateContinue(void)
{
	// マウスカーソルを画面中央に戻す
	SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);
	// ゲームシーンへ
	SceneManager::GetInstance()->PopScene();
}

void Pause::UpdateOption(void)
{

}

void Pause::UpdateMainMenu(void)
{
	// 確認シーンへ
	confirm_->ChangeResult(Confirm::RESULT::MAIN_MENU);
	SceneManager::GetInstance()->PushScene(confirm_);
}

void Pause::UpdateQuit(void)
{
	// 確認シーンへ
	confirm_->ChangeResult(Confirm::RESULT::QUIT);
	SceneManager::GetInstance()->PushScene(confirm_);
}
