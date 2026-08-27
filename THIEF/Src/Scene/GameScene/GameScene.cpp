#include <EffekseerForDXLib.h>
#include <algorithm>

#include "../../Application.h"
#include "../../Manager/Input/InputManager.h"
#include "../../Manager/Audio/AudioManager.h"
#include "../../Manager/Score/ScoreManager.h"
#include "../../Manager/EffectResManager/EffectResManager.h"
#include "../../Manager/PlayerStatus/PlayerStatusManager.h"
#include "../SceneManager.h"
#include "../GameClear/GameClear.h"
#include "../GameOver/GameOver.h"
#include "../Shop/ShopScene.h"
#include "../Pause/Pause.h"
#include "../../Object/Actor/Enemy/EnemyManager.h"
#include "../../Object/Actor/Enemy/EnemyBase.h"
#include "../../Object/Actor/Enemy/Weapon/WeaponBase.h"
#include "../../Object/ObjectManager/ObjectManager.h"
#include "../../Object/Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../Object/Component/Collider/StageCollider/StageCollider.h"
#include "../../Object/Component/Collider/ItemCollider/ItemCollider.h"
#include "../../Object/Component/Collider/DeliveryLocationCollider/DeliveryLocationCollider.h"
#include "../../Object/Component/Render/Render3D.h"
#include "../../Object/Component/Camera/Camera.h"
#include "../../Object/Component/PlayerController/PlayerController.h"
#include "../../Object/Component/PlayerController/Map/Map.h"
#include "../../Object/Component/Animation/Animation.h"
#include "../../Object/Component/Stage/Stage.h"
#include "../../Object/Component/Cart/Cart.h"
#include "../../Object/Component/Wisp/Wisp.h"
#include "../../Object/Component/Item/Item.h"
#include "../../Object/Component/Item/ItemInfo.h"
#include "../../Object/Component/Item/Goblet/Goblet.h"
#include "../../Object/Component/Item/Potion/Potion.h"
#include "../../Object/Component/Item/Amphora/Amphora.h"
#include "../../Object/Component/Item/Bottle/Bottle.h"
#include "../../Object/Component/Item/Jar/Jar.h"
#include "../../Object/Component/Item/Mug/Mug.h"
#include "../../Object/Component/Item/Skull/Skull.h"
#include "../../Object/Component/Crosshair/Crosshair.h"
#include "../../Object/Component/Effect/FlashEffect.h"
#include "../../Object/Component/Transform/Transform.h"
#include "../../Common/Transform/MatrixUtility.h"
#include "../../Common/CameraUtility/CameraUtility.h"
#include "../../Common/Collision/Collision.h"
#include "../../Object/Component/Collider/CartCollider/CartCollider.h"
#include "../../Common/MouseCursor/MouseCursor.h"
#include "../StageClear/StageClear.h"

#include "GameScene.h"

GameScene::GameScene(void)
{
	// マウスの表示を消す
	MouseCursor::GetInstance()->SetMouseDraw(false);
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// スコアの初期化
	ScoreManager::GetInstance()->ResetGame();

	// オブジェクトマネージャー初期化
	objectManger_->Init();

	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	auto player = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);
	enemyManager_->Init(player,stage->GetModelId());

	// 現在のステージ数を見て生成する敵を変更
	switch (SceneManager::GetInstance()->GetCurrentStage())
	{
	case STAGE_NUM::STAGE_1:
		// ステージ1
		enemyManager_->CreateEnemyStage1();
		break;
	case STAGE_NUM::STAGE_2:
		// ステージ2
		enemyManager_->CreateEnemyStage2();
		break;
	case STAGE_NUM::STAGE_3:
		// ステージ3
		enemyManager_->CreateEnemyStage3();
		break;
	default:
		break;
	}

	// BGM再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME_1);
}

void GameScene::Load(void)
{
	// サウンド読みこみ
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);

	// エフェクト管理初期化
	EffectResManager::CreateInstance();

	// オブジェクトマネージャーの生成
	objectManger_ = new ObjectManager();

	// クロスヘアの生成
	CrosshairCreate();

	// 現在のステージ数を見て初期化処理を変更
	switch (SceneManager::GetInstance()->GetCurrentStage())
	{
	case STAGE_NUM::STAGE_1:

		// ステージ1の初期化処理
		Stage1Init();

		break;
	case STAGE_NUM::STAGE_2:

		// ステージ2の初期化処理
		Stage2Init();

		break;
	case STAGE_NUM::STAGE_3:
		
		// ステージ3の初期化処理
		Stage3Init();

		break;
	default:
		break;
	}
}

void GameScene::LoadEnd(void)
{
	Init();

	// エフェクシアが非同期ロードに対応していないためここでロード
	EffectResManager::GetInstance().Load();

	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	// スコアマネージャーにアイテムたちを渡す
	ScoreManager::GetInstance()->SetItems(stage->GetItems());

	auto map = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player)->GetOwner()->GetComponent<Map>();
	// マップにアイテムたちを渡す
	map->SetItems(stage->GetItems());
}

