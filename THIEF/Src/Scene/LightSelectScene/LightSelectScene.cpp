#include "LightSelectScene.h"
#include "LightSelectInfo.h"
#include "../../Common/Manager/Light/LightManager.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../SceneManager.h"
#include "../../Application.h"

LightSelectScene::LightSelectScene(void)
{
}

LightSelectScene::~LightSelectScene(void)
{
}

void LightSelectScene::Init(void)
{
	// デフォルト
	lightType_ = COLOR_0;
}

void LightSelectScene::Load(void)
{
	// ライトのプレビュー画像を読み込む
	for (auto table : LightSelectSceneTable::Table)
	{
		wispImgs_.emplace(table.first, LoadGraph(table.second.c_str()));
	}

	// 選択矢印画像を読み込む
	selectArrowImg_[RIGHT] = LoadGraph("Data/Image/LightSelectScene/arrow.png");
	selectArrowImg_[LEFT] = LoadGraph("Data/Image/LightSelectScene/arrow.png");

	// ゲームスタートボタン画像を読み込む
	gameStartImg_ = LoadGraph("Data/Image/LightSelectScene/GameStart.png");
}

void LightSelectScene::LoadEnd(void)
{
	Init();
}

void LightSelectScene::Update(void)
{

	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_SPACE))
	{
		// ライトの設定をする
		LightManager::GetInstance().SetLightType(lightType_);

		// ゲームシーンへ移行
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameScene>());
		return;
	}


	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_H))
	{
		switch (lightType_)
		{
		case COLOR_0:
			lightType_ = LIGHT_TYPE::COLOR_1;
			break;
		case COLOR_1:
			lightType_ = LIGHT_TYPE::COLOR_2;
			break;
		case COLOR_2:
			lightType_ = LIGHT_TYPE::COLOR_3;
			break;
		case COLOR_3:
			lightType_ = LIGHT_TYPE::COLOR_4;
			break;
		case COLOR_4:
			lightType_ = LIGHT_TYPE::COLOR_5;
			break;
		case COLOR_5:
			lightType_ = LIGHT_TYPE::COLOR_6;
			break;
		case COLOR_6:
			lightType_ = LIGHT_TYPE::COLOR_7;
			break;
		case COLOR_7:
			lightType_ = LIGHT_TYPE::COLOR_8;
			break;
		case COLOR_8:
			lightType_ = LIGHT_TYPE::COLOR_9;
			break;
		case COLOR_9:
			lightType_ = LIGHT_TYPE::COLOR_10;
			break;
		case COLOR_10:
			lightType_ = LIGHT_TYPE::COLOR_11;
			break;
		case COLOR_11:
			lightType_ = LIGHT_TYPE::COLOR_12;
			break;
		case COLOR_12:
			lightType_ = LIGHT_TYPE::COLOR_0;
			break;
		default:
			break;
		}

	}



}

void LightSelectScene::Draw(void)
{
	// ライトの選ばれている種類によって描画変更
	DrawGraph(0, 0, wispImgs_.find(lightType_)->second, true);
	
	// ライトがデフォルト色だったら
	if (lightType_ == COLOR_0)
	{
		// おすすめふきだし描画
	}

	// 矢印ボタン
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2 - 44-80, Application::SCREEN_SIZE_Y / 2, 1.0, 0.0,
		selectArrowImg_[RIGHT], true);

	// 矢印ボタン
	DrawRotaGraph(44 + 10, Application::SCREEN_SIZE_Y / 2, 1.0, 0.0, selectArrowImg_[LEFT], true, true);

	// ゲームスタートボタン
	DrawGraph(Application::SCREEN_SIZE_X / 2 - 166, Application::SCREEN_SIZE_Y - 58 - 30, gameStartImg_, true);


}

void LightSelectScene::Release(void)
{
	// 画像を解放する
	for (auto wisp : wispImgs_)
	{
		DeleteGraph(wisp.second);
	}
	wispImgs_.clear();

	DeleteGraph(selectArrowImg_[ARROW_TYPE::RIGHT]);
	DeleteGraph(selectArrowImg_[ARROW_TYPE::LEFT]);

	DeleteGraph(gameStartImg_);
}
