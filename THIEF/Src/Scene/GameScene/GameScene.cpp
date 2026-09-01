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
#include "../../Object/Actor/Enemy/EnemyBase.h"
#include "../../Object/Actor/Enemy/Yeti/Yeti.h"
#include "../../Object/Actor/Enemy/Skeleton/Skeleton.h"
#include "../../Object/Actor/Enemy/Giggle/Giggle.h"
#include "../../Object/Actor/Enemy/Mushnub/Mushnub.h"
#include "../../Object/Actor/Enemy/Statue/Statue.h"
#include "../../Object/Actor/Enemy/Weapon/WeaponBase.h"

#include "../../Object/ObjectManager/ObjectManager.h"
#include "../../Object/Component/Collider/3DCollider/CapsuleCollider.h"
#include "../../Object/Component/Collider/StageCollider/StageCollider.h"
#include "../../Object/Component/Collider/ItemCollider/ItemCollider.h"
#include "../../Object/Component/Collider/EnemyCollider/EnemyCollider.h"
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
#include "../../Common/Math/Math.h"
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
	stagePathData_.reset();

	// オブジェクトマネージャー削除
	delete objectManger_;

	// エフェクト管理解放
	EffectResManager::GetInstance().Destroy();

	// 音の解放
	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);
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
	EnemyCreateStage1();

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
	EnemyCreateStage2();

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
	EnemyCreateStage3();

	// カートの作成
	CartCreate();

	// アイテムの作成
	ItemCreateStage3();
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

void GameScene::EnemyCreateStage1(void)
{
	// ステージ生成時に1回だけパスデータを生成・ロードする
	InitPathData();

	EnemyCreate(ENEMY_TAG::GIGGLE, {0.0f, 0.0f, 0.0f});
	EnemyCreate(ENEMY_TAG::YETI, { -5617.04f,10.0f,6573.71f });

	EnemySpawnParam mushParam;
	mushParam.minAreaPos_ = { -2150.0f, 1.0f, 5400.0f };
	mushParam.maxAreaPos_ = { -560.0f, 700.0f, 7800.0f };
	mushParam.chasePos_ = { -1506.83f, 10.0f, 6513.62f };
	EnemyCreate(ENEMY_TAG::MUSHNUB, { -1526.83f, 10.0f, 5500.0f }, mushParam);
}

void GameScene::EnemyCreateStage2(void)
{
	// ステージ生成時に1回だけパスデータを生成・ロードする
	InitPathData();

	EnemyCreate(ENEMY_TAG::GIGGLE, { 0.0f, 0.0f, 0.0f });
	EnemyCreate(ENEMY_TAG::YETI, { -7887.0f, 10.0f, 1399.0f });

	EnemySpawnParam statueParam;
	statueParam.minAreaPos_ = { -7690.0f, 1.0f, 3450.0f };
	statueParam.maxAreaPos_ = { -5870.0f, 1110.0f, 5920.0f };
	statueParam.chasePos_ = { -7444.0f, 40.0f, 3570.0f };
	EnemyCreate(ENEMY_TAG::STATUE, { -7444.0f, 10.0f, 3570.0f }, statueParam);

	float startX = -4000.0f;
	float spacingX = 200.0f;
	float leftZ = 2197.0f;
	float rightZ = 680.0f;
	float posY = 12.0f;

	// 右側のスケルトン
	for (int i = 0; i < 8; ++i)
	{
		EnemySpawnParam param;
		VECTOR pos = { startX - (i * spacingX), posY, rightZ };

		float random = static_cast<float>(GetRand(360));
		param.angle_ = { 0.0f, Math::Deg2Rad(random), 0.0f };
		param.side_ = ENEMY_SIDE::RIGHT;

		EnemyCreate(ENEMY_TAG::SKELETON, pos, param);
	}

	// 左側のスケルトン
	for (int i = 0; i < 8; ++i)
	{
		EnemySpawnParam param;
		VECTOR pos = { startX - (i * spacingX), posY, leftZ };

		float random = static_cast<float>(GetRand(360));
		param.angle_ = { 0.0f, Math::Deg2Rad(random), 0.0f };
		param.side_ = ENEMY_SIDE::LEFT;

		EnemyCreate(ENEMY_TAG::SKELETON, pos, param);
	}

}

