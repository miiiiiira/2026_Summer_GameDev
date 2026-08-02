#include "OptionScene.h"

#include "../../Application.h"
#include "../../Common/Manager/Input/KeyConfigUI.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/FrameRenderer/FrameRenderer.h"
#include "../../Scene/SceneManager.h"

OptionScene::OptionScene(void)
	:
	keyConfigUI_(nullptr)
{
}

OptionScene::~OptionScene(void)
{
}

void OptionScene::Init(void)
{
	// キーコンフィグUIの初期化
	keyConfigUI_->Init();

	// 最初からキーコンフィグUIをアクティブにする
	keyConfigUI_->SetActive(true);
}

void OptionScene::Load(void)
{
	keyConfigUI_ = new KeyConfigUI();
	keyConfigUI_->Load();

	// CONTROLS画像
	naviButtons_.push_back({ MENU::CONTROLS, LoadGraph((Application::PATH_IMAGE + "Option/controls.png").c_str()),
								NAVI_POS_X, NAVI_POS_Y, IMAGE_SIZE_X, IMAGE_SIZE_Y });
	// BACK画像
	naviButtons_.push_back({ MENU::BACK, LoadGraph((Application::PATH_IMAGE + "Option/back.png").c_str()),
							NAVI_POS_X, BACK_POS_Y, BACK_IMAGE_SIZE_X, BACK_IMAGE_SIZE_Y });
}

void OptionScene::LoadEnd(void)
{
	keyConfigUI_->LoadEnd();
}

void OptionScene::Update(void)
{
	if (!keyConfigUI_->IsWaiting())
	{
		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::CANCEL) || InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::CANCEL))
		{
			// ポーズシーンへ
			SceneManager::GetInstance()->PopScene();
			return;
		}
	}

	// キーコンフィグUIの更新のみ処理
	keyConfigUI_->Update();
}

void OptionScene::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0xffffff, true);

	// キーコンフィグUIを描画
	keyConfigUI_->Draw();

	//for (const auto& button : naviButtons_)
	//{
	//	if (button.type == currentNavi_)
	//	{
	//		FrameRenderer::Draw(button.x, button.y);
	//	}
	//	DrawGraph(button.x, button.y, button.graphHandle, true);		// メニューボタンの画像
	//}
}

void OptionScene::Release(void)
{
	for (const auto& button : naviButtons_)
	{
		DeleteGraph(button.graphHandle);
	}
	naviButtons_.clear();

	// メモリの解放
	if (keyConfigUI_ != nullptr)
	{
		keyConfigUI_->Delete();
		delete keyConfigUI_;
		keyConfigUI_ = nullptr;
	}
}

void OptionScene::UpdateNaviSelect(void)
{
}

void OptionScene::UpdateSettingsContent(void)
{
}