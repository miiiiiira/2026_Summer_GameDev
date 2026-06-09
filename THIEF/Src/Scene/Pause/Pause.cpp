#include <memory>
#include <vector>

#include "Pause.h"

#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../../Common/Manager/System/SystemManager.h"
#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../SceneManager.h"
#include "../../Common/Collision/Collision.h"
#include "../../Application.h"
#include "../Confirm/Confirm.h"

Pause::Pause(void)
{
	handle_ = -1;
	confirm_ = nullptr;

	// マウスの表示する
	SetMouseDispFlag(true);
}

Pause::~Pause(void)
{
}

void Pause::Init(void)
{
	ChangeSelect(MENU::NONE);

	confirm_ = std::make_shared<Confirm>();
}

void Pause::Load(void)
{
	handle_ = LoadGraph((Application::PATH_IMAGE + "pause.png").c_str());		// PAUSEの文字画像

	menuButtons_.clear();

	// CONTINUE画像
	menuButtons_.push_back({ MENU::CONTINUE, LoadGraph((Application::PATH_IMAGE + "continue.png").c_str()),
								CONTINUE_POS_X, CONTINUE_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// OPTION画像
	menuButtons_.push_back({ MENU::OPTION, LoadGraph((Application::PATH_IMAGE + "option.png").c_str()),
								OPTION_POS_X, OPTION_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// MAIN MENU画像
	menuButtons_.push_back({ MENU::MAINMENU,  LoadGraph((Application::PATH_IMAGE + "mainMenu.png").c_str()),
								MAINMENU_POS_X, MAINMENU_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// QUIT画像
	menuButtons_.push_back({ MENU::QUIT, LoadGraph((Application::PATH_IMAGE + "quit.png").c_str()),
								QUIT_POS_X, QUIT_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });

}

void Pause::LoadEnd(void)
{
	Init();
}

void Pause::Update(void)
{
	// Escape押したら
	if (InputManager::GetInstance()->PauseButtons())
	{
		// ゲームシーンへ戻る(ポーズモードを終了する)
		SceneManager::GetInstance()->PopScene();
	}

	// 選択処理
	SelectUpgrade();

	// マウスを左クリックされていなかったら、ここで終了
	if (!InputManager::GetInstance()->ConfirmButton()) return;

	// 選択されているメニューがない場合も、ここで終了
	if (currentMenu_ == MENU::NONE) return;

	// クリックされていて、かつメニューが選ばれている場合
	switch (currentMenu_)
	{
	case MENU::CONTINUE:
		UpdateContinue();
		break;

	case MENU::OPTION:
		UpdateOption();
		break;

	case MENU::MAINMENU:
		UpdateMainMenu();
		break;

	case MENU::QUIT:
		UpdateQuit();
		break;
	}

	// TODO　決定SEを流す

}

void Pause::Draw(void)
{
	// 背景色を半透明で表示
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawGraph(PAUSE_POS_X, PAUSE_POS_Y, handle_, true);					// PAUSEの文字を表示


	for (const auto& button : menuButtons_)
	{
		if (button.type == currentMenu_)
		{
			FrameRenderer::Draw(button.x,button.y);
		}

		DrawGraph(button.x, button.y, button.graphHandle, true);		// メニューの文字を表示
	}
}

void Pause::Release(void)
{
	for (const auto& button : menuButtons_)
	{
		DeleteGraph(button.graphHandle);
	}
	menuButtons_.clear();

	DeleteGraph(handle_);
	handle_ = -1;
}

void Pause::ChangeSelect(MENU menu)
{
	currentMenu_ = menu;
}

void Pause::UpdateContinue(void)
{
	// マウスカーソルを画面中央に戻す
	SetMousePoint(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2);

	// マウスの表示を消す
	SetMouseDispFlag(false);

	// ゲームシーンへ
	SceneManager::GetInstance()->PopScene();
}

void Pause::UpdateOption(void)
{

}

void Pause::UpdateMainMenu(void)
{
	// 確認シーンへ
	confirm_->ChangeType(Confirm::TYPE::MAIN_MENU);
	SceneManager::GetInstance()->PushScene(confirm_);
}

void Pause::UpdateQuit(void)
{
	// 確認シーンへ
	confirm_->ChangeType(Confirm::TYPE::QUIT);
	SceneManager::GetInstance()->PushScene(confirm_);
}

void Pause::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	MENU prevSelect = currentMenu_;

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
	if (currentMenu_ != MENU::NONE
		&& currentMenu_ != prevSelect)
	{
		// TODO　選択SEを流す

	}
}

void Pause::MouseSelect(void)
{
	MENU nextSelect = MENU::NONE;

	// 衝突判定
	for (const auto& button : menuButtons_)
	{
		if (Collision::HitMouse2Box({ static_cast<float>(button.x), static_cast<float>(button.y) },
			static_cast<float>(button.sizeX), static_cast<float>(button.sizeY)))
		{
			nextSelect = button.type;
			break;
		}
	}

	// 最後に一回だけ状態を更新する
	ChangeSelect(nextSelect);
}

void Pause::PadSelect(void)
{
	auto prevMenu = currentMenu_;

	switch (currentMenu_)
	{
	case Pause::MENU::NONE:

		ChangeSelect(MENU::CONTINUE);

		break;
	case Pause::MENU::CONTINUE:
		if (InputManager::GetInstance()->SelectDown())
		{
			ChangeSelect(MENU::OPTION);
		}
		break;
	case Pause::MENU::OPTION:

		if (InputManager::GetInstance()->SelectUp())
		{
			ChangeSelect(MENU::CONTINUE);
		}

		if (InputManager::GetInstance()->SelectDown())
		{
			ChangeSelect(MENU::MAINMENU);
		}

		break;
	case Pause::MENU::MAINMENU:

		if (InputManager::GetInstance()->SelectUp())
		{
			ChangeSelect(MENU::OPTION);
		}

		if (InputManager::GetInstance()->SelectDown())
		{
			ChangeSelect(MENU::QUIT);
		}

		break;
	case Pause::MENU::QUIT:

		if (InputManager::GetInstance()->SelectUp())
		{
			ChangeSelect(MENU::MAINMENU);
		}

		break;
	default:
		break;
	}
}