void GameScene::EnemyCreateStage3(void)
{
	// ステージ生成時に1回だけパスデータを生成・ロードする
	InitPathData();

	EnemyCreate(ENEMY_TAG::GIGGLE, { 0.0f, 0.0f, 0.0f });
	EnemyCreate(ENEMY_TAG::YETI, { 5704.0f, 10.0f, 7919.0f });

	EnemySpawnParam mush1Param;
	mush1Param.minAreaPos_ = { 6547.0f, 1.0f, -609.0f };
	mush1Param.maxAreaPos_ = { 7688.0f, 700.0f, 882.0f };
	mush1Param.chasePos_ = { 6996.0f, 10.0f, 623.0f };
	EnemyCreate(ENEMY_TAG::MUSHNUB, { 6996.0f, 10.0f, 623.0f }, mush1Param);

	EnemySpawnParam mush2Param;
	mush2Param.minAreaPos_ = { 3458.0f, 1.0f, 2207.0f };
	mush2Param.maxAreaPos_ = { 5081.0f, 700.0f, 3695.0f };
	mush2Param.chasePos_ = { 3606.0f, 10.0f, 2610.0f };
	EnemyCreate(ENEMY_TAG::MUSHNUB, { 3606.0f, 10.0f, 2610.0f }, mush2Param);

	EnemySpawnParam statueParam;
	statueParam.minAreaPos_ = { -1200.0f, 1.0f, 4450.0f };
	statueParam.maxAreaPos_ = { 250.0f, 700.0f, 5810.0f };
	statueParam.chasePos_ = { 45.0f, 40.0f, 4680.0f };
	EnemyCreate(ENEMY_TAG::STATUE, { 45.0f, 10.0f, 4680.0f }, statueParam);
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

void GameScene::EnemyCreate(ENEMY_TAG tag, VECTOR pos, const EnemySpawnParam& param)
{
	// テーブルから敵のデータを検索
	auto enemyData = EnemyTable::Table.find(tag);

	// データの中身が無ければ処理を行わない
	if (enemyData == EnemyTable::Table.end()) return;

	// 敵オブジェクトの生成
	auto enemyObj = objectManger_->CreateObject();

	// タグの付与
	enemyObj->SetTagAndPriority(Tag::Enemy);

	// 座標の設定
	auto trans = enemyObj->AddComponent<Transform>();
	trans->pos_ = pos;
	trans->angle_ = param.angle_;

	auto render = enemyObj->AddComponent<Render3D>();
	render->SetModel(enemyData->second.path);

	// コライダーの追加とテーブルデータからの設定
	auto capsule = enemyObj->AddComponent<CapsuleCollider>();
	capsule->AddCapsule(
		enemyData->second.capStartOffset,
		enemyData->second.capEndOffset,
		enemyData->second.capRadius
	);

	// ステージコライダー追加
	auto stageCol = enemyObj->AddComponent<StageCollider>();
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	stageCol->SetStage(stage);
	stageCol->SetFloorNormalY(0.85f);
	stageCol->SetWallNormalY(0.20f);
	stageCol->SetSlopeNormalY(0.65f);
	stageCol->SetStepHeight(15.0f);

	// プレイヤー取得
	auto playerController = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);
	// クロスヘアの取得
	auto crosshair = objectManger_->FindComponentWithTag<Crosshair>(Tag::Crosshair);

	// 敵の当たり判定
	auto enemyCol = enemyObj->AddComponent<EnemyCollider>();
	// プレイヤーを渡す
	enemyCol->SetPlayer(playerController);
	// ステージを渡す
	enemyCol->SetStage(stage);
	// クロスヘアを渡す
	enemyCol->SetCrosshair(crosshair);

	// 敵の種類に応じたコンポーネントの付与
	EnemyBase* enemyComp = nullptr;
	switch (tag)
	{
	case ENEMY_TAG::YETI:
		enemyComp = enemyObj->AddComponent<Yeti>();
		break;

	case ENEMY_TAG::MUSHNUB:
	{
		auto mushnub = enemyObj->AddComponent<Mushnub>();
		mushnub->SetAreaPos(param.minAreaPos_, param.maxAreaPos_);
		mushnub->SetChasePos(param.chasePos_);
		enemyComp = mushnub;
		break;
	}

	case ENEMY_TAG::SKELETON:
	{
		auto skeleton = enemyObj->AddComponent<Skeleton>();
		skeleton->SetSide(param.side_);
		enemyComp = skeleton;
		break;
	}

	case ENEMY_TAG::GIGGLE:
		enemyComp = enemyObj->AddComponent<Giggle>();
		break;

	case ENEMY_TAG::STATUE:
	{
		auto statue = enemyObj->AddComponent<Statue>();
		statue->SetAreaPos(param.minAreaPos_, param.maxAreaPos_);
		statue->SetChasePos(param.chasePos_);
		enemyComp = statue;
		break;
	}
	
	default:
		break;
	}

	if (enemyComp != nullptr)
	{
		// テーブルのデータをEnemyBaseに渡す
		enemyComp->SetEnemyData(enemyData->second);

		auto player = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);
		int stageModelId = (stage->GetCollModelId() == -1) ? stage->GetModelId() : stage->GetCollModelId();

		enemyComp->SetPathData(player, stageModelId, stagePathData_);
	}
}

void GameScene::InitPathData(void)
{
	if (stagePathData_)
	{
		stagePathData_.reset();
	}

	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	int stageModelId = (stage->GetCollModelId() == -1) ? stage->GetModelId() : stage->GetCollModelId();

	stagePathData_ = std::make_shared<StagePathData>(stageModelId);

	auto stageNum = SceneManager::GetInstance()->GetCurrentStage();
	switch (stageNum)
	{
	case STAGE_1: stagePathData_->Load("Data/PointSave.csv");  break;
	case STAGE_2: stagePathData_->Load("Data/PointSave2.csv"); break;
	case STAGE_3: stagePathData_->Load("Data/PointSave3.csv"); break;
	default: break;
	}
}
