#include "Pause.h"

#include <DxLib.h>

#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"
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
	GetMousePoint(&mousePosX_, &mousePosY_);
	mousePos_ = { static_cast<float>(mousePosX_), static_cast<float>(mousePosY_) };
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

	GetMousePoint(&mousePosX_, &mousePosY_);
	mousePos_ = { static_cast<float>(mousePosX_), static_cast<float>(mousePosY_) };

	// Escape押されると次のシーンへ
	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_ESCAPE))
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->PopScene();
	}

	if(Collision::HitCircleBox(mousePos_, { CONTINUE_POS_X, CONTINUE_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::CONTINUE);
	}
	else if (Collision::HitCircleBox(mousePos_, { OPTION_POS_X, OPTION_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::OPTION);
	}
	else if (Collision::HitCircleBox(mousePos_, { MAINMENU_POS_X, MAINMENU_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::MAINMENU);
	}
	else if (Collision::HitCircleBox(mousePos_, { QUIT_POS_X, QUIT_POS_Y }, IMAGE_SIZE_X, IMAGE_SIZE_Y))
	{
		ChangeSelect(Menu::QUIT);
	}
	else
	{	
		ChangeSelect(Menu::NONE);

	}
}

void Pause::Draw(void)
{

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	//DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x696969, true);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawGraph(PAUSE_POS_X, PAUSE_POS_Y, handle_, true);
	DrawGraph(currentMenuPos_.x, currentMenuPos_.y, frameImg_, true);
	DrawGraph(CONTINUE_POS_X, CONTINUE_POS_Y, continueImg_, true);
	DrawGraph(OPTION_POS_X, OPTION_POS_Y, optionsImg_, true);
	DrawGraph(MAINMENU_POS_X, MAINMENU_POS_Y, mainMenuImg_, true);
	DrawGraph(QUIT_POS_X, QUIT_POS_Y, quitImg_, true);


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
		break;

	case Menu::CONTINUE:
		currentMenuPos_ = { CONTINUE_POS_X, CONTINUE_POS_Y };
		break;

	case Menu::OPTION:
		currentMenuPos_ = { OPTION_POS_X, OPTION_POS_Y };
		break;

	case Menu::MAINMENU:
		currentMenuPos_ = { MAINMENU_POS_X, MAINMENU_POS_Y };
		break;

	case Menu::QUIT:
		currentMenuPos_ = { QUIT_POS_X, QUIT_POS_Y };
		break;
	}
}
