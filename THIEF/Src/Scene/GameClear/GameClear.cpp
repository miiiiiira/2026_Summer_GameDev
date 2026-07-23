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

	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME_CLEAR);
}

void GameClear::LoadEnd(void)
{
	Init();
}

void GameClear::Update(void)
{
	if (InputManager::GetInstance()->IsAction(INPUT_INFO::ACTION::DECIDE) || InputManager::GetInstance()->IsDebugAction(INPUT_INFO::DEBUG_ACTION::DECIDE))
	{
		skipTimer_++;	// ボタンが押されるとスキップカウンターをカウントさせる
	}
	else
	{
		skipTimer_ = 0;	// ボタンが離されたら、カウンターを0にする
	}

	// スキップ処理を1秒以上押されたら、タイトルに遷移
	if (skipTimer_ >= 60)
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), TITLE);
		return;
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
}

void GameClear::Release(void)
{
	DeleteGraph(handle_);

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