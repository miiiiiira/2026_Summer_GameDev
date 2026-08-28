#include <DxLib.h>

#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Manager/Score/ScoreManager.h"
#include "../SceneManager.h"
#include "../../Object/Component/PlayerController/Upgrade/UpgradeManager.h"
#include "../../Common/Collision/Collision.h"
#include "../GameScene/GameScene.h"
#include "../../Common/MouseCursor/MouseCursor.h"

#include "ShopScene.h"

ShopScene::ShopScene(void)
{
	// マウスを表示させる
	MouseCursor::GetInstance()->SetMouseDraw(true);
}

void ShopScene::Init(void)
{
	// BGMを再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_SHOP);
}

void ShopScene::Load(void)
{
	// 背景読み込み
	backImg_ = LoadGraph("Data/Image/Shop/shopBackScreen.png");

	// アップグレードマネージャーの読み込み処理
	UpgradeManager::GetInstance()->Load();

	// ショップ用の音を読み込み
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::SHOP);
}

void ShopScene::LoadEnd(void)
{
	// 初期化処理
	Init();

	// アップグレードマネージャーの初期化処理
	UpgradeManager::GetInstance()->Init();
}

void ShopScene::Update(void)
{
	// アップグレードマネージャーの更新処理
	UpgradeManager::GetInstance()->Update();

	// アップグレードの終了フラグが立っていたら
	if (UpgradeManager::GetInstance()->GetIsUpgradeEnd())
	{
		// シャッターSEを再生
		AudioManager::GetInstance()->PlaySE(SoundID::SE_SHOP_SHUTTER);
		// ゲームシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameScene>(),GAME, false, Fader::TYPE::SHUTTER);
	}
}

void ShopScene::Draw(void)
{
	// 背景を描画
	DrawGraph(0, 0, backImg_, false);

	// アップグレードマネージャーの描画処理
	UpgradeManager::GetInstance()->Draw();
}

void ShopScene::Release(void)
{
	// 画像ハンドルの解放
	DeleteGraph(backImg_);

	// ショップ用の音を解放
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::SHOP);

	// アップグレードマネージャーの解放処理
	UpgradeManager::GetInstance()->Release();
}
