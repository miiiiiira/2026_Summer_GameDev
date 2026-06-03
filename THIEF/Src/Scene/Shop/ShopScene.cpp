#include <DxLib.h>
#include "ShopScene.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../SceneManager.h"
#include "../GameScene/GameScene.h"

ShopScene::ShopScene(void)
{
}

ShopScene::~ShopScene(void)
{
}

void ShopScene::Init(void)
{
}

void ShopScene::Load(void)
{
}

void ShopScene::LoadEnd(void)
{
}

void ShopScene::Update(void)
{
	// ボタンが押されると次のシーンへ
	if (InputManager::GetInstance()->PushAnyButton())
	{
		// ゲームシーンへ
		SceneManager::GetInstance()->PushScene(std::make_shared<GameScene>());
	}
}

void ShopScene::Draw(void)
{
#ifdef _DEBUG

	DrawString(10, 10, "ShopScene", 0xff0000);

#endif // _DEBUG
}

void ShopScene::Release(void)
{
}
