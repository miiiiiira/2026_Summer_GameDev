#include <fstream>
#include <sstream>
#include <algorithm>

#include "../../Common/Manager/PlayerActionCounter/PlayerActionCounter.h"
#include "../../Application.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../../Common/Manager/Score/ScoreManager.h"
#include "../SceneManager.h"
#include "../GameClear/GameClear.h"
#include "../GameOver/GameOver.h"
#include "../Shop/ShopScene.h"
#include "../Pause/Pause.h"

#include "../../Object/Actor/Enemy/EnemyManager.h"
#include "../../Object/Actor/Enemy/EnemyBase.h"
#include "../../Object/Actor/Enemy/Weapon/WeaponBase.h"
#include "../../Common/Crosshair/Crosshair.h"
#include "../../Object/ObjectManager/ObjectManager.h"
#include "../../Object/Component/Component.h"

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
#include "../../Object/Component/Transform/Transform.h"
#include "../../Common/Transform/MatrixUtility.h"
#include "../../Common/CameraUtility/CameraUtility.h"

#include "../../Common/Collision/Collision.h"
#include "../../Object/Component/Collider/CartCollider/CartCollider.h"
#include "../../Common/MouseCursor/MouseCursor.h"

#include "TutorialScene.h"
#include "../MainMenu/MainMenu.h"
#include "../LightSelectScene/LightSelectScene.h"

TutorialScene::TutorialScene(void)
{
	// マウスの表示を消す
	MouseCursor::GetInstance().SetMouseDraw(false);
	crosshair_ = nullptr;

	// 状態の登録
	stateTable_[Tutorial::MOVE] = &TutorialScene::Move;
	stateTable_[Tutorial::JUMP] = &TutorialScene::Jump;
	stateTable_[Tutorial::DASH] = &TutorialScene::Dash;
	stateTable_[Tutorial::CROUCH] = &TutorialScene::Crouch;
	stateTable_[Tutorial::SLIDING] = &TutorialScene::Sliding;
	stateTable_[Tutorial::LIGHT] = &TutorialScene::Light;
	stateTable_[Tutorial::MAP] = &TutorialScene::OpenMap;
	stateTable_[Tutorial::GRAB] = &TutorialScene::Grab;
	stateTable_[Tutorial::RANGE] = &TutorialScene::Range;
	stateTable_[Tutorial::CART] = &TutorialScene::ItemInCart;
	stateTable_[Tutorial::DELIVER] = &TutorialScene::Deliver;
}

TutorialScene::~TutorialScene(void)
{
}

void TutorialScene::Init(void)
{
	// はじめを移動ステートとする
	currentState_ = Tutorial::MOVE;

	// 指定されたステートのカウントを初期化
	PlayerActionCounter::GetInstance()->ResetCounter(currentState_);

	// クリアカウント初期化
	clearStateEndCount_ = 0;

	// 確認項目クリアフラグを立てる
	isClearState_ = false;

	// シーンマネージャー側にステートと値を保持してもらう(他クラスで使いたい情報なため)
	SceneManager::GetInstance()->SetTutorialStateAndValue(currentState_, steps_[static_cast<int>(currentState_)].value);

	// プレイヤーの行動カウンタクラスの初期化
	PlayerActionCounter::GetInstance()->Init();

	// スコアの初期化
	ScoreManager::GetInstance().ResetGame();

	// オブジェクトマネージャー初期化
	objectManger_->Init();

	// クロスヘアの初期化処理
	crosshair_->Init();

	// BGM再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME_1);
}

void TutorialScene::Load(void)
{
	LoadCsvData();

	// サウンド読みこみ
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);

	// オブジェクトマネージャーの生成
	objectManger_ = new ObjectManager();

	// クロスヘアの作成
	crosshair_ = new Crosshair();
	crosshair_->Load();

	// チュートリアルに使用するオブジェクトの初期化処理
	TutorialCreate();

}

void TutorialScene::LoadEnd(void)
{
	Init();
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	// スコアマネージャーに空のベクターを渡す(チュートリアルの場合納品金額が1円になる)
	ScoreManager::GetInstance().SetItems(stage->GetItems());

	auto map = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player)->GetOwner()->GetComponent<Map>();
	// マップにアイテムたちを渡す
	map->SetItems(stage->GetItems());
}

