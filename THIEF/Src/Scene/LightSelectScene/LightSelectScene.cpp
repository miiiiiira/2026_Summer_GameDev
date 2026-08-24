#include "LightSelectScene.h"
#include "../../Manager/Light/LightManager.h"
#include "../../Manager/Input/InputManager.h"
#include "../../Manager/System/SystemManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../../Application.h"
#include "../../Common/MouseCursor/MouseCursor.h"
#include "../../Common/Collision/Collision.h"
#include "../MainMenu/MainMenu.h"

LightSelectScene::LightSelectScene(void)
{
	// マウスの表示
	MouseCursor::GetInstance()->SetMouseDraw(true);
}

LightSelectScene::~LightSelectScene(void)
{
}

void LightSelectScene::Init(void)
{
	// デフォルト
	lightType_ = COLOR_0;

	// マウスが現在選択している画像の種類
	selectType_ = LightSelectTypeTable::SELECT_TYPE::NON;

	// BGMを再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_LIGHT_SELECT);
}

void LightSelectScene::Load(void)
{
	// ライトセレクトシーンで使用するサウンドをロード
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::LIGHT_SELECT);

	// 「カラーを選んでね」の画像読み込む
	selectLightColorTextImg_ = LoadGraph("Data/Image/LightSelectScene/SelectLightColorText.png");

	// 「Best」の画像読み込み
	BestTextImg_ = LoadGraph("Data/Image/LightSelectScene/BestText.png");

	// ライトのプレビュー画像を読み込む
	for (auto table : LightSelectSceneWispTable::Table)
	{
		wispImgs_.emplace(table.first, LoadGraph(table.second.c_str()));
	}

	// 選択矢印画像を読み込む
	for (auto selectTypeTable : LightSelectTypeTable::Table)
	{
		selectTypeImg_[selectTypeTable.first] = LoadGraph(selectTypeTable.second.path.c_str());
		selectTypeFrameImg_[selectTypeTable.first] = LoadGraph(selectTypeTable.second.framePath.c_str());
	}
}

void LightSelectScene::LoadEnd(void)
{
	// 初期化処理
	Init();
}

void LightSelectScene::Update(void)
{
	// 選択処理
	SelectUpgrade();

	// 確定処理
	ConfirmUpgrade();
}

void LightSelectScene::Draw(void)
{
	// 「カラーを選んでね」の画像描画
	DrawGraph(0, 0, selectLightColorTextImg_, true);

	// ライトの選ばれている種類によって描画変更
	DrawGraph(0, 0, wispImgs_.find(lightType_)->second, true);
	
	// ライトがデフォルト色だったら
	if (lightType_ == COLOR_0)
	{
		// おすすめふきだし描画
		DrawGraph(0, 0, BestTextImg_, true);
	}

	for (auto selectTypeTable : LightSelectTypeTable::Table)
	{
		DrawRotaGraphF(
			selectTypeTable.second.pos.x,
			selectTypeTable.second.pos.y,
			1.0,
			0.0,
			selectTypeImg_[selectTypeTable.first],
			true);
	}

	// マウスの状態を使ってテーブルの情報を取ってくる
	auto typeTable = LightSelectTypeTable::Table.find(selectType_);

	// 中身があったら
	if (typeTable != LightSelectTypeTable::Table.end())
	{
		DrawRotaGraphF(
			typeTable->second.pos.x,
			typeTable->second.pos.y,
			1.0,
			0.0,
			selectTypeFrameImg_[typeTable->first],
			true);
	}
}

void LightSelectScene::Release(void)
{
	// 画像を解放する
	DeleteGraph(selectLightColorTextImg_);
	DeleteGraph(BestTextImg_);

	for (auto wisp : wispImgs_)
	{
		DeleteGraph(wisp.second);
	}
	wispImgs_.clear();

	for (auto selectTypeTable : LightSelectTypeTable::Table)
	{
		DeleteGraph(selectTypeImg_[selectTypeTable.first]);
		DeleteGraph(selectTypeFrameImg_[selectTypeTable.first]);
	}

	// ライトセレクトシーンで使用するサウンドを削除
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::LIGHT_SELECT);
}