void GameScene::Update(void)
{
#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugActionUp(INPUT_INFO::DEBUG_ACTION::CLEAR))
	{
		// ステージクリアにする
		SceneManager::GetInstance()->TrueStageClear();
	}

	if (InputManager::GetInstance()->IsDebugActionUp(INPUT_INFO::DEBUG_ACTION::OVER))
	{
		// ゲームオーバーにする
		SceneManager::GetInstance()->TrueGameOver();
	}

#endif // _DEBUG

	// オブジェクトの更新
	objectManger_->Update();

	enemyManager_->Update();

	CheckEnemyAttack();
	CollisionEnemy2Player();
	CollisionEnemy2PlayerGrab();
	CollisionEnemyToStage();

	// Effekseerにより再生中のエフェクトを更新する
	UpdateEffekseer3D();

	// スコアマネージャーの更新
	ScoreManager::GetInstance()->Update();

	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::PAUSE))
	{
		// ポーズ画面を開いたサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_ON);
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return;
	}

	if (SceneManager::GetInstance()->GetIsClear())
	{
		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		// ゲームクリアシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameClear>(),CLEAR);
		return;
	}

	if (SceneManager::GetInstance()->GetIsOver())
	{
		// トータルスコアを初期化
		ScoreManager::GetInstance()->ResetTotalPrice();
		// HP情報のみ初期化(リトライ時に自己強化した項目は残しておきたいため)
		PlayerStatusManager::GetInstance()->ResetHP();
		// ゲームオーバーシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<GameOver>(),OVER);
		return;
	}

	if (SceneManager::GetInstance()->GetIsStageClear())
	{
		// 納品した文の金額をショップで使える金額に加算
		ScoreManager::GetInstance()->AddTotalPrice(ScoreManager::GetInstance()->GetDeliveryPrice());
		// ステージクリアシーンへ
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<StageClear>(),STAGE_CLEAR);
		return;
	}

}

void GameScene::Draw(void)
{
	// オブジェクトの描画前
	objectManger_->PreDraw();

	enemyManager_->Draw();

	// オブジェクトの3D描画
	objectManger_->Draw3D();

	// Effekseerにより再生中のエフェクトを描画する
	DrawEffekseer3D();

	// オブジェクトの2D描画
	objectManger_->Draw2D();

	// 納品金額 / 目標金額の描画
	ScoreManager::GetInstance()->Draw();
}

void GameScene::Release(void)
{
	// オブジェクトマネージャー削除
	delete objectManger_;

	// 敵の解放
	enemyManager_->Release();
	delete enemyManager_;
	enemyManager_ = nullptr;

	// エフェクト管理解放
	EffectResManager::GetInstance().Destroy();

	// 音の解放
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
}

void GameScene::CameraCreate(void)
{
	// カメラ生成
	auto cameraObj = objectManger_->CreateObject();

	// タグの付与
	cameraObj->SetTagAndPriority(Tag::Camera);

	// 座標の設定
	auto cameraTrans = cameraObj->AddComponent<Transform>();
	cameraTrans->pos_ = { 0.0f,0.0f,0.0f };

	// ステージ情報を取ってきて初期化処理を行う
	auto stageNum = SceneManager::GetInstance()->GetCurrentStage();
	switch (stageNum)
	{
	case STAGE_1:
		cameraTrans->angle_ = { 0.0f, 90.0f * DX_PI_F / 180.0f, 0.0f };
		break;
	case STAGE_2:
		cameraTrans->angle_ = { 0.0f,0.0f,0.0f };
		break;
	case STAGE_3:
		cameraTrans->angle_ = { 0.0f, 90.0f * DX_PI_F / 180.0f, 0.0f };
		break;
	default:
		break;
	}

	// カメラコンポーネントの付与
	auto camera = cameraObj->AddComponent<Camera>();

	// カメラのモードを変更
	camera->ChangeMode(Camera::MODE::FOLLOW);
}

void GameScene::StageCreate(std::string path, std::string collPath)
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
	render->SetModel(path);

	// 描画
	stage->AddComponent<Animation>();

	// ステージ機能
	auto stageCom = stage->AddComponent<Stage>();
	stageCom->SetCollModel(collPath);

	// 納品場所の当たり判定追加
	auto delivery = stage->AddComponent<DeliveryLocationCollider>();

	// クロスヘアの取得
	auto crosshair = objectManger_->FindComponentWithTag<Crosshair>(Tag::Crosshair);
	delivery->SetCrosshair(crosshair);
}

void GameScene::WispCreate(void)
{
	// 火生成
	auto wisp = objectManger_->CreateObject();

	// タグを付与
	wisp->SetTagAndPriority(Tag::Wisp);

	// 座標の設定
	auto trans = wisp->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };
	trans->angle_ = { 0.0f,0.0f,0.0f };

	// 描画
	auto render = wisp->AddComponent<Render3D>();
	render->SetModel("Data/Model/Player/Light/Wisp.mv1");

	// 火機能
	auto cont = wisp->AddComponent<Wisp>();
}

