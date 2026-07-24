#include "GameClear.h"

#include <DxLib.h>

#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"
#include "../../Common/Shader/Shader.h"
#include "../../Common/MouseCursor/MouseCursor.h"

GameClear::GameClear(void)
{
	handle_ = -1;
	skipHandle_ = -1;
	skipGauge_ = -1;
	// マウスの表示する
	MouseCursor::GetInstance().SetMouseDraw(true);
	waitTimer_ = 0;
	scrollY_ = 0;
	skipTimer_ = 0;
}

GameClear::~GameClear(void)
{
}

void GameClear::Init(void)
{
	waitTimer_ = 0;
	scrollY_ = 0;
	skipTimer_ = 0;

	state_ = STATE::WAIT_SHADER;
}

void GameClear::Load(void)
{
	handle_ = LoadGraph("Data/Image/GameClear/EndRoll.png");

	skipHandle_ = LoadGraph("Data/Image/GameClear/skip.png");
	skipGauge_ = LoadGraph("Data/Image/GameClear/skipGauge.png");

	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME_CLEAR);
}

void GameClear::LoadEnd(void)
{
	Init();
}

void GameClear::Update(void)
{
	// スキップ処理を1秒以上押されたら、タイトルに遷移
	if (skipTimer_ >= MAX_SKIP_TIME)
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), TITLE);
		return;
	}

	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::DECIDE) || InputManager::GetInstance()->IsDebugAction(INPUT_INFO::DEBUG_ACTION::DECIDE))
	{
		skipTimer_++;	// ボタンが押されるとスキップカウンターをカウントさせる

		alpha_ += 10.0f;

		if (alpha_ >= 255.0f)
		{
			alpha_ = 255.0f;
		}
	}
	else
	{
		skipTimer_ = 0.0f;	// ボタンが離されたら、カウンターを0にする

		alpha_ -= 10.0f;

		if (alpha_ <= 0.0f)
		{
			alpha_ = 0.0f;
		}
	}

	switch (state_)
	{
	case STATE::WAIT_SHADER:

		if (waitTimer_ >= 120)
		{
			ShaderInit();
			waitTimer_ = 0; // タイマーをリセット

			// BGMを再生
			AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAMECLEAR);

			state_ = STATE::SCROLL;
		}
		else
		{
			waitTimer_++;
		}
		break;

	case STATE::SCROLL:

		if (waitTimer_ >= 60)
		{
			scrollY_ -= 1;

			if (scrollY_ <= LIMIT_Y)
			{
				scrollY_ = LIMIT_Y;
				waitTimer_ = 0; // タイマーをリセット

				state_ = STATE::WAIT_END;
			}
		}
		else
		{
			waitTimer_++;
		}
		break;

	case STATE::WAIT_END:

		if (waitTimer_ >= 120)
		{
			// タイトルシーンへ
			SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), TITLE);
		}
		else
		{
			waitTimer_++;
		}
		break;
	}
}

void GameClear::Draw(void)
{
	DrawGraph(0, scrollY_, handle_, true);

	// アルファ値を設定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);

	int gaugePosX = Application::SCREEN_SIZE_X - SKIP_IMAGE_SIZE / 2;
	int gaugePosY = Application::SCREEN_SIZE_Y - SKIP_IMAGE_SIZE / 2;
	float rate = (static_cast<float>(skipTimer_) / MAX_SKIP_TIME) * 100.0;
	DrawCircleGauge(gaugePosX, gaugePosY, rate, skipGauge_, 0.0);

	int posX = Application::SCREEN_SIZE_X - SKIP_IMAGE_SIZE;
	int posY = Application::SCREEN_SIZE_Y - SKIP_IMAGE_SIZE;
	DrawGraph(posX, posY, skipHandle_, true);

	// アルファ値を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void GameClear::Release(void)
{
	DeleteGraph(handle_);
	DeleteGraph(skipHandle_);
	DeleteGraph(skipGauge_);

	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME_CLEAR);
}

void GameClear::ShaderInit(void)
{
	// 走査線
	SceneManager::GetInstance()->GetShader()->SetScanlineIntensity(0.5f);
	// 歪み
	SceneManager::GetInstance()->GetShader()->SetCurvatureAmount(0.4f);
	// ノイズ
	SceneManager::GetInstance()->GetShader()->SetNoisePower(0.4f);
	// 色ずれ
	SceneManager::GetInstance()->GetShader()->SetRgbShift(0.004f);
}