void LightSelectScene::SelectUpgrade(void)
{
	// 前回の選択物を入れておく
	LightSelectTypeTable::SELECT_TYPE type = selectType_;

	// 使っているデバイスによって変更
	if (InputManager::GetInstance()->GetActiveDevice() == InputManager::ActiveDevice::KEY_MOUSE)
	{
		// マウス選択
		MouseSelect();
	}
	else
	{
		// パッド選択
		PadSelect();
	}

	// 中身がNONじゃないかつ、選択物が変わっていたら
	if (selectType_ != LightSelectTypeTable::SELECT_TYPE::NON
		&& selectType_ != type)
	{
		// ボタンに乗ったサウンドを出す
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_SELECT_ON);
	}
}

void LightSelectScene::MouseSelect(void)
{
	selectType_ = LightSelectTypeTable::SELECT_TYPE::NON;

	// 当たり判定取る
	for (auto selectTypeTable : LightSelectTypeTable::Table)
	{
		// 左上座標にする
		Vector2 pos = selectTypeTable.second.pos;
		pos.x -= selectTypeTable.second.size.x / 2;
		pos.y -= selectTypeTable.second.size.y / 2;

		// 当たっていなかったら次の処理へ
		if (!Collision::HitMouseImg2Box(pos, selectTypeTable.second.size.x, selectTypeTable.second.size.y))continue;

		// 選択しているマウス変更
		ChangeSelectType(selectTypeTable.first);
	}
}

void LightSelectScene::PadSelect(void)
{
	switch (selectType_)
	{
	case LightSelectTypeTable::RIGHT_ARROW:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_LEFT))
		{
			ChangeSelectType(LightSelectTypeTable::LEFT_ARROW);
		}

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			ChangeSelectType(LightSelectTypeTable::GAME_START);
		}

		break;
	case LightSelectTypeTable::LEFT_ARROW:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_RIGHT))
		{
			ChangeSelectType(LightSelectTypeTable::RIGHT_ARROW);
		}

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_DOWN))
		{
			ChangeSelectType(LightSelectTypeTable::GAME_START);
		}

		break;
	case LightSelectTypeTable::GAME_START:

		if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::UI_MOVE_UP))
		{
			ChangeSelectType(LightSelectTypeTable::LEFT_ARROW);
		}

		break;
	case LightSelectTypeTable::MAX:
	case LightSelectTypeTable::NON:
		ChangeSelectType(LightSelectTypeTable::LEFT_ARROW);
		break;
	default:
		break;
	}
}

void LightSelectScene::ConfirmUpgrade(void)
{
	// 決定ボタンが押されていなかったら処理を行わない
	if (!InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::DECIDE) && !InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::DECIDE)) return;

	// ライトの種類を整数型にして足し引き算出来るようにする
	int nowlightType = static_cast<int>(lightType_);

	switch (selectType_)
	{
	case LightSelectTypeTable::RIGHT_ARROW:

		// ボタンクリック音を再生
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_3);

		// 次のライトの種類にする
		nowlightType++;

		// 上限値を超えていたら
		if (nowlightType > static_cast<int>(LIGHT_TYPE::COLOR_12))
		{
			// 最初のライトの種類に戻す
			nowlightType = static_cast<int>(LIGHT_TYPE::COLOR_0);
		}

		break;
	case LightSelectTypeTable::LEFT_ARROW:

		// ボタンクリック音を再生
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_3);

		// 前のライトの種類にする
		nowlightType--;

		// 下限値を超えていたら
		if (nowlightType < static_cast<int>(LIGHT_TYPE::COLOR_0))
		{
			// 最後のライトの種類に戻す
			nowlightType = static_cast<int>(LIGHT_TYPE::COLOR_12);
		}

		break;
	case LightSelectTypeTable::GAME_START:

		// ボタンクリック音を再生
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_BUTTON_2);

		// ライトの設定をする
		LightManager::GetInstance()->SetLightType(lightType_);

		// ゲームシーンへ移行
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameScene>(),GAME);
		return;

		break;
	case LightSelectTypeTable::MAX:
		break;
	case LightSelectTypeTable::NON:
		break;
	default:
		break;
	}

	// 設定されたライトの種類を適用
	lightType_ = static_cast<LIGHT_TYPE>(nowlightType);
}

void LightSelectScene::ChangeSelectType(LightSelectTypeTable::SELECT_TYPE type)
{
	selectType_ = type;
}