void TutorialScene::Update(void)
{
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::CANCEL))
	{
		// ポーズ画面を開いたサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_ON);
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return;
	}

	// オブジェクトの更新
	objectManger_->Update();

	// クロスヘアの更新
	crosshair_->Update();

	// スコアマネージャーの更新
	ScoreManager::GetInstance().Update();

	// 確認項目がクリア判定になっていたら「Good job!」のための時間を取る
	if (isClearState_)
	{
		Clear();
	}
	else
	{
		// 確認項目が最大まで行っていたら処理を行わない
		if (currentState_ == Tutorial::STATE::MAX)return;

		if (stateTable_[currentState_])
		{
			(this->*stateTable_[currentState_])();
		}
	}

	//　ステップ10まで終わったら遷移する
	if (currentState_ == Tutorial::MAX)
	{
		// トータルスコアを初期化
		ScoreManager::GetInstance().ResetTotalPrice();

		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();

		// ライトセレクトシーンに飛ばす
		SceneManager::GetInstance()->NextChangeScene(std::make_shared<LightSelectScene>(),LIGHT_SELECT);
		return;
	}
}

void TutorialScene::Draw(void)
{
	// オブジェクトの描画前
	objectManger_->PreDraw();

	// オブジェクトの3D描画
	objectManger_->Draw3D();

	// クロスヘアの描画
	crosshair_->Draw();

	// オブジェクトの2D描画
	objectManger_->Draw2D();

	// 納品金額 / 目標金額の描画
	ScoreManager::GetInstance().Draw();

#ifdef _DEBUG
	if (isClearState_)
	{
		DrawStringToHandle(150, 500, "Good job!", 0xffffff, Application::GetInstance()->GetFont());
	}
	// CLEAR以外の時は、CSVから読み込む
	else
	{
		// MAXの時、確認項目は全て終了しているため処理を行わない
		if (currentState_ == Tutorial::STATE::MAX)return;

		int index = static_cast<int>(currentState_);

		if (index >= 0 && index < static_cast<int>(steps_.size()))
		{
			// steps_ から取得し、描画
			// テキスト表示
			DrawStringToHandle(150, 500, steps_[index].textEN.c_str(), 0xffffff, Application::GetInstance()->GetFont());
			DrawStringToHandle(150, 540, steps_[index].textJP.c_str(), 0xffffff, Application::GetInstance()->GetDefaultFont());
			// ステート表示
			DrawFormatStringToHandle(10, 230, 0xffffff, 
					Application::GetInstance()->GetFont(), "ステート：　%s", steps_[index].type.c_str());
		}
	}
	// パーセント表示
	DrawFormatStringToHandle(10, 250, 0xffffff,
		Application::GetInstance()->GetFont(), "パーセント：　%.2f ％", PlayerActionCounter::GetInstance()->GetCounter(currentState_));
	// ステップ表示  ステートが0から始まるため、+1で補正
	DrawFormatStringToHandle(10, 270, 0xffffff,
		Application::GetInstance()->GetFont(), "ステップ：　%d / %d", static_cast<int>(currentState_) + 1, static_cast<int>(Tutorial::STATE::MAX));
#endif //_DEBUG
}

void TutorialScene::Release(void)
{
	if (!steps_.empty())
	{
		steps_.clear();
	}

	// オブジェクトマネージャー削除
	delete objectManger_;

	crosshair_->Release();
	delete crosshair_;
	crosshair_ = nullptr;

	AudioManager::GetInstance()->DeleteSceneSound(LoadScene::GAME);

}

void TutorialScene::SetState(Tutorial::STATE newState)
{
	// 指定されたステートへ変更
	currentState_ = newState;

	// 指定されたステートのカウントを初期化
	PlayerActionCounter::GetInstance()->ResetCounter(currentState_);

	// クリアカウント初期化
	clearStateEndCount_ = 0;

	// 確認項目クリアフラグを立てる
	isClearState_ = true;
}

