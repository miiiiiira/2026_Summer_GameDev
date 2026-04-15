#include "GameOver.h"

#include <DxLib.h>

#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"

GameOver::GameOver(void)
{
	handle_ = -1;
}

GameOver::~GameOver(void)
{
}

void GameOver::Init(void)
{
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
}

void GameOver::Draw(void)
{
#ifdef _DEBUG

	DrawString(0, 0, "GameOver", GetColor(255, 255, 255));

#endif // _DEBUG

	DrawGraph(0, 0, handle_, true);
}

void GameOver::Release(void)
{
	DeleteGraph(handle_);
}
