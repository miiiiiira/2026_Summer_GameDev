#include <DxLib.h>
#include "ShopScene.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Score/ScoreManager.h"
#include "../SceneManager.h"
#include "../../Application.h"
#include "../../Object/Component/PlayerController/Upgrade/UpgradeManager.h"
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

}

void ShopScene::Load(void)
{
	UpgradeManager::GetInstance().Load();
}

void ShopScene::LoadEnd(void)
{
	UpgradeManager::GetInstance().Init();
}

void ShopScene::Update(void)
{
	//// ボタンが押されると次のシーンへ
	//if (InputManager::GetInstance()->PushAnyButton())
	//{
	//	// ゲームシーンへ
	//	SceneManager::GetInstance()->ChangeScene(std::make_shared<GameScene>());
	//}
	UpgradeManager::GetInstance().Update();

}

void ShopScene::Draw(void)
{
#ifdef _DEBUG

	DrawString(10, 10, "ShopScene", 0xff0000);

#endif // _DEBUG

	int price = ScoreManager::GetInstance().GetTotalPrice();

	DrawFormatString(Application::SCREEN_SIZE_X - 150, 50, 0xffffff, "%d", price);

	UpgradeManager::GetInstance().Draw();

}

void ShopScene::Release(void)
{
	UpgradeManager::GetInstance().Release();
}
