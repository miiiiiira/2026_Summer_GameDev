#include "GameScene.h"

#include <DxLib.h>

#include "../../Application.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../../Common/Manager/Score/ScoreManager.h"
#include "../SceneManager.h"
#include "../GameClear/GameClear.h"
#include "../GameOver/GameOver.h"
#include "../Pause/Pause.h"

#include "../../Object/Actor/Enemy/Yeti/Yeti.h"
#include "../../Common/Crosshair/Crosshair.h"
#include "../../Object/ObjectManager/ObjectManager.h"
#include "../../Object/Tag.h"

#include "../../Object/Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../Object/Component/Collider/StageCollider/StageCollider.h"
#include "../../Object/Component/Collider/ItemCollider/ItemCollider.h"
#include "../../Object/Component/Collider/DeliveryLocationCollider/DeliveryLocationCollider.h"
#include "../../Object/Component/Render/Render3D.h"
#include "../../Object/Component/Camera/Camera.h"
#include "../../Object/Component/PlayerController/PlayerController.h"
#include "../../Object/Component/Animation/Animation.h"
#include "../../Object/Component/Stage/Stage.h"
#include "../../Object/Component/Lantern/Lantern.h"
#include "../../Object/Component/Item/Item.h"
#include "../../Object/Component/Item/Goblet/Goblet.h"
#include "../../Object/Component/Transform/Transform.h"
#include "../../Common/Transform/MatrixUtility.h"
#include "../../Common/CameraUtility/CameraUtility.h"


#include "../../Common/Collision/Collision.h"


GameScene::GameScene(void)
{
	// マウスの表示を消す
	SetMouseDispFlag(false);
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// スコアの初期化
	ScoreManager::GetInstance().ResetGame();

	// オブジェクトマネージャー初期化
	objectManger_->Init();

	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	enemy_->Init(stage->GetModelId());

	// クロスヘアの初期化処理
	crosshair_->Init();
}

void GameScene::Load(void)
{
	// オブジェクトマネージャーの生成
	objectManger_ = new ObjectManager();

	enemy_ = new Yeti();
	enemy_->Load();

	// クロスヘアの作成
	crosshair_ = new Crosshair();
	crosshair_->Load();

	// カメラの作成
	CameraCreate();

	// カメラユーティリティにカメラのポインタを渡す
	CameraUtility::SetCameraPoint(objectManger_->FindComponentWithTag<Camera>(Tag::Camera));

	// ステージの作成
	StageCreate();

	// ランタンの作成
	LanternCreate();

	// プレイヤーの作成
	PlayerCreate();

	// 敵の作成
	EnemyCreate();

	// アイテムの作成
	ItemCreate();

	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	// スコアマネージャーにアイテムたちを渡す
	ScoreManager::GetInstance().SetItems(stage->GetItems());
}

void GameScene::LoadEnd(void)
{
	Init();
}

void GameScene::Update(void)
{
#ifdef _DEBUG

	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_C))
	{
		// ゲームクリアにする
		SceneManager::GetInstance()->TrueGameClear();
	}

	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_O))
	{
		// ゲームオーバーにする
		SceneManager::GetInstance()->TrueGameOver();
	}

#endif // _DEBUG

	// オブジェクトの更新
	objectManger_->Update();

	enemy_->Update();

	// クロスヘアの更新
	crosshair_->Update();

	// スコアマネージャーの更新
	ScoreManager::GetInstance().Update();

	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_ESCAPE))
	{
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return;
	}

	if (SceneManager::GetInstance()->GetIsClear())
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClear>());
		return;
	}

	if (SceneManager::GetInstance()->GetIsOver())
	{
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameOver>());
		return;
	}

}

void GameScene::Draw(void)
{
	// オブジェクトの描画前
	objectManger_->PreDraw();

	enemy_->Draw();

	// オブジェクトの描画
	objectManger_->Draw();

	// クロスヘアの描画
	crosshair_->Draw();

	// 納品金額 / 目標金額の描画
	ScoreManager::GetInstance().Draw();
}

void GameScene::Release(void)
{
	// オブジェクトマネージャー削除
	delete objectManger_;

	enemy_->Release();
	delete enemy_;
	enemy_ = nullptr;

	crosshair_->Release();
	delete crosshair_;
	crosshair_ = nullptr;
}

