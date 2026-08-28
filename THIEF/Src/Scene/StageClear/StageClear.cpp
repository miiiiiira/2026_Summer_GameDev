#include <DxLib.h>

#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../Shop/ShopScene.h"
#include "../TitleScene/TitleScene.h"
#include "../../Common/MouseCursor/MouseCursor.h"

#include "StageClear.h"

StageClear::StageClear(void)
{
	// マウスの表示する
	MouseCursor::GetInstance()->SetMouseDraw(true);
}

void StageClear::Init(void)
{
	// 現在選択している種類の初期化
	currentType_ = NONE;

	// BGMを再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_STAGECLEAR);
}

void StageClear::Load(void)
{
	// 画像を読み込み
	// 「Stage Clear」画像
	handle_ = LoadGraph("Data/Image/StageClear/StageClear.png");

	// RETRY画像
	buttons_.push_back({ TYPE::NEXT_STAGE, LoadGraph("Data/Image/StageClear/NextStage.png"),
								RETRY_POS_X, RETRY_POS_Y, NEXT_STAGE_SIZE_X, NEXT_STAGE_SIZE_Y });
	// RETURN_TITLE画像
	buttons_.push_back({ TYPE::RETURN_TITLE, LoadGraph("Data/Image/StageClear/ReturnTitle.png"),
							RETURN_TITLE_POS_X, RETURN_TITLE_POS_Y, RETURN_TITLE_SIZE_X, RETURN_TITLE_SIZE_Y });

	// ステージクリア用の音を読み込み
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::STAGE_CLEAR);
}

void StageClear::LoadEnd(void)
{
	// 初期化処理
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

		// ショップシーンへ
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
	// 「Stage Clear」画像の描画
	DrawGraph(0, 0, handle_, true);

	for (const auto& button : buttons_)
	{
		// 選択している種類の場所にフレームを描画
		if (button.type == currentType_)
		{
			FrameRenderer::Draw(button.x, button.y, button.sizeX, button.sizeY, FRAME_OFFSET);
		}

		// ボタン表示
		DrawGraph(button.x, button.y, button.graphHandle, true);
	}
}

void StageClear::Release(void)
{
	// 画像を解放
	DeleteGraph(handle_);

	for (const auto& button : buttons_)
	{
		DeleteGraph(button.graphHandle);
	}
	buttons_.clear();

	// ステージクリア用の音を解放
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
		// 当たっていたら
		if (Collision::HitMouseImg2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			// 現在選択している種類を変更
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

		// 下を押したら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			// タイトルボタンへ
			currentType_ = RETURN_TITLE;
		}

		break;
	case StageClear::RETURN_TITLE:

		// 上を押したら
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			// ネクストステージボタンへ
			currentType_ = NEXT_STAGE;
		}

		break;
	case StageClear::NONE:

		// ネクストステージボタンへ
		currentType_ = NEXT_STAGE;

		break;
	default:
		break;
	}
}