void GameScene::PlayerCreate(void)
{
	// プレイヤー生成
	auto player = objectManger_->CreateObject();

	// タグを付与
	player->SetTagAndPriority(Tag::Player);

	// 座標の設定
	auto trans = player->AddComponent<Transform>();
	trans->pos_ = { 0.0f,100.0f,0.0f };

	// 移動の設定
	auto cont = player->AddComponent<PlayerController>();

	// カメラの取得
	auto camera = objectManger_->FindComponentWithTag<Camera>(Tag::Camera);
	// プレイヤーの情報をカメラに設定
	camera->SetTarget(trans);
	camera->SetPlayerController(cont);

	// 当たり判定の設定
	auto col = player->AddComponent<CapsuleCollider>();

	// カプセルの当たり判定を登録
	col->AddCapsule(
		PlayerController::STANDING_CAP_START_OFFSET,
		PlayerController::STANDING_CAP_END_OFFSET,
		40.0f
	);

	// ステージの取得
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);

	// ステージの当たり判定
	auto stageCol = player->AddComponent<StageCollider>();
	stageCol->SetStage(stage);
	stageCol->SetFloorNormalY(0.85f);
	stageCol->SetWallNormalY(0.20f);
	stageCol->SetSlopeNormalY(0.65f);

	stageCol->SetStepHeight(25.0f);

	// プレイヤー取得
	auto playerController = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);

	// ライト取得
	auto wisp = objectManger_->FindComponentWithTag<Wisp>(Tag::Wisp);

	// ライトのポインタを渡す
	playerController->SetWisp(wisp);

	// 納品場所用当たり判定にプレイヤーを渡す
	auto deliveryCol = stage->GetOwner()->GetComponent<DeliveryLocationCollider>();
	deliveryCol->SetPlayer(playerController);

	// マップの設定
	auto map = player->AddComponent<Map>();
	map->SetOwner(player);

	// フラッシュエフェクトの設定
	auto effect = player->AddComponent<FlashEffect>();
	effect->SetOwner(player);
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

void GameScene::CartCreate(void)
{
	// カートの作成
	auto cart = objectManger_->CreateObject();

	// タグを付与
	cart->SetTagAndPriority(Tag::Cart);

	// 座標の設定
	auto trans = cart->AddComponent<Transform>();

	// ステージ情報を取ってきて初期化処理を行う
	auto stageNum = SceneManager::GetInstance()->GetCurrentStage();
	switch (stageNum)
	{
	case STAGE_1:
		trans->pos_ = { 500.0f,30.0f,0.0f };
		trans->angle_ = { 0.0f,90.0f * (DX_PI_F / 180.0f),0.0f };
		break;
	case STAGE_2:
		trans->pos_ = { 0.0f,30.0f,500.0f };
		trans->angle_ = { 0.0f,0.0f,0.0f };
		break;
	case STAGE_3:

		trans->pos_ = { 500.0f,30.0f,0.0f };
		trans->angle_ = { 0.0f,90.0f * (DX_PI_F / 180.0f),0.0f };

		break;
	default:
		break;
	}

	// 描画
	auto render = cart->AddComponent<Render3D>();
	render->SetModel("Data/Model/Cart/Cart.mv1");

	// カプセルコライダー
	auto capsule = cart->AddComponent<CapsuleCollider>();

	const float HEIGHT = 70.0f;
	const float BOTTOM = 30.0f;

	const float WIDTH = 35.0f;
	const float DEPTH = 50.0f;
	const float RADIUS = 60.0f;

	// 左列
	capsule->AddCapsule(VGet(-WIDTH, HEIGHT, -DEPTH), VGet(-WIDTH, BOTTOM, -DEPTH), RADIUS);
	capsule->AddCapsule(VGet(-WIDTH, HEIGHT, DEPTH), VGet(-WIDTH, BOTTOM, DEPTH), RADIUS);

	// 右列
	capsule->AddCapsule(VGet(WIDTH, HEIGHT, -DEPTH), VGet(WIDTH, BOTTOM, -DEPTH), RADIUS);
	capsule->AddCapsule(VGet(WIDTH, HEIGHT, DEPTH), VGet(WIDTH, BOTTOM, DEPTH), RADIUS);

	// ステージコライダー
	auto stageCol = cart->AddComponent<StageCollider>();
	stageCol->SetFloorNormalY(0.90f);
	stageCol->SetWallNormalY(0.75f);
	stageCol->SetSlopeNormalY(0.85f);

	stageCol->SetStepHeight(5.0f);

	// ステージ取得
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	stageCol->SetStage(stage);

	// カート機能
	cart->AddComponent<Cart>();

	// カートの当たり判定追加
	auto cartColl = cart->AddComponent<CartCollider>();

	// ステージの取得
	cartColl->SetStage(stage);

	// プレイヤーの取得
	auto player = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);
	cartColl->SetPlayer(player);

	// クロスヘアの取得
	auto crosshair = objectManger_->FindComponentWithTag<Crosshair>(Tag::Crosshair);
	cartColl->SetCrosshair(crosshair);
}