void TutorialScene::LoadCsvData(void)
{
	steps_.clear();

	// ファイルの読み込み
	std::ifstream ifs = std::ifstream("Data/Csv/TutorialData.csv");
	if (!ifs)
	{
#ifdef _DEBUG
		printf("ファイルが開けませんでした。");
#endif // _DEBUG
		return;	//	エラー発生
	}

	// ファイルを１行ずつ読み込む
	std::string line;// 1行の文字情報
	bool isHeader = true;

	while (getline(ifs, line))
	{
		if (isHeader)
		{
			isHeader = false;
			continue;
		}

		// １行をカンマ区切りで分割
		std::istringstream stream(line);
		std::string field;
		std::vector<std::string> strSplit; // 1行を1文字の動的配列に分割


		while (getline(stream, field, ','))
		{
			strSplit.push_back(field);
		}

		if (strSplit.size() != 5)
		{
			continue;
		}

		Tutorial::TutorialInfo data{};
		int index = 0;

		// ID
		data.id = std::stoi(strSplit[index++]);

		// タイプ
		data.type = strSplit[index++];

		// 値
		data.value = std::stof(strSplit[index++]);

		// テキスト
		data.textEN = strSplit[index++];
		data.textJP = strSplit[index++];

		steps_.push_back(data);
	}

	ifs.close();
}

void TutorialScene::Move(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugAction(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}
#endif //_DEBUG

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をジャンプにする
		SetState(Tutorial::STATE::JUMP);
		return;
	}

}

