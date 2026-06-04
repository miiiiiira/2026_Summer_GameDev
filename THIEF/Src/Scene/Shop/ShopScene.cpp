#include <DxLib.h>
#include "ShopScene.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Score/ScoreManager.h"
#include "../SceneManager.h"
#include "../../Object/Component/PlayerController/Upgrade/UpgradeManager.h"
#include "../../Common/Collision/Collision.h"
#include "../GameScene/GameScene.h"

ShopScene::ShopScene(void)
{
	// マウスを表示させる
	SetMouseDispFlag(true);

	endButtonImg_ = -1;
}

ShopScene::~ShopScene(void)
{
}

void ShopScene::Init(void)
{
}

void ShopScene::Load(void)
{
	endButtonImg_ = LoadGraph("Data/Image/endButton.png");
	UpgradeManager::GetInstance().Load();
}

void ShopScene::LoadEnd(void)
{
	UpgradeManager::GetInstance().Init();
}

void ShopScene::Update(void)
{
	// 当たっていたら
	if (Collision::HitMouse2Box({ END_BUTTON_POS_X ,END_BUTTON_POS_Y}, COL_SIZE_X, COL_SIZE_Y))
	{
		// ボタンが押されると次のシーンへ
		if (InputManager::GetInstance()->PushAnyButton())
		{
			// アップグレードを終了させる
			UpgradeManager::GetInstance().TrueIsUpgradeEnd();
			// ゲームシーンへ
			SceneManager::GetInstance()->ChangeScene(std::make_shared<GameScene>());
		}
	}

	UpgradeManager::GetInstance().Update();

}

void ShopScene::Draw(void)
{
#ifdef _DEBUG

	DrawString(10, 10, "ShopScene", 0xff0000);

#endif // _DEBUG

	// 持っている金額
	int price = ScoreManager::GetInstance().GetTotalPrice();
	DrawFormatString(Application::SCREEN_SIZE_X - 150, 50, 0xffffff, "%d", price);

	// 終了ボタンの描画
	DrawGraph(END_BUTTON_POS_X, END_BUTTON_POS_Y, endButtonImg_, true);

	UpgradeManager::GetInstance().Draw();

}

void ShopScene::Release(void)
{
	// 画像を解放
	DeleteGraph(endButtonImg_);

	UpgradeManager::GetInstance().Release();
}