void GameScene::ItemCreateStage1(void)
{
	ItemCreate(Tag::Item_Potion_Green, { 1960.0f,20.0f,428.0f });
	ItemCreate(Tag::Item_Bottle, { 2103.0f,186.0f,1263.0f });
	ItemCreate(Tag::Item_Mug, { 1957.0f,184.0f,4480.0f });
	ItemCreate(Tag::Item_Potion_Blue, { -362.0f,105.0f,4330.0f });
	ItemCreate(Tag::Item_Goblet, { 2161.0f,9.0f,9648.0f });
	ItemCreate(Tag::Item_Potion_Blue, { 2232.0f,452.0f,5346.0f });
	ItemCreate(Tag::Item_Potion_Red, { -3111.0f,19.0f,4386.0f });
	ItemCreate(Tag::Item_Amphora, { -3704.0f,18.0f,4724.0f });
	ItemCreate(Tag::Item_Bottle, { -2421.0f,173.0f,2476.0f });
	ItemCreate(Tag::Item_Jar, { -1196.0f,162.0f,5059.0f });
	ItemCreate(Tag::Item_Potion_Green, { -1952.0f,4.0f,7835.0f });
	ItemCreate(Tag::Item_Amphora, { -3717.0f,14.0f,6319.0f });
	ItemCreate(Tag::Item_Potion_Red, { -3563.0f,250.0f,7697.0f });
	ItemCreate(Tag::Item_Goblet, { -5775,154.0f,7690.0f });
	ItemCreate(Tag::Item_Skull, { -5578.0f,200.0f,7785.0f });
}

void GameScene::ItemCreateStage2(void)
{
	ItemCreate(Tag::Item_Potion_Green, { -689.0f,375.0f,672.0f });
	ItemCreate(Tag::Item_Potion_Red, { 567.0f,350.0f,1037.0f });
	ItemCreate(Tag::Item_Amphora, { -586.0f,10.0f,2592.0f });
	ItemCreate(Tag::Item_Bottle, { 192.0f,160.0f,3260.0f });
	ItemCreate(Tag::Item_Mug, { -2346.0f,182.0f,1348.0f });
	ItemCreate(Tag::Item_Amphora, { -7419.0f,417.0f,5645.0f });
	ItemCreate(Tag::Item_Potion_Blue, { -5949.0f,417.0f,5841.0f });
	ItemCreate(Tag::Item_Goblet, { -8108.0f,9.0f,1416.0f });
	ItemCreate(Tag::Item_Jar, { -7312.0f,9.0f,4863.0f });
	ItemCreate(Tag::Item_Skull, { -7478.0f,9.0f,3568.0f });
}

void GameScene::ItemCreateStage3(void)
{
	ItemCreate(Tag::Item_Potion_Red, { 1810.0f,14.0f,-669.0f });
	ItemCreate(Tag::Item_Goblet, { 2078.0f,182.0f,268.0f });
	ItemCreate(Tag::Item_Bottle, { 4945.0f,10.0f,-672.0f });
	ItemCreate(Tag::Item_Potion_Green, { 3289.0f,17.0f,744.0f });
	ItemCreate(Tag::Item_Goblet, { 5194.0f,163.0f,855.0f });
	ItemCreate(Tag::Item_Amphora, { 5949.0f,11.0f,8.0f });
	ItemCreate(Tag::Item_Mug, { 6442.0f,19.0f,278.0f });
	ItemCreate(Tag::Item_Jar, { 7525.0f,200.0f,935.0f });
	ItemCreate(Tag::Item_Mug, { 5226.0f,196.0f,1183.0f });
	ItemCreate(Tag::Item_Potion_Blue, { 6514.0f,22.0f,-695.0f });
	ItemCreate(Tag::Item_Potion_Blue, { 3959.0f,452.0f,1992.0f });
	ItemCreate(Tag::Item_Potion_Red, { 3309.0f,453.0f,1148.0f });
	ItemCreate(Tag::Item_Potion_Green, { 3290.0f,160.0f,2353.0f });
	ItemCreate(Tag::Item_Jar, { 3371.0f,453.0f,3498.0f });
	ItemCreate(Tag::Item_Bottle, { 4207.0f,157.0f,4189.0f });
	ItemCreate(Tag::Item_Goblet, { 4802.0f,526.0f,4581.0f });
	ItemCreate(Tag::Item_Amphora, { 5087.0f,213.0f,8005.0f });
	ItemCreate(Tag::Item_Potion_Green, { 6070.0f,17.0f,7362.0f });
	ItemCreate(Tag::Item_Potion_Blue, { 5076.0f,20.0f,7012.0f });
	ItemCreate(Tag::Item_Potion_Blue, { -1175.0f,11.0f,5550.0f });
	ItemCreate(Tag::Item_Goblet, { 254.0f,306.0f,5042.0f });
	ItemCreate(Tag::Item_Skull, { -82.0f,10.0f,4499.0f });
	ItemCreate(Tag::Item_Skull, { 5023.0f,167.0f,2540.0f });
}

void GameScene::CrosshairCreate(void)
{
	// クロスヘアの作成
	auto crosshair = objectManger_->CreateObject();

	// タグを付与
	crosshair->SetTagAndPriority(Tag::Crosshair);

	// クロスヘア付ける
	crosshair->AddComponent<Crosshair>();
}

