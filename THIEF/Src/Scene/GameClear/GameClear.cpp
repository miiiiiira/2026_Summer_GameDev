#include "GameClear.h"

#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"
#include "../../Common/MouseCursor/MouseCursor.h"

GameClear::GameClear(void)
{
	handle_ = -1;
	// マウスの表示する
	MouseCursor::GetInstance().SetMouseDraw(true);
	tiHandle_ = -1;
}

GameClear::~GameClear(void)
{
}

void GameClear::Init(void)
{
	handle_ = LoadGraph("Data/Image/GameClear.png"); 
	tiHandle_ = LoadGraph("Data/Image/TT.png");
}

void GameClear::Load(void)
{
	
}

void GameClear::LoadEnd(void)
{
	Init();
}

void GameClear::Update(void)
{
	// ボタンが押されると次のシーンへ
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE))
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(),TITLE);
	}
}

void GameClear::Draw(void)
{
#ifdef _DEBUG

	DrawString(0, 0, "GameClear", GetColor(255, 255, 255));

#endif // _DEBUG

	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0, 0.0, handle_, true);

	DrawRotaGraph(TITLE_POS_X, TITLE_POS_Y, 1.0, 0.0, tiHandle_, true);
}

void GameClear::Release(void)
{
	DeleteGraph(handle_);
	DeleteGraph(tiHandle_);
}