void GameScene::CameraCreate(void)
{
	// カメラ生成
	auto cameraObj = objectManger_->CreateObject();

	// タグの付与
	cameraObj->SetTagAndPriority(Tag::Camera);

	// 座標の設定
	auto cameraTrans = cameraObj->AddComponent<Transform>();

	// カメラコンポーネントの付与
	auto camera = cameraObj->AddComponent<Camera>();

	// カメラのモードを変更
	camera->ChangeMode(Camera::MODE::FOLLOW);
}

void GameScene::StageCreate(void)
{
	// ステージの作成
	auto stage = objectManger_->CreateObject();

	// タグを付与
	stage->SetTagAndPriority(Tag::Stage);

	// 座標の設定
	auto trans = stage->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };

	// 描画
	auto render = stage->AddComponent<Render3D>();
	render->SetModel("Data/Model/Stage/Dummy.mv1");

	// ステージ機能
	stage->AddComponent<Stage>();

	// 納品場所の当たり判定追加
	auto delivery = stage->AddComponent<DeliveryLocationCollider>();
	delivery->SetCrosshair(crosshair_);
}

void GameScene::LanternCreate(void)
{
	// ランタン生成
	auto lantern = objectManger_->CreateObject();

	// タグを付与
	lantern->SetTagAndPriority(Tag::Lantern);

	// 描画
	auto render = lantern->AddComponent<Render3D>();
	render->SetModel("Data/Model/Lantern/Lantern.mv1");

	// ランタン機能
	auto cont = lantern->AddComponent<Lantern>();

	// 座標の設定
	auto trans = lantern->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };
}

void GameScene::PlayerCreate(void)
{
	// プレイヤー生成
	auto player = objectManger_->CreateObject();

	// タグを付与
	player->SetTagAndPriority(Tag::Player);

	// 座標の設定
	auto trans = player->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };

	// 移動の設定
	auto cont = player->AddComponent<PlayerController>();

	// カメラの取得
	auto camera = objectManger_->FindComponentWithTag<Camera>(Tag::Camera);
	// プレイヤーの情報をカメラに設定
	camera->SetTarget(trans);
	camera->SetPlayerController(cont);

	// 当たり判定の設定
	auto col = player->AddComponent<CapsuleCollider>();
	col->radius_ = 20.0f;

	// ステージの取得
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);

	// ステージの当たり判定
	auto stageCol = player->AddComponent<StageCollider>();
	stageCol->SetStage(stage);

	// プレイヤー取得
	auto playerController = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);

	// ランタン取得
	auto lantern = objectManger_->FindComponentWithTag<Lantern>(Tag::Lantern);

	// ランタンのポインタを渡す
	playerController->SetLantern(lantern);

	// 納品場所用当たり判定にプレイヤーを渡す
	auto deliveryCol = stage->GetOwner()->GetComponent<DeliveryLocationCollider>();
	deliveryCol->SetPlayer(playerController);
}

void GameScene::EnemyCreate(void)
{
	//// 敵の生成
	//auto enemy = objectManger_->CreateObject();

	//// タグの付与
	//enemy->SetTag(Tag::Enemy);

	//// 座標の設定
	//auto trans = enemy->AddComponent<Transform>();
	//trans->pos_ = { 0,0,0 };

	//// 描画の設定
	//auto render = enemy->AddComponent<Render3D>();
	//render->SetModel("Data/Model/Player/Player.mv1");

	//// 当たり判定の設定
	//auto col = enemy->AddComponent<CapsuleCollider>();
	//col->radius_ = 20.0f;
}

void GameScene::ItemCreate(void)
{
	// アイテムの作成
	auto item = objectManger_->CreateObject();

	// タグを付与
	item->SetTagAndPriority(Tag::Goblet);

	// 座標の設定
	auto trans = item->AddComponent<Transform>();
	trans->pos_ = { 0.0f,100.0f,0.0f };

	// 描画
	auto render = item->AddComponent<Render3D>();
	render->SetModel("Data/Model/Item/Goblet.mv1");

	// アイテム機能
	auto goblet = item->AddComponent<Goblet>();

	// プレイヤー取得
	auto playerController = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);

	// ステージ取得
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);

	// アイテムの当たり判定
	auto itemCol = item->AddComponent<ItemCollider>();
	// プレイヤーを渡す
	itemCol->SetPlayer(playerController);
	// ステージを渡す
	itemCol->SetStage(stage);
	// クロスヘアを渡す
	itemCol->SetCrosshair(crosshair_);

	// ステージにアイテムを渡す
	stage->SetItem(goblet);

}