void GameScene::Stage1Init(void)
{
	// カメラの作成
	CameraCreate();

	// カメラユーティリティにカメラのポインタを渡す
	CameraUtility::SetCameraPoint(objectManger_->FindComponentWithTag<Camera>(Tag::Camera));

	// ステージの作成
	StageCreate("Data/Model/Stage/Stage.mv1", "Data/Model/Stage/StageColl.mv1");

	// ライトの作成
	WispCreate();

	// プレイヤーの作成
	PlayerCreate();

	// 敵の作成
	EnemyCreate();

	// 敵作成
	enemyManager_ = new EnemyManager();
	enemyManager_->LoadStage1();

	// カートの作成
	CartCreate();

	// アイテムの作成
	ItemCreateStage1();
}

void GameScene::Stage2Init(void)
{
	// カメラの作成
	CameraCreate();

	// カメラユーティリティにカメラのポインタを渡す
	CameraUtility::SetCameraPoint(objectManger_->FindComponentWithTag<Camera>(Tag::Camera));

	// ステージの作成
	StageCreate("Data/Model/Stage/Stage2.mv1", "Data/Model/Stage/Stage2Coll.mv1");

	// ライトの作成
	WispCreate();

	// プレイヤーの作成
	PlayerCreate();

	// 敵の作成
	EnemyCreate();

	// 敵作成
	enemyManager_ = new EnemyManager();
	enemyManager_->LoadStage2();

	// カートの作成
	CartCreate();

	// アイテムの作成
	ItemCreateStage2();
}

void GameScene::Stage3Init(void)
{
	// カメラの作成
	CameraCreate();

	// カメラユーティリティにカメラのポインタを渡す
	CameraUtility::SetCameraPoint(objectManger_->FindComponentWithTag<Camera>(Tag::Camera));

	// ステージの作成
	StageCreate("Data/Model/Stage/Stage3.mv1", "Data/Model/Stage/Stage3Coll.mv1");

	// ライトの作成
	WispCreate();

	// プレイヤーの作成
	PlayerCreate();

	// 敵の作成
	EnemyCreate();
	// 敵作成
	enemyManager_ = new EnemyManager();
	enemyManager_->LoadStage3();

	// カートの作成
	CartCreate();

	// アイテムの作成
	ItemCreateStage3();
}

void GameScene::CheckEnemyAttack(void)
{
	// プレイヤーと敵の攻撃の当たり判定
	auto player = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);
	VECTOR startPos = player->GetOwner()->GetComponent<CapsuleCollider>()->GetStart();
	VECTOR endPos = player->GetOwner()->GetComponent<CapsuleCollider>()->GetEnd();
	float radius = player->GetOwner()->GetComponent<CapsuleCollider>()->GetRadius();

	for (auto enemy : enemyManager_->GetEnemys())
	{
		// 武器の情報
		WeaponBase* useWeapon = enemy->GetUseWeapon();
		if (useWeapon == nullptr) continue;

		// 攻撃中（描画されている）なら
		if (useWeapon->IsAlive())
		{
			// 無敵時間が
			if (player->GetInvincibleTime() > 0)continue;

			auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
			int stageModelId = stage->GetCollModelId() == -1 ? stage->GetModelId() : stage->GetCollModelId();
			// 敵の攻撃とステージのの当たり判定
			MV1_COLL_RESULT_POLY_DIM hits = MV1CollCheck_Sphere
			(
				stageModelId,
				-1,
				useWeapon->GetPos(),
				useWeapon->GetCollisionRadius()
			);

			// 当たっているなら描画をやめる
			if (hits.HitNum > 0)
			{
				useWeapon->SetAlive(false);
			}
			MV1CollResultPolyDimTerminate(hits);

			if (Collision::HitSphereCapsule(useWeapon->GetPos(), useWeapon->GetCollisionRadius(),
				startPos, endPos, radius))
			{
				// プレイヤーにダメージを与える
				player->SetDamage(static_cast<int>(enemy->GetAttackDamagePow()));

				VECTOR moveDir = VNorm(VSub(startPos, useWeapon->GetPos()));
				moveDir.y = 0.0f;
				player->SetHitReact(moveDir, enemy->GetAttackMoveSpeed(), enemy->GetAttackJumpPow());
				useWeapon->SetAlive(false);
			}
		}
	}
}

