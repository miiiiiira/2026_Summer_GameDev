#include "OptionScene.h"

#include "../../Application.h"
#include "../../Common/Manager/Input/KeyConfigUI.h"
#include "../../Common/Manager/Input/InputManager.h"
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
}

void OptionScene::Release(void)
{
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

void OptionScene::SelectNaviUp(void)
{
}

void OptionScene::SelectNaviDown(void)
{
}

void OptionScene::MouseNaviSelect(void)
{
}

void OptionScene::UpdateSettingsContent(void)
{
}
