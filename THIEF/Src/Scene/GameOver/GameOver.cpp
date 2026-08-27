#include <DxLib.h>

#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../SceneManager.h"
#include "../../Common/Shader/Shader.h"
#include "../../Common/Collision/Collision.h"
#include "../GameScene/GameScene.h"
#include "../TitleScene/TitleScene.h"
#include "../../Common/MouseCursor/MouseCursor.h"

#include "GameOver.h"

GameOver::GameOver(void)
{
	// マウスの表示する
	MouseCursor::GetInstance()->SetMouseDraw(true);


}

GameOver::~GameOver(void)
{
}

void GameOver::Init(void)
{
	// 現在の選択タイプを初期化
	currentType_ = NONE;

	// ステートを初期化
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
	// 画像を読み込み
	// ゲームオーバー画像
	handle_ = LoadGraph("Data/Image/GameOver/GameOver01.png");
	// ひび割れ画像
	crackHandle_ = LoadGraph("Data/Image/GameOver/GameOver02.png");
	// RETRY画像
	buttons_.push_back({ TYPE::RETRY, LoadGraph("Data/Image/GameOver/Retry.png"),
								RETRY_POS_X, RETRY_POS_Y, RETRY_SIZE_X, RETRY_SIZE_Y });
	// RETURN_TITLE画像
	buttons_.push_back({ TYPE::RETURN_TITLE, LoadGraph("Data/Image/GameOver/ReturnTitle.png"),
							RETURN_TITLE_POS_X, RETURN_TITLE_POS_Y, RETURN_TITLE_SIZE_X, RETURN_TITLE_SIZE_Y });

	// サウンドを読み込み
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME_OVER);
}

void GameOver::LoadEnd(void)
{
	// 初期化
	Init();
}

void GameOver::Update(void)
{
	// ステートによって変更
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

		// ひび割れ画像の描画
		DrawGraph(0, 0, crackHandle_, true);

		// アルファ値を設定
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);

		// ボタンを描画
		for (const auto& button : buttons_)
		{
			if (button.type == currentType_)
			{
				FrameRenderer::Draw(button.x, button.y, button.sizeX, button.sizeY, FRAME_OFFSET);
			}
			DrawGraph(button.x, button.y, button.graphHandle, true);
		}

		// アルファ値を元に戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		break;
	default:
		break;
	}

}

void GameOver::Release(void)
{
	// 画像の解放
	DeleteGraph(handle_);
	DeleteGraph(crackHandle_);

	for (const auto& button : buttons_)
	{
		DeleteGraph(button.graphHandle);
	}
	buttons_.clear();

	// サウンドの解放
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME_OVER);
}

void GameOver::SelectUpdate(void)
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
		// 当たっていたら
		if (Collision::HitMouseImg2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			// 選択している種類へ変更
			nextType = button.type;
			break;
		}
	}

	// 選択している種類を反映
	currentType_ = nextType;
}

void GameOver::PadSelect(void)
{
	switch (currentType_)
	{
	case GameOver::RETRY:

		// 下ボタンを押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			// タイトルを選択
			currentType_ = RETURN_TITLE;
		}

		break;
	case GameOver::RETURN_TITLE:

		// 上ボタンを押されたら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			// リトライを選択
			currentType_ = RETRY;
		}

		break;
	case GameOver::NONE:

		// リトライを選択
		currentType_ = RETRY;

		break;
	default:
		break;
	}
}

void GameOver::GetShakeOffset(int& offset)
{
	// ヒットストップカウンタが動いていたら
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
	// ステップを設定
	step_ = NON_STEP;
}

void GameOver::ChangeShake(void)
{
	// 揺らし時間を設定
	hitStopCounter_ = SHAKE_TIME;
}

void GameOver::ChangeCrack(void)
{
	// ひび割れSEを再生
	AudioManager::GetInstance()->PlaySE(SoundID::SE_CRACK);

	// ステップを設定
	step_ = CRACK_STEP;
}

void GameOver::ChangeSelect(void)
{
	// BGMを再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAMEOVER);
}

void GameOver::UpdateNone(void)
{
	// ステップが0より小さくなっていたら
	if (step_ < 0.0f)
	{
		// ステートを揺らすに変更
		ChangeState(STATE::SHAKE);
		return;
	}

	// ステップを減らす
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
		// 揺らしが終わったらステートをひび割れに変更
		ChangeState(STATE::CRACK);
	}
}

void GameOver::UpdateCrack(void)
{
	// ステップが0より小さくなっていたら
	if (step_ < 0.0f)
	{
		// ステートを選択に変更
		ChangeState(STATE::SELECT);
		return;
	}

	// ステップを減らす
	step_ -= SceneManager::GetInstance()->GetDeltaTime();
}

void GameOver::UpdateSelect(void)
{
	// アルファ値が最大値ではなかったら
	if (alpha_ < ALPHA_MAX)
	{
		// アルファ値を加算
		alpha_ += ADD_ALPHA;

		// 最大値越えしたら
		if (alpha_ > ALPHA_MAX)
		{
			// 最大値に固定
			alpha_ = ALPHA_MAX;
		}
	}
	else
	{
		// 選択処理
		SelectUpdate();

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
}
