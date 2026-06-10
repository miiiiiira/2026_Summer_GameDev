#include "GameOver.h"

#include <DxLib.h>

#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../../Common/Manager/System/SystemManager.h"
#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../GameScene/GameScene.h"
#include "../TitleScene/TitleScene.h"

GameOver::GameOver(void)
{
	handle_ = -1;
	// マウスの表示する
	SetMouseDispFlag(true);
	currentType_ = NONE;
}

GameOver::~GameOver(void)
{
}

void GameOver::Init(void)
{
	handle_ = LoadGraph("Data/Image/GameOver/GameOver.png");

	// RETRY画像
	buttons_.push_back({ TYPE::RETRY, LoadGraph("Data/Image/GameOver/Retry.png"),
								RETRY_POS_X, RETRY_POS_Y, RETRY_SIZE_X, RETRY_SIZE_Y });
	// RETURN_TITLE画像
	buttons_.push_back({ TYPE::RETURN_TITLE, LoadGraph( "Data/Image/GameOver/ReturnTitle.png"),
							RETURN_TITLE_POS_X, RETURN_TITLE_POS_Y, RETURN_TITLE_SIZE_X, RETURN_TITLE_SIZE_Y });
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
	// 選択処理
	SelectUpgrade();

	// マウスを左クリックしなかったら、処理を行わない
	if (!InputManager::GetInstance()->ConfirmButton()) return;

	// 種類が選択されていない場合、処理を行わない
	if (currentType_ == TYPE::NONE) return;

	switch (currentType_)
	{
	case GameOver::RETRY:

		// ゲームシーンへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameScene>());
		return;

		break;
	case GameOver::RETURN_TITLE:

		// タイトルシーンへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<TitleScene>());
		return;

		break;
	default:
		break;
	}

	// ボタン押下のSEを流す
	AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_1);
}

void GameOver::Draw(void)
{
#ifdef _DEBUG

	DrawString(0, 0, "GameOver", GetColor(255, 255, 255));

#endif // _DEBUG

	// 画像の描画
	DrawGraph(GAMEOVER_POS_X, GAMEOVER_POS_Y, handle_, true);

	for (const auto& button : buttons_)
	{
		if (button.type == currentType_)
		{
			FrameRenderer::Draw(button.x, button.y, button.sizeX, button.sizeY, FRAME_OFFSET);
		}
		DrawGraph(button.x, button.y,button.graphHandle, true);
	}

}

void GameOver::Release(void)
{
	DeleteGraph(handle_);
	for (const auto& button : buttons_)
	{
		DeleteGraph(button.graphHandle);
	}
	buttons_.clear();
}

void GameOver::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	TYPE prevType = currentType_;

	if (SystemManager::GetInstance().GetIsDevice())
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
		if (Collision::HitMouse2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
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

		if (InputManager::GetInstance()->SelectDown())
		{
			currentType_ = RETURN_TITLE;
		}

		break;
	case GameOver::RETURN_TITLE:

		if (InputManager::GetInstance()->SelectUp())
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
