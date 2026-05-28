#include "GameOver.h"

#include <DxLib.h>
#include "../../Application.h"
#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../GameScene/GameScene.h"
#include "../TitleScene/TitleScene.h"

GameOver::GameOver(void)
{
	handle_ = -1;
	reId_ = -1;
	tiId_ = -1;
}

GameOver::~GameOver(void)
{
}

void GameOver::Init(void)
{
	handle_ = LoadGraph("Data/Image/GV.png");
	reId_ = LoadGraph("Data/Image/Try.png");
	tiId_ = LoadGraph("Data/Image/TT.png");
}

void GameOver::Load(void)
{
}

void GameOver::LoadEnd(void)
{
	Init();
}

void GameOver::Update(void)
{
	// スペースが押されると次のシーンへ
	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_SPACE))
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
	}

	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_C))
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameScene>());
	}
}

void GameOver::Draw(void)
{
#ifdef _DEBUG

	DrawString(0, 0, "GameOver", GetColor(255, 255, 255));

#endif // _DEBUG

	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0, 0.0, handle_, true);

	DrawRotaGraph(RE_POS_X, RE_POS_Y, 1.0, 0.0, reId_, true);

	DrawRotaGraph(TITLE_POS_X, TITLE_POS_Y, 1.0, 0.0, tiId_, true);
}

void GameOver::Release(void)
{
	DeleteGraph(handle_);
	DeleteGraph(reId_);
	DeleteGraph(tiId_);
}
