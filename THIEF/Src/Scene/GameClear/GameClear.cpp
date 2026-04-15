#include "GameClear.h"

#include <DxLib.h>

#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"

GameClear::GameClear(void)
{
	handle_ = -1;
}

GameClear::~GameClear(void)
{
}

void GameClear::Init(void)
{
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
	// スペースが押されると次のシーンへ
	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_SPACE))
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

	DrawGraph(0, 0, handle_, true);
}

void GameClear::Release(void)
{
	DeleteGraph(handle_);
}
