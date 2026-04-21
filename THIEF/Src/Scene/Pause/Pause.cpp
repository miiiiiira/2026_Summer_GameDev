#include "Pause.h"

#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../../Application.h"

Pause::Pause(void)
{
	handle_ = -1;
	continueImg_ = -1;
	optionsImg_ = -1;
	mainMenuImg_ = -1;
	quitImg_ = -1;
	frameImg_ = -1;
}

Pause::~Pause(void)
{
}

void Pause::Init(void)
{
	ChangeSelect(Menu::NONE);
}

void Pause::Load(void)
{
	handle_ = LoadGraph((Application::PATH_IMAGE + "pause.png").c_str());
	continueImg_ = LoadGraph((Application::PATH_IMAGE + "continue.png").c_str());
	optionsImg_ = LoadGraph((Application::PATH_IMAGE + "option.png").c_str());
	mainMenuImg_ = LoadGraph((Application::PATH_IMAGE + "mainMenu.png").c_str());
	quitImg_ = LoadGraph((Application::PATH_IMAGE + "quit.png").c_str());
	frameImg_ = LoadGraph((Application::PATH_IMAGE + "frame.png").c_str());
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

	// 衝突判定
	if (Collision::HitCircleBox({ CONTINUE_POS_X, CONTINUE_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::CONTINUE);
	}
	else if (Collision::HitCircleBox({ OPTION_POS_X, OPTION_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::OPTION);
	}
	else if (Collision::HitCircleBox({ MAINMENU_POS_X, MAINMENU_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::MAINMENU);
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

}

void Pause::Draw(void)
{
	// 背景色を半透明で表示
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawGraph(PAUSE_POS_X, PAUSE_POS_Y, handle_, true);					// PAUSEの文字を表示
	DrawGraph(currentMenuPos_.x, currentMenuPos_.y, frameImg_, true);	// フレーム画像を表示
	DrawGraph(CONTINUE_POS_X, CONTINUE_POS_Y, continueImg_, true);		// CONTINUEの文字を表示
	DrawGraph(OPTION_POS_X, OPTION_POS_Y, optionsImg_, true);			// OPTIONの文字を表示
	DrawGraph(MAINMENU_POS_X, MAINMENU_POS_Y, mainMenuImg_, true);		// MAIN MENUの文字を表示
	DrawGraph(QUIT_POS_X, QUIT_POS_Y, quitImg_, true);					// QUITの文字を表示


}

void Pause::Release(void)
{
	DeleteGraph(handle_);
	DeleteGraph(continueImg_);
	DeleteGraph(optionsImg_);
	DeleteGraph(mainMenuImg_);
	DeleteGraph(quitImg_);
	DeleteGraph(frameImg_);

}

void Pause::ChangeSelect(Menu menu)
{
	switch (menu)
	{
	case Menu::NONE:
		currentMenuPos_ = { 0, -100 };
		currentMenu_ = Menu::NONE;
		break;

	case Menu::CONTINUE:
		currentMenuPos_ = { CONTINUE_POS_X, CONTINUE_POS_Y };
		currentMenu_ = Menu::CONTINUE;
		break;

	case Menu::OPTION:
		currentMenuPos_ = { OPTION_POS_X, OPTION_POS_Y };
		break;

	case Menu::MAINMENU:
		currentMenuPos_ = { MAINMENU_POS_X, MAINMENU_POS_Y };
		currentMenu_ = Menu::MAINMENU;
		break;

	case Menu::QUIT:
		currentMenuPos_ = { QUIT_POS_X, QUIT_POS_Y };
		currentMenu_ = Menu::QUIT;
		break;
	}
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

}

void Pause::UpdateQuit(void)
{
	Application::GetInstance()->SetEnd(true);
}
