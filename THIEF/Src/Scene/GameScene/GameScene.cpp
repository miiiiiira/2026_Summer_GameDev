#include "GameScene.h"

#include <DxLib.h>

#include "../../Application.h"
#include "../../Input/InputManager.h"
#include "../../Audio/AudioManager.h"
#include "../SceneManager.h"
#include "../GameClear/GameClear.h"
#include "../GameOver/GameOver.h"
#include "../Pause/Pause.h"

#include "../../Object/Actor/Item/Goblet/Goblet.h"

#include "../../Object/ObjectManager/ObjectManager.h"
#include "../../Object/Tag.h"

#include "../../Object/Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../Object/Component/Collider/StageCollider/StageCollider.h"
#include "../../Object/Component/Render/Render3D.h"
#include "../../Object/Component/Camera/Camera.h"
#include "../../Object/Component/PlayerController/PlayerController.h"
#include "../../Object/Component/Animation/Animation.h"
#include "../../Object/Component/Stage/Stage.h"
#include "../../Object/Component/Lantern/Lantern.h"
#include "../../Object/Component/Transform/Transform.h"
#include "../../Common/Transform/MatrixUtility.h"


GameScene::GameScene(void)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// オブジェクトマネージャー初期化
	objectManger_->Init();

	// アイテム初期化
	item_->Init();
}

void GameScene::Load(void)
{
	// オブジェクトマネージャーの生成
	objectManger_ = new ObjectManager();

	// ステージの作成
	StageCreate();

	// カメラの作成
	CameraCreate();

	// プレイヤーの作成
	PlayerCreate();

	// ランタンの作成
	LanternCreate();

	// 敵の作成
	EnemyCreate();

	// 杯クラス(アイテム)
	item_ = new Goblet();
	item_->Load();
	// カメラの取得
	auto camera = objectManger_->FindComponentWithTag<Camera>(Tag::Camera);
	VECTOR* cameraPos = &camera->GetTransform()->pos_;
	VECTOR* cameraAngle = camera->GetAngle();
	item_->SetCameraPosAngle(cameraPos, cameraAngle);
}

void GameScene::LoadEnd(void)
{
	Init();
}

void GameScene::Update(void)
{

	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_C))
	{
		// ゲームクリアへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClear>());
	}

	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_O))
	{
		// ゲームオーバーへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameOver>());
	}

	if (InputManager::GetInstance()->IsTrgUp(KEY_INPUT_ESCAPE))
	{
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return;
	}

	// オブジェクトの更新
	objectManger_->Update();

	// アイテム更新
	item_->Update();

	// アイテムとプレイヤーのレンジの当たり判定
	CheckItemPlayerCollision();
}

void GameScene::Draw(void)
{
	// オブジェクトの描画前
	objectManger_->PreDraw();

	// オブジェクトの描画
	objectManger_->Draw();

	// アイテム描画
	item_->Draw();
}

void GameScene::Release(void)
{
	// オブジェクトマネージャー削除
	delete objectManger_;

	// アイテム解放
	item_->Release();
	delete item_;
	item_ = nullptr;
}

void GameScene::CameraCreate(void)
{
	// カメラ生成
	auto cameraObj = objectManger_->CreateObject();

	// タグの付与
	cameraObj->SetTag(Tag::Camera);

	// 座標の設定
	auto cameraTrans = cameraObj->AddComponent<Transform>();

	// カメラコンポーネントの付与
	auto camera = cameraObj->AddComponent<Camera>();

	// カメラのモードを変更
	camera->ChangeMode(Camera::MODE::FOLLOW);
}

void GameScene::PlayerCreate(void)
{
	// プレイヤー生成
	auto player = objectManger_->CreateObject();

	// タグを付与
	player->SetTag(Tag::Player);

	// 座標の設定
	auto trans = player->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };

	// 当たり判定の設定
	auto col = player->AddComponent<CapsuleCollider>();
	col->radius_ = 20.0f;

	// ステージの取得
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);

	// ステージの当たり判定
	auto stageCol = player->AddComponent<StageCollider>();
	stageCol->SetStage(stage);

	// カメラの取得
	auto camera = objectManger_->FindComponentWithTag<Camera>(Tag::Camera);

	// 移動の設定
	auto cont = player->AddComponent<PlayerController>();
	cont->SetCamera(camera);

	// プレイヤーの情報をカメラに設定
	camera->SetTarget(trans);
	camera->SetPlayerController(cont);
}

void GameScene::LanternCreate(void)
{
	// ランタン生成
	auto lantern = objectManger_->CreateObject();

	// タグを付与
	lantern->SetTag(Tag::Lantern);

	// 描画
	auto render = lantern->AddComponent<Render3D>();
	render->SetModel("Data/Model/Lantern/Lantern.mv1");

	// ランタン機能
	auto cont = lantern->AddComponent<Lantern>();
	// カメラの取得
	auto camera = objectManger_->FindComponentWithTag<Camera>(Tag::Camera);
	VECTOR* cameraPos = &camera->GetTransform()->pos_;
	VECTOR* cameraAngle = camera->GetAngle();
	
	// カメラの座標ポインタと向きポインタを付与
	cont->SetCameraPosAngle(cameraPos,cameraAngle);

	// 座標の設定
	auto trans = lantern->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };
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

void GameScene::StageCreate(void)
{
	// ステージの作成
	auto stage = objectManger_->CreateObject();

	// タグを付与
	stage->SetTag(Tag::Stage);

	// 座標の設定
	auto trans = stage->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };

	// 描画
	auto render = stage->AddComponent<Render3D>();
	render->SetModel("Data/Model/Stage/Dummy.mv1");

	// ステージ機能
	stage->AddComponent<Stage>();
}

void GameScene::CheckItemPlayerCollision(void)
{
	// アイテムの座標
	VECTOR itemPos = item_->GetInfo().pos_;
	// アイテムのモデルIDを取得
	int itemModelId = item_->GetModelID();

	// カメラの取得
	auto camera = objectManger_->FindComponentWithTag<Camera>(Tag::Camera);
	VECTOR* cameraPos = &camera->GetTransform()->pos_;
	VECTOR* cameraAngle = camera->GetAngle();

	// 線分の上座標
	VECTOR topPos = *cameraPos;

	// 線分の下座標
	VECTOR downPos = *cameraPos;

	// カメラの回転行列
	VECTOR vec = { cameraAngle->x ,cameraAngle->y ,0.0f };
	MATRIX matRot = Matrix::GetMatrixRotateXYZ(vec);

	// カメラの視線方向のベクトルを計算
	// DxlibのVTransformを使用
	VECTOR forward = VGet(0.0f, 0.0f, 1.0f); // 前方向をZ軸とする
	// カメラの方向を算出
	VECTOR cameraDir = VTransform(forward, matRot);

	// ローカル座標
	VECTOR localPosRot;

	// 相対座標
	VECTOR LOCAL_POS = { 0.0f,0.0f,100.0f };

	localPosRot = VTransform(LOCAL_POS, matRot);

	// 座標に反映
	downPos = VAdd(*cameraPos, localPosRot);

	// 線分とモデルの衝突判定
	MV1_COLL_RESULT_POLY res =
		MV1CollCheck_Line(itemModelId, -1, topPos, downPos);

	if (res.HitFlag)
	{
			// アイテムの追従モードをオンにする
		item_->StartGrabbed({ -20.0f ,0.0f,50.0f });
	}
}