void GameScene::CollisionEnemyToStage(void)
{
	// 必要なコンポーネントが存在しないなら処理しない
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	if (!stage) return;

	// 1フレーム中に複数回衝突する場合に対応するための最大反復回数
	const int MAX_BOUNCE = 5;

	// めり込み防止用の押し出し量
	const float SKIN = 1.0f;

	// 登れる最大段差
	const float STEP_HEIGHT = 15.0f;

	// 各種判定用のしきい値
	const float FLOOR_NORMAL_Y = 0.866f;
	const float SLOPE_NORMAL_Y = 0.5f;
	const float WALL_NORMAL_Y = 0.1f;

	int stageModelId = stage->GetCollModelId() == -1 ? stage->GetModelId() : stage->GetCollModelId();

	for (auto enemy : enemyManager_->GetEnemys())
	{
		// 敵の回転行列を取得
		MATRIX mat = MGetRotY(enemy->GetAngle().y);

		// 現在座標
		VECTOR currentPos = enemy->GetPos();

		// 前フレーム座標
		VECTOR prevPos = enemy->GetPrevPos();

		// 今フレーム移動量
		VECTOR move = VSub(currentPos, prevPos);

		// 衝突判定開始座標
		VECTOR pos = prevPos;

		// 毎フレーム初期化
		enemy->SetGround(false);

		// 衝突とスライドを繰り返す
		for (int bounce = 0; bounce < MAX_BOUNCE; bounce++)
		{
			// 残り移動量の長さ
			float length = VSize(move);

			// ほぼ移動していないなら終了
			if (length < 0.01f)
				break;

			float radius = enemy->GetRadius();

			// 高速移動時のすり抜け防止のため、
			// 移動経路を細かく分割して判定する
			int stepCount = (int)(length / (radius * 0.1f)) + 1;

			// 分割数の上限・下限を設定
			stepCount = std::clamp(stepCount, 1, 64);

			// 1ステップ当たりの移動量
			VECTOR stepMove = VScale(move, 1.0f / stepCount);

			// 衝突情報
			bool hit = false;
			VECTOR hitNormal = VGet(0, 0, 0);

			// 衝突していない最後の座標
			VECTOR safePos = pos;

			// 衝突したステップ番号
			int hitStep = stepCount;

			// 経路を少しずつ進めながら判定
			for (int step = 0; step < stepCount; step++)
			{
				// 次に移動する座標
				VECTOR nextPos = VAdd(safePos, stepMove);

				VECTOR normalSum = VGet(0.0f, 0.0f, 0.0f);
				float totalWeight = 0.0f;
				bool collision = false;

				// カプセル始点・終点を算出
				VECTOR capStart = VAdd(nextPos, VTransform(enemy->GetStart(), mat));
				VECTOR capEnd = VAdd(nextPos, VTransform(enemy->GetEnd(), mat));

				// ステージとカプセルの衝突判定
				auto result =
					MV1CollCheck_Capsule(
						stageModelId,
						-1,
						capStart,
						capEnd,
						radius);

				// ヒットしたポリゴンを調べる
				for (int i = 0; i < result.HitNum; i++)
				{
					auto& poly = result.Dim[i];

					// ポリゴン法線
					VECTOR normal = VNorm(poly.Normal);

					if (normal.y >= FLOOR_NORMAL_Y)
					{
						normal = VGet(0.0f, 1.0f, 0.0f);
					}

					// 現在の移動方向と法線の向きから
					// 正面衝突している度合いを求める
					float push = -VDot(VNorm(move), normal);

					// 背面や平行な面は無視
					if (push <= 0.0f)
						continue;

					normalSum = VAdd(normalSum, VScale(normal, push));
					totalWeight += push;

					collision = true;

					// 敵の種類がスケルトンじゃなかったら処理をしない
					if (enemy->GetTag() != ENEMY_TAG::SKELETON) continue;
					enemy->SetCollisionStage(collision);
				}

				// 衝突結果を解放
				MV1CollResultPolyDimTerminate(result);

				// 衝突したら探索終了
				if (collision && totalWeight > 0.0f)
				{
					hit = true;
					hitNormal = VNorm(VScale(normalSum, 1.0f / totalWeight));
					hitStep = step;
					break;
				}

				// この位置までは移動しても大丈夫
				safePos = nextPos;
			}

			// 最後まで衝突しなかった
			if (!hit)
			{
				pos = VAdd(pos, move);
				break;
			}

			// 壁に衝突した場合は段差として登れるか確認する(y成分が小さい法線は壁として扱う)
			if (!enemy->GetGround() && hitNormal.y < FLOOR_NORMAL_Y)
			{
				// 段差判定
				if (CanStepUp(enemy, safePos, stepMove, STEP_HEIGHT))
				{
					// 衝突していない最後の位置へ戻す
					pos = safePos;

					// 段差の高さ分だけ上へ移動（階段を1段上がるイメージ）
					pos.y += STEP_HEIGHT;

					// 今回消費した移動量を残り移動量から除外
					VECTOR consumed = VSub(pos, prevPos);
					move = VSub(move, consumed);

					// 次のループで残り移動を処理する
					continue;
				}
			}

			// 衝突していない最後の座標へ戻す
			pos = safePos;

			// 少しだけ法線の方向へ押し出して
			// めり込みを防止する
			if (hitNormal.y >= FLOOR_NORMAL_Y)
			{
				pos.y += SKIN;
			}
			else
			{
				pos = VAdd(pos, VScale(hitNormal, SKIN));
			}

			float velocityY = enemy->GetVelocity();

			// 床判定
			if (hitNormal.y >= SLOPE_NORMAL_Y)
			{
				// 接地フラグを立てる
				enemy->SetGround(true);

				// 落下速度を停止
				if (velocityY < 0.0f)
				{
					velocityY = 0.0f;
				}
			}

			// 天井判定
			if (hitNormal.y < -0.6f)
			{
				// 上昇速度を停止
				if (velocityY > 0.0f)
				{
					velocityY = 0.0f;
				}
			}
			enemy->SetVelocity(velocityY);

			// 衝突後に残っている移動割合
			float remainRatio = (float)(stepCount - hitStep) / stepCount;

			VECTOR remainMove = VScale(move, remainRatio);

			// 壁スライド処理
			// 法線方向成分を除去して壁に沿って移動させる
			// 坂の場合
			if (hitNormal.y >= WALL_NORMAL_Y && hitNormal.y < FLOOR_NORMAL_Y)
			{
				// 坂方向へ移動できるようにする
				VECTOR slopeMove = remainMove;

				// 法線方向の押し込みだけ削除
				float dot = VDot(slopeMove, hitNormal);

				if (dot < 0.0f)
				{
					slopeMove = VSub(
						slopeMove,
						VScale(hitNormal, dot)
					);
				}

				remainMove = slopeMove;
			}
			// 壁の場合
			else
			{
				float dot = VDot(remainMove, hitNormal);

				if (dot < 0.0f)
				{
					remainMove = VSub(
						remainMove,
						VScale(hitNormal, dot)
					);
				}
			}

			// 次の反復で残り移動量を処理
			move = remainMove;
		}

		// 最終位置を反映
		enemy->SetPos(pos);
	}
}

