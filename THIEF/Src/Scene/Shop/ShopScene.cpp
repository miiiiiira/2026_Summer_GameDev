#include <DxLib.h>
#include "ShopScene.h"
#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Manager/Score/ScoreManager.h"
#include "../SceneManager.h"
#include "../../Object/Component/PlayerController/Upgrade/UpgradeManager.h"
#include "../../Common/Collision/Collision.h"
#include "../GameScene/GameScene.h"
#include "../../Common/MouseCursor/MouseCursor.h"

ShopScene::ShopScene(void)
{
	// マウスを表示させる
	MouseCursor::GetInstance()->SetMouseDraw(true);
}

ShopScene::~ShopScene(void)
{
}

void ShopScene::Init(void)
{
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_SHOP);
}

void ShopScene::Load(void)
{
	// 背景読み込み
	backImg_ = LoadGraph("Data/Image/Shop/shopBackScreen.png");

	UpgradeManager::GetInstance()->Load();

	AudioManager::GetInstance()->LoadSceneSound(LoadScene::SHOP);
}

void ShopScene::LoadEnd(void)
{
	Init();
	UpgradeManager::GetInstance()->Init();
}

void ShopScene::Update(void)
{
	UpgradeManager::GetInstance()->Update();

	// アップグレードの終了フラグが立っていたら
	if (UpgradeManager::GetInstance()->GetIsUpgradeEnd())
	{
		// ゲームシーンへ
		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_SHUTTER);
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameScene>(),GAME, false, Fader::TYPE::SHUTTER);
	}
}

void ShopScene::Draw(void)
{
	DrawGraph(0, 0, backImg_, false);
#ifdef _DEBUG

	DrawString(10, 10, "ShopScene", 0xff0000);

#endif // _DEBUG

	UpgradeManager::GetInstance()->Draw();
}

void ShopScene::Release(void)
{
	DeleteGraph(backImg_);
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::SHOP);
	UpgradeManager::GetInstance()->Release();
}
