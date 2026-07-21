#include "OptionScene.h"

#include "../../Application.h"
#include "../../Common/Manager/Input/KeyConfigUI.h"

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
	// キーコンフィグUIの更新のみ処理
	keyConfigUI_->Update();
}

void OptionScene::Draw(void)
{
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