void TutorialScene::Jump(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をダッシュにする
		SetState(Tutorial::STATE::DASH);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Dash(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugAction(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をしゃがみにする
		SetState(Tutorial::STATE::CROUCH);
		return;
	}
#endif //_DEBUG

}

void TutorialScene::Crouch(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugAction(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をスライディングにする
		SetState(Tutorial::STATE::SLIDING);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Sliding(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をライトの動かし方にする
		SetState(Tutorial::STATE::LIGHT);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Light(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をマップの開き方にする
		SetState(Tutorial::STATE::MAP);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::OpenMap(void)
{
#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目を掴み方にする
		SetState(Tutorial::STATE::GRAB);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Grab(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をアイテムの動かし方にする
		SetState(Tutorial::STATE::RANGE);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Range(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をカートへの入れ方にする
		SetState(Tutorial::STATE::CART);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::ItemInCart(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目を納品の仕方にする
		SetState(Tutorial::STATE::DELIVER);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Deliver(void)
{

#ifdef _DEBUG
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::TUTORIAL))
	{
		PlayerActionCounter::GetInstance()->SetCounter(currentState_, steps_[static_cast<int>(currentState_)].value);
	}

	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をアイテムの動かし方にする
		SetState(Tutorial::STATE::MAX);
		return;
	}
#endif //_DEBUG
}

void TutorialScene::Clear(void)
{
	// 規定値に到達したら
	if (clearStateEndCount_ >= MAX_CLEAR_COUNT)
	{
		// クリア判定を切る
		isClearState_ = false;

		// シーンマネージャー側にステートと値を保持してもらう(他クラスで使いたい情報なため)
		SceneManager::GetInstance()->SetTutorialStateAndValue(currentState_, steps_[static_cast<int>(currentState_)].value);
		return;
	}

	// カウントを進める
	clearStateEndCount_++;
}

void TutorialScene::TutorialCreate(void)
{
	// カメラの作成
	CameraCreate();

	// カメラユーティリティにカメラのポインタを渡す
	CameraUtility::SetCameraPoint(objectManger_->FindComponentWithTag<Camera>(Tag::Camera));

	// ステージの作成
	StageCreate("Data/Model/Stage/TutorialStage.mv1");

	// ライトの作成
	WispCreate();

	// プレイヤーの作成
	PlayerCreate();

	// カートの作成
	CartCreate();

	// アイテムの作成
	ItemCreateTutorial();
}

void TutorialScene::CameraCreate(void)
{
	// カメラ生成
	auto cameraObj = objectManger_->CreateObject();

	// タグの付与
	cameraObj->SetTagAndPriority(Tag::Camera);

	// 座標の設定
	auto cameraTrans = cameraObj->AddComponent<Transform>();
	cameraTrans->pos_ = { 0.0f,0.0f,0.0f };
	cameraTrans->angle_ = { 0.0f, 0.0f, 0.0f };

	// カメラコンポーネントの付与
	auto camera = cameraObj->AddComponent<Camera>();

	// カメラのモードを変更
	camera->ChangeMode(Camera::MODE::FOLLOW);
}

void TutorialScene::StageCreate(std::string path, std::string collPath)
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
	delivery->SetCrosshair(crosshair_);
}

void TutorialScene::WispCreate(void)
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

void TutorialScene::PlayerCreate(void)
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
}

void TutorialScene::CartCreate(void)
{
	// カートの作成
	auto cart = objectManger_->CreateObject();

	// タグを付与
	cart->SetTagAndPriority(Tag::Cart);

	// 座標の設定
	auto trans = cart->AddComponent<Transform>();
	trans->pos_ = { -14.0f,10.0f,13440.0f };
	trans->angle_ = { 0.0f,0.0f,0.0f };

	// 描画
	auto render = cart->AddComponent<Render3D>();
	render->SetModel("Data/Model/Cart/Cart.mv1");

	// カプセルコライダー
	auto capsule = cart->AddComponent<CapsuleCollider>();

	const float HEIGHT = 80.0f;
	const float BOTTOM = 30.0f;

	const float WIDTH = 50.0f;
	const float DEPTH = 60.0f;
	const float RADIUS = 40.0f;

	// 左列
	capsule->AddCapsule(VGet(-WIDTH, HEIGHT, -DEPTH), VGet(-WIDTH, BOTTOM, -DEPTH), RADIUS);
	capsule->AddCapsule(VGet(-WIDTH, HEIGHT, 0.0f), VGet(-WIDTH, BOTTOM, 0.0f), RADIUS);
	capsule->AddCapsule(VGet(-WIDTH, HEIGHT, DEPTH), VGet(-WIDTH, BOTTOM, DEPTH), RADIUS);

	// 中央列
	capsule->AddCapsule(VGet(0.0f, HEIGHT, -DEPTH), VGet(0.0f, BOTTOM, -DEPTH), RADIUS);
	capsule->AddCapsule(VGet(0.0f, HEIGHT, DEPTH), VGet(0.0f, BOTTOM, DEPTH), RADIUS);

	// 右列
	capsule->AddCapsule(VGet(WIDTH, HEIGHT, -DEPTH), VGet(WIDTH, BOTTOM, -DEPTH), RADIUS);
	capsule->AddCapsule(VGet(WIDTH, HEIGHT, 0.0f), VGet(WIDTH, BOTTOM, 0.0f), RADIUS);
	capsule->AddCapsule(VGet(WIDTH, HEIGHT, DEPTH), VGet(WIDTH, BOTTOM, DEPTH), RADIUS);

	// ステージコライダー
	auto stageCol = cart->AddComponent<StageCollider>();
	stageCol->SetFloorNormalY(0.85f);
	stageCol->SetWallNormalY(0.20f);
	stageCol->SetSlopeNormalY(0.65f);

	stageCol->SetStepHeight(25.0f);

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
	cartColl->SetCrosshair(crosshair_);
}

void TutorialScene::ItemCreateTutorial(void)
{
	// 掴みのアイテム
	ItemCreate(Tag::Item_Mug, { -284.0f,10.0f,10607.0f });
	ItemCreate(Tag::Item_Goblet, { 352.0f,10.0f,10617.0f });
	ItemCreate(Tag::Item_Potion_Green, { 439.0f,10.0f,9854.0f });
	ItemCreate(Tag::Item_Jar, { -403.0f,10.0f,9873.0f });

	// レンジのアイテム
	ItemCreate(Tag::Item_Potion_Red, { 338.0f,10.0f,12148.0f });
	ItemCreate(Tag::Item_Bottle, { -262.0f,10.0f,12136.0f });

	// カートのアイテム
	ItemCreate(Tag::Item_Skull, { 378.0f,10.0f,12824.0f });
	ItemCreate(Tag::Item_Potion_Blue, { 393.0f,10.0f,13579.0f });
	ItemCreate(Tag::Item_Amphora, { -368.0f,10.0f,13564.0f });
	ItemCreate(Tag::Item_Mug, { -434.0f,10.0f,12693.0f });
}

void TutorialScene::ItemCreate(Tag tag, VECTOR pos)
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

	// アイテムの当たり判定
	auto itemCol = item->AddComponent<ItemCollider>();
	// プレイヤーを渡す
	itemCol->SetPlayer(playerController);
	// ステージを渡す
	itemCol->SetStage(stage);
	// カートを渡す
	itemCol->SetCart(cart);
	// クロスヘアを渡す
	itemCol->SetCrosshair(crosshair_);

	// ステージにアイテムを渡す
	if (itemBase != nullptr)
	{
		stage->SetItem(itemBase);
	}
}