// 小さな段差を登れるか判定する
bool GameScene::CanStepUp(EnemyBase* enemy, const VECTOR& pos, const VECTOR& move, float stepHeight)
{
	// 必要なコンポーネントが存在しないなら処理しない
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	if (!stage) return false;

	// 各種判定用のしきい値
	const float FLOOR_NORMAL_Y = 0.866f;
	const float WALL_NORMAL_Y = 0.1f;

	// 当たり判定専用モデルがあればそちらを使用する
	int stageModelId = stage->GetCollModelId() == -1 ? stage->GetModelId() : stage->GetCollModelId();

	// カプセルのオフセットをワールド座標へ変換するために使用
	MATRIX mat = MGetRotY(enemy->GetAngle().y);

	// テスト用の座標
	VECTOR testPos = pos;

	// 段差の高さ分だけ上へ持ち上げる(階段の1段上に乗れるか確認するため）
	testPos.y += stepHeight;

	const int FORWARD_STEP = 8;

	for (int s = 1; s <= FORWARD_STEP; s++)
	{
		float rate = (float)s / FORWARD_STEP;

		VECTOR checkPos = testPos;
		checkPos = VAdd(checkPos, VScale(move, rate));

		bool hitWall = false;

		// カプセル位置の算出（回転行列を適用）
		VECTOR start = VAdd(checkPos, VTransform(enemy->GetStart(), mat));
		VECTOR end = VAdd(checkPos, VTransform(enemy->GetEnd(), mat));

		auto result = MV1CollCheck_Capsule(
			stageModelId,
			-1,
			start,
			end,
			enemy->GetRadius());

		for (int i = 0; i < result.HitNum; i++)
		{
			VECTOR normal = VNorm(result.Dim[i].Normal);

			// 壁だけを見る
			if (normal.y <= WALL_NORMAL_Y)
			{
				hitWall = true;
				break;
			}
		}

		MV1CollResultPolyDimTerminate(result);

		// 少しでも前へ進めないなら失敗
		if (hitWall)
			return false;
	}

	// 最後まで壁が無ければ
	testPos = VAdd(testPos, move);

	// 下へ少しずつ落として床を探す
	const float DROP_STEP = 1.0f;

	// 現在どれだけ下降したか
	float dropped = 0.0f;

	while (dropped <= stepHeight)
	{
		// 落下後の判定位置
		VECTOR dropPos = testPos;

		// 少しずつ下げる
		dropPos.y -= dropped;

		// カプセル位置の算出
		VECTOR start = VAdd(dropPos, VTransform(enemy->GetStart(), mat));
		VECTOR end = VAdd(dropPos, VTransform(enemy->GetEnd(), mat));

		// ステージとの衝突確認
		auto result = MV1CollCheck_Capsule(
			stageModelId,
			-1,
			start,
			end,
			enemy->GetRadius());

		// ヒットした面が床か確認
		for (int i = 0; i < result.HitNum; i++)
		{
			// ポリゴン法線
			VECTOR normal = VNorm(result.Dim[i].Normal);

			// Y方向の法線が大きければ床
			if (normal.y >= FLOOR_NORMAL_Y)
			{
				MV1CollResultPolyDimTerminate(result);
				return true;
			}
		}

		// 衝突結果解放
		MV1CollResultPolyDimTerminate(result);

		// 次の高さを調べる
		dropped += DROP_STEP;
	}

	// どの高さでも床を見つけられなかった
	return false;
}

void GameScene::CollisionEnemy2Player(void)
{
	// 押し出し量を計算
	auto player = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);
	VECTOR playerPos = player->GetTransform()->pos_;
	VECTOR playerTop = player->GetCapsule()->GetStart();
	float playerRad = player->GetCapsule()->GetRadius();

	for (auto enemy : enemyManager_->GetEnemys())
	{
		VECTOR enemyPos = enemy->GetPos();
		VECTOR enemyTop = VAdd(enemyPos, enemy->GetStart());
		float enemyRad = enemy->GetRadius();

		VECTOR pushVector = Collision::ExtrusionCollisionCapsule(playerPos, playerTop, playerRad, enemyPos, enemyTop, enemyRad);

		// カプセル1（プレイヤーなど）は足し算
		playerPos = VAdd(playerPos, pushVector);
		player->GetTransform()->pos_ = playerPos; 

		if (enemy->GetTag() == ENEMY_TAG::MUSHNUB && 
			(pushVector.x > 0.1f || pushVector.y > 0.1f || pushVector.z > 0.1f))
		{
			player->SetDamage(static_cast<int>(enemy->GetAttackDamagePow()));
		}

		// カプセル2（敵など）は引き算
		enemyPos = VSub(enemyPos, pushVector);
		enemy->SetPos(enemyPos);
	}
}

