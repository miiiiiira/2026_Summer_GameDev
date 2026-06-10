#include "TitleScene.h"

#include <DxLib.h>

#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Application.h"
#include "../GameScene/GameScene.h"
#include "../MainMenu/MainMenu.h"

TitleScene::TitleScene(void)
{
	handle_ = -1;
	buttonHandle_ = -1;
	alpha_ = 0.0f;
	isIncreasing_ = false;
	// マウスの表示する
	SetMouseDispFlag(true);
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	alpha_ = 255.0f;
	isIncreasing_ = false;
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
	// ボタンが押されると次のシーンへ
	if (InputManager::GetInstance()->PushAnyButton())
	{
		// ボタン音
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_2);

		// ゲームシーンへ
		SceneManager::GetInstance()->PushScene(std::make_shared<MainMenu>());
	}

	// ボタンのアルファ値を変化させる
	if (isIncreasing_)
	{
		alpha_ += ALPHA_SPEED; // 増加速度

		// 増加が最大になったら減少に切り替える
		if(alpha_ >= ALPHA_MAX)
		{
			alpha_ = ALPHA_MAX;
			isIncreasing_ = false; 
		}
	}
	else
	{
		alpha_ -= ALPHA_SPEED; // 減少速度

		// 減少が最小になったら増加に切り替える
		if(alpha_ <= ALPHA_MIN)
		{
			alpha_ = ALPHA_MIN;
			isIncreasing_ = true;
		}
	}
}

void TitleScene::Draw(void)
{
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 - 100, 1.0, 0.0, handle_, true);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
	DrawRotaGraph(BUTTON_POS_X, BUTTON_POS_Y, 1.0, 0.0, buttonHandle_, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void TitleScene::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::TITLE);

	DeleteGraph(handle_);
	DeleteGraph(buttonHandle_);
}
