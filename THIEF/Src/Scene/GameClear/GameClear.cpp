#include <DxLib.h>
#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../TitleScene/TitleScene.h"
#include "../../Common/Shader/Shader.h"
#include "../../Common/MouseCursor/MouseCursor.h"

#include "GameClear.h"

namespace
{
	// 画像のYサイズ
	constexpr int IMAGE_SIZE_Y = 3840;

	// エンドロールのY軸リミット値
	constexpr int LIMIT_Y = Application::SCREEN_SIZE_Y - IMAGE_SIZE_Y;

	// スキップ画像のサイズ
	constexpr int SKIP_IMAGE_SIZE = 120;

	// スキップ時間の最大値
	constexpr float MAX_SKIP_TIME = 120.0f;

	// 最大アルファ値
	constexpr int ALPHA_MAX = 255;

	// 最小アルファ値
	constexpr int ALPHA_MIN = 0;

	// アルファ値
	constexpr int ALPHA_STEP = 10;

	// シェーダーを変えるまでの待ち時間
	constexpr int WAIT_SHADER_TIME = 120;

	// スクロールを始めるまでの待ち時間
	constexpr int SCROLL_START_TIME = 60;

	// スクロール量
	constexpr int SCROLL_SPEED = 1;

	// スクロール後、タイトルへ行くまでの待ち時間
	constexpr int WAIT_END_TIME = 120;

	// ゲージの最大値
	constexpr float GAUGE_MAX_PERCENT = 100.0f;

	// ゲージの開始位置
	constexpr double GAUGE_START_PERCENT = 0.0;
}

GameClear::GameClear(void)
{
}

GameClear::~GameClear(void)
{
}

void GameClear::Init(void)
{
	// マウスの表示する
	MouseCursor::GetInstance()->SetMouseDraw(true);

	waitTimer_ = 0;
	scrollY_ = 0;
	skipTimer_ = 0;
	alpha_ = ALPHA_MIN;

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
		// タイトルシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), TITLE);
		return;
	}

	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::DECIDE) || InputManager::GetInstance()->IsDebugAction(INPUT_INFO::DEBUG_ACTION::DECIDE))
	{
		skipTimer_++;	// ボタンが押されるとスキップカウンターをカウントさせる

		alpha_ += ALPHA_STEP;
		if (alpha_ >= ALPHA_MAX)
		{
			alpha_ = ALPHA_MAX;
		}
	}
	else
	{
		skipTimer_ = 0.0f;	// ボタンが離されたら、カウンターを0にする

		alpha_ -= ALPHA_STEP;
		if (alpha_ <= ALPHA_MIN)
		{
			alpha_ = ALPHA_MIN;
		}
	}

	switch (state_)
	{
	case STATE::WAIT_SHADER:
		if (waitTimer_ >= WAIT_SHADER_TIME)
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
		if (waitTimer_ >= SCROLL_START_TIME)
		{
			scrollY_ -= SCROLL_SPEED;

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
		if (waitTimer_ >= WAIT_END_TIME)
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
	float rate = (static_cast<float>(skipTimer_) / MAX_SKIP_TIME) * GAUGE_MAX_PERCENT;
	DrawCircleGauge(gaugePosX, gaugePosY, rate, skipGauge_, GAUGE_START_PERCENT);

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
	SceneManager::GetInstance()->GetShader()->
		SetScanlineIntensity(Shader::MENU_SCANLINE_INTENSITY);

	// 歪み
	SceneManager::GetInstance()->GetShader()->
		SetCurvatureAmount(Shader::MENU_CURVATURE_AMOUNT);
	
	// ノイズ
	SceneManager::GetInstance()->GetShader()->
		SetNoisePower(Shader::MENU_NOISE_POWER);
	
	// 色ずれ
	SceneManager::GetInstance()->GetShader()->
		SetRgbShift(Shader::MENU_RGB_SHIFT);
}