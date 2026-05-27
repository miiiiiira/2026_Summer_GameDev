#include "GameClear.h"

#include <DxLib.h>
#include "../../Application.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"

GameClear::GameClear(void)
{
	handle_ = -1;
	// マウスの表示する
	SetMouseDispFlag(true);
}

GameClear::~GameClear(void)
{
}

void GameClear::Init(void)
{
	handle_ = LoadGraph("Data/Image/GCI.png");
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
	if (InputManager::GetInstance()->PushAnyButton())
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
	}
}

void GameClear::Draw(void)
{
#ifdef _DEBUG

	DrawString(0, 0, "GameClear", GetColor(255, 255, 255));

#endif // _DEBUG

	/*DrawGraph(0, 0, handle_, true);*/
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0, 0.0, handle_, true);

}

void GameClear::Release(void)
{
	DeleteGraph(handle_);
}
