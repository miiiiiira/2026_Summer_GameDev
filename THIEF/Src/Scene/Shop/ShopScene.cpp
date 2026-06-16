#include <DxLib.h>
#include "ShopScene.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../../Common/Manager/Score/ScoreManager.h"
#include "../SceneManager.h"
#include "../../Object/Component/PlayerController/Upgrade/UpgradeManager.h"
#include "../../Common/Collision/Collision.h"
#include "../GameScene/GameScene.h"

ShopScene::ShopScene(void)
{
	// マウスを表示させる
	SetMouseDispFlag(true);
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
	UpgradeManager::GetInstance().Load();

	AudioManager::GetInstance()->LoadSceneSound(LoadScene::SHOP);
}

void ShopScene::LoadEnd(void)
{
	Init();
	UpgradeManager::GetInstance().Init();
}

void ShopScene::Update(void)
{
	UpgradeManager::GetInstance().Update();
}

void ShopScene::Draw(void)
{
#ifdef _DEBUG

	DrawString(10, 10, "ShopScene", 0xff0000);

#endif // _DEBUG

	UpgradeManager::GetInstance().Draw();
}

void ShopScene::Release(void)
{
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::SHOP);
	UpgradeManager::GetInstance().Release();
}
