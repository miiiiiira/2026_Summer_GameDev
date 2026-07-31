#include "GameOver.h"

#include <DxLib.h>

#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../../Common/Manager/System/SystemManager.h"
#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../SceneManager.h"
#include "../../Common/Shader/Shader.h"
#include "../../Common/Collision/Collision.h"
#include "../GameScene/GameScene.h"
#include "../TitleScene/TitleScene.h"
#include "../../Common/MouseCursor/MouseCursor.h"

GameOver::GameOver(void)
{
	handle_ = -1;
	crackHandle_ = -1;
	step_ = 0;
	// マウスの表示する
	MouseCursor::GetInstance().SetMouseDraw(true);
	currentType_ = NONE;
}

GameOver::~GameOver(void)
{
}

void GameOver::Init(void)
{
	ChangeState(STATE::NONE);

	// ノイズ
	SceneManager::GetInstance()->GetShader()->SetNoisePower(0.5f);
	// 色ずれ
	SceneManager::GetInstance()->GetShader()->SetRgbShift(0.004f);
	// 歪み
	SceneManager::GetInstance()->GetShader()->SetCurvatureAmount(0.1f);
}

void GameOver::Load(void)
{
	handle_ = LoadGraph("Data/Image/GameOver/GameOver01.png");
	crackHandle_ = LoadGraph("Data/Image/GameOver/GameOver02.png");

	// RETRY画像
	buttons_.push_back({ TYPE::RETRY, LoadGraph("Data/Image/GameOver/Retry.png"),
								RETRY_POS_X, RETRY_POS_Y, RETRY_SIZE_X, RETRY_SIZE_Y });
	// RETURN_TITLE画像
	buttons_.push_back({ TYPE::RETURN_TITLE, LoadGraph("Data/Image/GameOver/ReturnTitle.png"),
							RETURN_TITLE_POS_X, RETURN_TITLE_POS_Y, RETURN_TITLE_SIZE_X, RETURN_TITLE_SIZE_Y });

	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME_OVER);
}

void GameOver::LoadEnd(void)
{
	Init();
}

void GameOver::Update(void)
{
	switch (state_)
	{
	case GameOver::STATE::NONE:
		UpdateNone();
		break;
	case GameOver::STATE::SHAKE:
		UpdateShake();
		break;
	case GameOver::STATE::CRACK:
		UpdateCrack();
		break;
	case GameOver::STATE::SELECT:
		UpdateSelect();
		break;
	default:
		break;
	}
}

void GameOver::Draw(void)
{
	int shake = 0;

	switch (state_)
	{
	case GameOver::STATE::NONE:
	case GameOver::STATE::SHAKE:

		// ヒットストップカウンタが0じゃない場合に揺らし量を計算
		GetShakeOffset(shake);
		// 画像の描画
		DrawGraph(shake, shake, handle_, true);

		break;
	case GameOver::STATE::CRACK:
	case GameOver::STATE::SELECT:
		// 画像の描画
		DrawGraph(0, 0, crackHandle_, true);

		break;
	default:
		break;
	}

	for (const auto& button : buttons_)
	{
		int x = button.x;
		int y = button.y;

		// 揺らし量分ずらす
		x += shake;
		y += shake;

		if (button.type == currentType_)
		{
			FrameRenderer::Draw(x, y, button.sizeX, button.sizeY, FRAME_OFFSET);
		}
		DrawGraph(x, y, button.graphHandle, true);
	}
}

void GameOver::Release(void)
{
	DeleteGraph(handle_);
	DeleteGraph(crackHandle_);

	for (const auto& button : buttons_)
	{
		DeleteGraph(button.graphHandle);
	}
	buttons_.clear();

	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME_OVER);
}

void GameOver::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	TYPE prevType = currentType_;

	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::KEY_MOUSE)
	{
		// マウス選択
		MouseSelect();
	}
	else
	{
		// パッド選択
		PadSelect();
	}

	// 中身がNONじゃないかつ、選択物が変わっていたら
	if (currentType_ != TYPE::NONE
		&& currentType_ != prevType)
	{
		// ボタンに乗ったサウンドを出す
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void GameOver::MouseSelect(void)
{
	TYPE nextType = TYPE::NONE;

	// 衝突判定
	for (const auto& button : buttons_)
	{
		if (Collision::HitMouseImg2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			nextType = button.type;
			break;
		}
	}

	currentType_ = nextType;
}

void GameOver::PadSelect(void)
{
	switch (currentType_)
	{
	case GameOver::RETRY:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			currentType_ = RETURN_TITLE;
		}

		break;
	case GameOver::RETURN_TITLE:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			currentType_ = RETRY;
		}

		break;
	case GameOver::NONE:

		currentType_ = RETRY;

		break;
	default:
		break;
	}
}

void GameOver::GetShakeOffset(int& offset)
{
	if (hitStopCounter_ > 0) {
		// 振動先をカウンターから計算する----------
		// 0 or 1
		offset = (hitStopCounter_ / 5) % 2;
		// 0 or 2　中心を作る
		offset *= 2;
		// -1 or 1　0を中心にする
		offset -= 1;
		// -3 or 3　振れ幅を付ける
		offset *= 5;
		// ----------------------------------------
	}
}

void GameOver::ChangeState(STATE state)
{
	state_ = state;

	switch (state_)
	{
	case GameOver::STATE::NONE:
		ChangeNone();
		break;
	case GameOver::STATE::SHAKE:
		ChangeShake();
		break;
	case GameOver::STATE::CRACK:
		ChangeCrack();
		break;
	case GameOver::STATE::SELECT:
		ChangeSelect();
		break;
	default:
		break;
	}
}

void GameOver::ChangeNone(void)
{
	step_ = 1.5f;
}

void GameOver::ChangeShake(void)
{
	hitStopCounter_ = SHAKE_TIME;
}

void GameOver::ChangeCrack(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_CRACK);
	step_ = 1.0f;
}

void GameOver::ChangeSelect(void)
{
	// BGMを再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAMEOVER);
}

void GameOver::UpdateNone(void)
{
	if (step_ < 0.0f)
	{
		ChangeState(STATE::SHAKE);
		return;
	}

	step_ -= SceneManager::GetInstance()->GetDeltaTime();
}

void GameOver::UpdateShake(void)
{
	// ヒットストップでの中断判定
	if (hitStopCounter_ > 0) {
		hitStopCounter_--;
	}
	else
	{
		ChangeState(STATE::CRACK);
	}
}

void GameOver::UpdateCrack(void)
{
	if (step_ < 0.0f)
	{
		ChangeState(STATE::SELECT);
		return;
	}

	step_ -= SceneManager::GetInstance()->GetDeltaTime();
}

void GameOver::UpdateSelect(void)
{
	// 選択処理
	SelectUpgrade();

	// マウスを左クリックしなかったら、処理を行わない
	if (!InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE) && !InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::DECIDE)) return;

	// 種類が選択されていない場合、処理を行わない
	if (currentType_ == TYPE::NONE) return;

	// ボタン押下のSEを流す
	AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);

	switch (currentType_)
	{
	case GameOver::RETRY:

		// ノイズ
		SceneManager::GetInstance()->GetShader()->SetNoisePower(0.1f);
		// 色ずれ
		SceneManager::GetInstance()->GetShader()->SetRgbShift(0.002f);
		// 歪み
		SceneManager::GetInstance()->GetShader()->SetCurvatureAmount(0.01f);

		// ゲームシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameScene>(), GAME);
		return;

		break;
	case GameOver::RETURN_TITLE:

		// タイトルシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), TITLE);
		return;

		break;
	default:
		break;
	}
}
