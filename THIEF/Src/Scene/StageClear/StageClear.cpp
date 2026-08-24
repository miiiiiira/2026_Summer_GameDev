#include "StageClear.h"

#include <DxLib.h>

#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Manager/System/SystemManager.h"
#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../Shop/ShopScene.h"
#include "../TitleScene/TitleScene.h"
#include "../../Common/MouseCursor/MouseCursor.h"

StageClear::StageClear(void)
{
	handle_ = -1;
	// マウスの表示する
	MouseCursor::GetInstance()->SetMouseDraw(true);
	currentType_ = NONE;
}

void StageClear::Init(void)
{
	// BGMを再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_STAGECLEAR);
}

void StageClear::Load(void)
{
	handle_ = LoadGraph("Data/Image/StageClear/StageClear.png");

	// RETRY画像
	buttons_.push_back({ TYPE::NEXT_STAGE, LoadGraph("Data/Image/StageClear/NextStage.png"),
								RETRY_POS_X, RETRY_POS_Y, NEXT_STAGE_SIZE_X, NEXT_STAGE_SIZE_Y });
	// RETURN_TITLE画像
	buttons_.push_back({ TYPE::RETURN_TITLE, LoadGraph("Data/Image/StageClear/ReturnTitle.png"),
							RETURN_TITLE_POS_X, RETURN_TITLE_POS_Y, RETURN_TITLE_SIZE_X, RETURN_TITLE_SIZE_Y });

	AudioManager::GetInstance()->LoadSceneSound(LoadScene::STAGE_CLEAR);
}

void StageClear::LoadEnd(void)
{
	Init();
}

void StageClear::Update(void)
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
	case StageClear::NEXT_STAGE:

		// ゲームシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<ShopScene>(), SHOP);
		return;

		break;
	case StageClear::RETURN_TITLE:

		// タイトルシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<TitleScene>(), TITLE);
		return;

		break;
	default:
		break;
	}
}

void StageClear::Draw(void)
{
	// 画像の描画
	DrawGraph(0, 0, handle_, true);

	// ボタン表示
	for (const auto& button : buttons_)
	{
		if (button.type == currentType_)
		{
			FrameRenderer::Draw(button.x, button.y, button.sizeX, button.sizeY, FRAME_OFFSET);
		}
		DrawGraph(button.x, button.y, button.graphHandle, true);
	}
}

void StageClear::Release(void)
{
	DeleteGraph(handle_);

	for (const auto& button : buttons_)
	{
		DeleteGraph(button.graphHandle);
	}
	buttons_.clear();

	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::STAGE_CLEAR);

}

void StageClear::SelectUpdate(void)
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

void StageClear::MouseSelect(void)
{
	// 衝突判定
	for (const auto& button : buttons_)
	{
		if (Collision::HitMouseImg2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			currentType_ = button.type;
			break;
		}
	}
}

void StageClear::PadSelect(void)
{
	switch (currentType_)
	{
	case StageClear::NEXT_STAGE:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			currentType_ = RETURN_TITLE;
		}

		break;
	case StageClear::RETURN_TITLE:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			currentType_ = NEXT_STAGE;
		}

		break;
	case StageClear::NONE:

		currentType_ = NEXT_STAGE;

		break;
	default:
		break;
	}
}