void GameScene::CollisionEnemy2PlayerGrab(void)
{
	auto player = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);
	// プレイヤーが何かを掴んでいる状態か無敵状態だったら処理を行わない
	if (player->GetGrabbingState() == GRABBING_STATE::IS_GRABBING
		|| player->GetInvincibleTime() > 0) return;

	for (auto enemy : enemyManager_->GetEnemys())
	{
		// 敵の種類がキノコじゃなかったら処理をしない
		if (enemy->GetTag() != ENEMY_TAG::MUSHNUB)continue;

		// 線分の上座標
		VECTOR lineStartPos = player->GetLineStartPos();

		// 線分の下座標
		VECTOR lineEndPos = player->GetLineEndPos();

		// 線分とカートモデル衝突判定
		MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(enemy->GetModelId(), -1, lineStartPos, lineEndPos);
		// 線分と当たっていないなら処理をしない
		if (!hitResult.HitFlag)continue;

		// カメラとアイテムに線分をつなげてステージに当たっているか
		// 線分とステージモデルの衝突判定
		auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
		MV1_COLL_RESULT_POLY stageHitResult =
			MV1CollCheck_Line(stage->GetModelId(), -1, lineStartPos, hitResult.HitPosition);

		// ステージに当たっていたら
		if (stageHitResult.HitFlag)continue;

		// クロスヘアの取得
		auto crosshair = objectManger_->FindComponentWithTag<Crosshair>(Tag::Crosshair);
		// 当たっている
		// クロスヘアの種類を掴めるに変更
		crosshair->ChangeCrosshair(CROSSHAIR_TYPE::CROSSHAIR_CAN_GRAB);

		// 掴もうとしていたら
		if (player->GetGrabbingState() == GRABBING_STATE::TRY_GRABBING)
		{
			// プレイヤーにダメージを与える
			player->SetDamage(static_cast<int>(enemy->GetAttackDamagePow()));

			// クロスヘアの種類を掴めないに変更
			crosshair->ChangeCrosshair(CROSSHAIR_TYPE::CROSSHAIR_NOT_GRAB);
		}
	}
}

void GameScene::ItemCreate(Tag tag, VECTOR pos)
{
	auto itemData = ItemTable::Table.find(tag);

	// データの中身が無ければ処理を行わない
	if (itemData == ItemTable::Table.end())return;

	// アイテムの作成
	auto item = objectManger_->CreateObject();

	// タグを付与
	item->SetTagAndPriority(tag);

	// 座標の設定
	auto trans = item->AddComponent<Transform>();
	trans->pos_ = pos;
	trans->pos_.y += itemData->second.posOffsetY;

	// 描画
	auto render = item->AddComponent<Render3D>();
	render->SetModel(itemData->second.path);

	Item* itemBase = nullptr;
	// アイテム機能
	switch (itemData->first)
	{
	case Tag::Item_Goblet:
		itemBase = item->AddComponent<Goblet>();
		break;
	case Tag::Item_Potion_Red:
		itemBase = item->AddComponent<Potion>();
		break;
	case Tag::Item_Potion_Green:
		itemBase = item->AddComponent<Potion>();
		break;
	case Tag::Item_Potion_Blue:
		itemBase = item->AddComponent<Potion>();
		break;
	case Tag::Item_Amphora:
		itemBase = item->AddComponent<Amphora>();
		break;
	case Tag::Item_Bottle:
		itemBase = item->AddComponent<Bottle>();
		break;
	case Tag::Item_Jar:
		itemBase = item->AddComponent<Jar>();
		break;
	case Tag::Item_Mug:
		itemBase = item->AddComponent<Mug>();
		break;
	case Tag::Item_Skull:
		itemBase = item->AddComponent<Skull>();
		break;
	default:
		break;
	}

	// プレイヤー取得
	auto playerController = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);

	// ステージ取得
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);

	// カート取得
	auto cart = objectManger_->FindComponentWithTag<Cart>(Tag::Cart);

	// クロスヘアの取得
	auto crosshair = objectManger_->FindComponentWithTag<Crosshair>(Tag::Crosshair);

	// アイテムの当たり判定
	auto itemCol = item->AddComponent<ItemCollider>();
	// プレイヤーを渡す
	itemCol->SetPlayer(playerController);
	// ステージを渡す
	itemCol->SetStage(stage);
	// カートを渡す
	itemCol->SetCart(cart);
	// クロスヘアを渡す
	itemCol->SetCrosshair(crosshair);

	// ステージにアイテムを渡す
	if (itemBase != nullptr)
	{
		stage->SetItem(itemBase);
	}
}
