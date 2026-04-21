#include "TitleScene.h"

#include <DxLib.h>

#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Application.h"
#include "../GameScene/GameScene.h"

TitleScene::TitleScene(void)
{
	handle_ = -1;
	buttonHandle_ = -1;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
}

void TitleScene::Load(void)
{
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::TITLE);
	handle_ = LoadGraph("Data/Image/Title.png");
	buttonHandle_ = LoadGraph("Data/Image/PushAnyButton.png");
}

void TitleScene::LoadEnd(void)
{
	Init();
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_TITLE);
}

void TitleScene::Update(void)
{
	// スペースが押されると次のシーンへ
	if(InputManager::GetInstance()->IsTrgUp(KEY_INPUT_SPACE))
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameScene>());
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0, 0.0, handle_, true);
	DrawRotaGraph(BUTTON_POS_X, BUTTON_POS_Y, 1.0, 0.0, buttonHandle_, true);
}

void TitleScene::Release(void)
{
	DeleteGraph(handle_);
	DeleteGraph(buttonHandle_);
}
