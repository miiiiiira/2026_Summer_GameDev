#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

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
#include "../../Object/Component/Crosshair/Crosshair.h"
#include "../../Object/Component/Transform/Transform.h"
#include "../../Common/Transform/MatrixUtility.h"
#include "../../Common/CameraUtility/CameraUtility.h"

#include "../../Common/Collision/Collision.h"
#include "../../Object/Component/Collider/CartCollider/CartCollider.h"
#include "../../Common/MouseCursor/MouseCursor.h"

#include "../../Object/Actor/Wall/TutorialWall.h"

#include "TutorialScene.h"
#include "../MainMenu/MainMenu.h"
#include "../LightSelectScene/LightSelectScene.h"
#include "../../Object/Component/Collider/TutorialWallCollider/TutorialWallCollider.h"
#include "../../Common/Manager/EffectResManager/EffectResManager.h"
#include <EffekseerForDXLib.h>
#include "../../Common/Manager/Light/LightManager.h"

TutorialScene::TutorialScene(void)
{
	// マウスの表示を消す
	MouseCursor::GetInstance().SetMouseDraw(false);
	tutorialWall_ = nullptr;

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
	achievementRate_ = 0;

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

	tutorialWall_->Init();

	// BGM再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME_1);

	// ライトをデフォルト色に設定する
	LightManager::GetInstance().SetLightType(LIGHT_TYPE::COLOR_0);
}

void TutorialScene::Load(void)
{
	LoadCsvData();

	// エフェクト管理初期化
	EffectResManager::CreateInstance();

	// サウンド読みこみ
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);

	// オブジェクトマネージャーの生成
	objectManger_ = new ObjectManager();

	// クロスヘアの生成
	CrosshairCreate();

	WallCreate();

	// チュートリアルに使用するオブジェクトの初期化処理
	TutorialCreate();
}

void TutorialScene::LoadEnd(void)
{
	Init();

	// エフェクシアが非同期ロードに対応していないためここでロード
	EffectResManager::GetInstance().Load();

	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	// スコアマネージャーに空のベクターを渡す(チュートリアルの場合納品金額が1円になる)
	ScoreManager::GetInstance().SetItems(stage->GetItems());

	auto map = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player)->GetOwner()->GetComponent<Map>();
	// マップにアイテムたちを渡す
	map->SetItems(stage->GetItems());
}

void TutorialScene::Update(void)
{
	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::CANCEL) || InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::CANCEL))
	{
		// ポーズ画面を開いたサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_ON);
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return;
	}

	// オブジェクトの更新
	objectManger_->Update();

	tutorialWall_->Update(currentState_, isClearState_, clearStateEndCount_, MAX_CLEAR_COUNT);

	// Effekseerにより再生中のエフェクトを更新する
	UpdateEffekseer3D();

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

	UpdateAchievementRate();
}

void TutorialScene::Draw(void)
{
	// オブジェクトの描画前
	objectManger_->PreDraw();

	// オブジェクトの3D描画
	objectManger_->Draw3D();

	tutorialWall_->Draw();

	// Effekseerにより再生中のエフェクトを描画する
	DrawEffekseer3D();

	// オブジェクトの2D描画
	objectManger_->Draw2D();

	// 納品金額 / 目標金額の描画
	ScoreManager::GetInstance().Draw();

#ifdef _DEBUG
#endif //_DEBUG

	// テキスト背景用のボックス
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
	DrawBox(0, 520, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// 達成率バー
	DrawBox(0, 540, achievementRate_, 545, 0xffc800, true);

	if (isClearState_)
	{
		DrawStringToHandle(150, 570, "Good Job!", 0xffffff, Application::GetInstance()->GetFont(FONT_SIZE_20));
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
			std::string textEN = ConvertTutorialTagToKeyName(steps_[index].textEN);
			std::string textJP = ConvertTutorialTagToKeyName(steps_[index].textJP);

			// 置換後のテキストを描画
			DrawTutorialTextWithHighlight(150, 570, steps_[index].textEN, 0xffffff, 0xffc800, Application::GetInstance()->GetFont(FONT_SIZE_20));
			DrawTutorialTextWithHighlight(150, 600, steps_[index].textJP, 0xffffff, 0xffc800, Application::GetInstance()->GetDefaultFont());

			// ステート表示
			DrawFormatStringToHandle(10, 230, 0xffffff, 
					Application::GetInstance()->GetFont(FONT_SIZE_20), "STATE：　%s", steps_[index].type.c_str());
		}
	}


	int step = static_cast<int>(currentState_);
	// ステップ表示  ステートが0から始まるため、+1で補正
	if (static_cast<int>(currentState_) < static_cast<int>(Tutorial::STATE::MAX))
	{
		step++;
	}
	DrawFormatStringToHandle(10, 270, 0xffffff,
		Application::GetInstance()->GetFont(FONT_SIZE_20), "STEP：　%d / %d", step, static_cast<int>(Tutorial::STATE::MAX));
}

void TutorialScene::Release(void)
{
	if (!steps_.empty())
	{
		steps_.clear();
	}

	// オブジェクトマネージャー削除
	delete objectManger_;

	if (tutorialWall_ != nullptr)
	{
		tutorialWall_->Release();
		delete tutorialWall_;
		tutorialWall_ = nullptr;
	}

	// エフェクト管理解放
	EffectResManager::GetInstance().Destroy();

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
	achievementRate_ = 0;

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

void TutorialScene::UpdateAchievementRate(void)
{
	if (isClearState_)
	{
		float ratio = static_cast<float>(clearStateEndCount_) / static_cast<float>(MAX_CLEAR_COUNT);
		achievementRate_ = static_cast<int>(Application::SCREEN_SIZE_X * ratio);
	}
	else
	{
		float ratio = PlayerActionCounter::GetInstance()->GetCounter(currentState_) / 100.0f;
		achievementRate_ = static_cast<int>(Application::SCREEN_SIZE_X * ratio);
	}
}

void TutorialScene::Move(void)
{
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
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をダッシュにする
		SetState(Tutorial::STATE::DASH);
		return;
	}
}

void TutorialScene::Dash(void)
{
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をしゃがみにする
		SetState(Tutorial::STATE::CROUCH);
		return;
	}

}

void TutorialScene::Crouch(void)
{
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をスライディングにする
		SetState(Tutorial::STATE::SLIDING);
		return;
	}
}

void TutorialScene::Sliding(void)
{
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をライトの動かし方にする
		SetState(Tutorial::STATE::LIGHT);
		return;
	}
}

void TutorialScene::Light(void)
{
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をマップの開き方にする
		SetState(Tutorial::STATE::MAP);
		return;
	}
}

void TutorialScene::OpenMap(void)
{
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目を掴み方にする
		SetState(Tutorial::STATE::GRAB);
		return;
	}
}

void TutorialScene::Grab(void)
{
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をアイテムの動かし方にする
		SetState(Tutorial::STATE::RANGE);
		return;
	}
}

void TutorialScene::Range(void)
{
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をカートへの入れ方にする
		SetState(Tutorial::STATE::CART);
		return;
	}
}

void TutorialScene::ItemInCart(void)
{
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目を納品の仕方にする
		SetState(Tutorial::STATE::DELIVER);
		return;
	}
}

void TutorialScene::Deliver(void)
{
	// 100％に達したら、クリアに遷移
	if (PlayerActionCounter::GetInstance()->GetCounter(currentState_) >= MAX_VALUE)
	{
		// 次の項目をアイテムの動かし方にする
		SetState(Tutorial::STATE::MAX);
		return;
	}
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
	clearStateEndCount_+= 2;
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
	// クロスヘアの取得
	auto crosshair = objectManger_->FindComponentWithTag<Crosshair>(Tag::Crosshair);
	delivery->SetCrosshair(crosshair);
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

	// チュートリアルの壁の当たり判定
	player->AddComponent<TutorialWallCollider>();

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

	// クロスヘアの取得
	auto crosshair = objectManger_->FindComponentWithTag<Crosshair>(Tag::Crosshair);
	cartColl->SetCrosshair(crosshair);
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

void TutorialScene::CrosshairCreate(void)
{
	// クロスヘアの作成
	auto crosshair = objectManger_->CreateObject();

	// タグを付与
	crosshair->SetTagAndPriority(Tag::Crosshair);

	// クロスヘア付ける
	crosshair->AddComponent<Crosshair>();
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

void TutorialScene::WallCreate(void)
{
	tutorialWall_ = new TutorialWall();
	tutorialWall_->Load();

	// --- 各ステップの壁座標を登録 ---
	tutorialWall_->AddWall(
		VGet(0.0f, 0.0f, 780.0f),
		{ Tutorial::MOVE }
	);

	tutorialWall_->AddWall(
		VGet(0.0f, 0.0f, 2280.0f),
		{ Tutorial::JUMP }
	);

	tutorialWall_->AddWall(
		VGet(0.0f, 0.0f, 3750.0f),
		{ Tutorial::DASH }
	);

	tutorialWall_->AddWall(
		VGet(0.0f, 0.0f, 5180.0f),
		{ Tutorial::CROUCH }
	);

	tutorialWall_->AddWall(
		VGet(0.0f, 0.0f, 6680.0f),
		{ Tutorial::SLIDING }
	);

	tutorialWall_->AddWall(
		VGet(0.0f, 0.0f, 8130.0f),
		{ Tutorial::LIGHT }
	);

	tutorialWall_->AddWall(
		VGet(0.0f, 0.0f, 9600.0f),
		{ Tutorial::MAP }
	);

	tutorialWall_->AddWall(
		VGet(0.0f, 0.0f, 11070.0f),
		{ Tutorial::GRAB }
	);

	tutorialWall_->AddWall(
		VGet(0.0f, 0.0f, 12500.0f),
		{ Tutorial::RANGE }
	);
}

std::string TutorialScene::GetSlot1KeyName(INPUT_INFO::ACTION action)
{
	// InputManagerから全アクションのバインド情報を取得
	const auto& binds = InputManager::GetInstance()->GetActionBinds();
	auto it = binds.find(action);

	// アクションの登録情報が存在しないなら"---" を返す
	if (it == binds.end()) return "---";

	// 現在アクティブなデバイスを取得
	auto activeDevice = InputManager::GetInstance()->GetActiveDevice();

	// パッド
	if (activeDevice == InputManager::ActiveDevice::PAD)
	{
		const auto& bind = it->second.pad[0];	// 1スロット目

		// 未割り当てなら"---" を返す
		if (bind.code == -1) return "---";

		switch (bind.type)
		{
		case InputManager::BindType::PAD_BTN:
			return INPUT_INFO::PadBtnToString(static_cast<INPUT_INFO::PAD_BTN>(bind.code));

		case InputManager::BindType::PAD_DIR:
			return INPUT_INFO::PadDirToString(static_cast<INPUT_INFO::PAD_DIR>(bind.code));

		case InputManager::BindType::PAD_TRIGGER:
			return INPUT_INFO::PadTriggerToString(static_cast<INPUT_INFO::PAD_TRIGGER>(bind.code));

		case InputManager::BindType::PAD_STICK:
			return INPUT_INFO::PadStickToString(static_cast<INPUT_INFO::PAD_STICK>(bind.code));

		default:
			return "---";
		}
	}

	// キーボード・マウス
	const auto& bind = it->second.keyMouse[0]; // 1スロット目

	if (bind.code == -1) return "---";

	// 未割り当てなら"---" を返す
	if (bind.type == InputManager::BindType::KEY)
	{
		return INPUT_INFO::GetKeyNameFromScanCode(bind.code);
	}
	else if (bind.type == InputManager::BindType::MOUSE)
	{
		return INPUT_INFO::MouseToString(static_cast<INPUT_INFO::MouseBtn>(bind.code));
	}

	return "---";
}

std::string TutorialScene::ReplaceAll(std::string str, const std::string& from, const std::string& to)
{
	// 空ならそのまま返す
	if (from.empty()) return str;

	// 検索を開始する位置
	size_t startPos = 0;

	// 開始位置からfromが見つからなくなるまでループ
	while ((startPos = str.find(from, startPos)) != std::string::npos)
	{
		// 見つかった位置から長さ文をtoに置き換え
		str.replace(startPos, from.length(), to);

		// 次の検索位置を置換後の文字列の後ろに進める
		startPos += to.length();
	}
	return str;
}

std::string TutorialScene::ConvertTutorialTagToKeyName(std::string text)
{
	// --- 単一アクションとタグの対応表 ---
	static const std::unordered_map<std::string, INPUT_INFO::ACTION> singleTagMap = {
		{ "%JUMP%",   INPUT_INFO::ACTION::JUMP },
		{ "%DASH%",   INPUT_INFO::ACTION::DASH },
		{ "%CROUCH%", INPUT_INFO::ACTION::CROUCH },
		{ "%LIGHT%",  INPUT_INFO::ACTION::LIGHT },
		{ "%MAP%",    INPUT_INFO::ACTION::MAP },
		{ "%GRAB%",   INPUT_INFO::ACTION::GRAB },
		{ "%DELIVER%",   INPUT_INFO::ACTION::GRAB },
	};

	// 単一アクションタグの置換
	for (const auto& pair : singleTagMap)
	{
		if (text.find(pair.first) != std::string::npos)
		{
			// 1スロット目のキー名を取得
			std::string keyName = GetSlot1KeyName(pair.second);
			// 文字列で置換
			text = ReplaceAll(text, pair.first, keyName);
		}
	}

	// --- 複合・特殊タグの個別置換 ---
	if (text.find("%MOVE%") != std::string::npos)
	{
		std::string moveStr = "";

		// 現在アクティブなデバイスを取得
		auto activeDevice = InputManager::GetInstance()->GetActiveDevice();

		if (activeDevice == InputManager::ActiveDevice::PAD)
		{
			// パッド時の文字列を取得
			moveStr = GetPadMoveString();
		}
		else
		{
			moveStr = GetSlot1KeyName(INPUT_INFO::ACTION::MOVE_FORWARD) + "/" +
				GetSlot1KeyName(INPUT_INFO::ACTION::MOVE_LEFT) + "/" +
				GetSlot1KeyName(INPUT_INFO::ACTION::MOVE_BACK) + "/" +
				GetSlot1KeyName(INPUT_INFO::ACTION::MOVE_RIGHT);
		}

		// 文字列で置換
		text = ReplaceAll(text, "%MOVE%", moveStr);
	}

	if (text.find("%WHEEL%") != std::string::npos)
	{
		std::string wheelStr = 
			GetSlot1KeyName(INPUT_INFO::ACTION::ITEM_PUSH) + "/" + 
			GetSlot1KeyName(INPUT_INFO::ACTION::ITEM_PULL);

		// 文字列で置換
		text = ReplaceAll(text, "%WHEEL%", wheelStr);
	}

	return text;
}

void TutorialScene::DrawTutorialTextWithHighlight(int x, int y, const std::string& originalText, unsigned int normalColor, unsigned int highlightColor, int fontHandle)
{
	// % から次の % までのタグを検出して描画する
	size_t currentPos = 0;
	int currentX = x;

	// 文字列の後ろになるまでループ
	while (currentPos < originalText.length())
	{
		// 最初の '%' の位置を探す
		size_t tagStart = originalText.find('%', currentPos);

		// '%' がこれ以上存在しないなら
		if (tagStart == std::string::npos)
		{
			// 残りの文字列を描画
			std::string sub = originalText.substr(currentPos);
			DrawStringToHandle(currentX, y, sub.c_str(), normalColor, fontHandle);
			break;
		}
		// '%' の直前までに通常テキストが存在するなら
		if (tagStart > currentPos)
		{
			// タグ手前までの通常文字列を取り出して描画する
			std::string normalStr = originalText.substr(currentPos, tagStart - currentPos);
			DrawStringToHandle(currentX, y, normalStr.c_str(), normalColor, fontHandle);
			// 描画した長さ分、X座標を進める
			currentX += GetDrawStringWidthToHandle(normalStr.c_str(), static_cast<int>(normalStr.length()), fontHandle);
		}

		// 閉じの '%' を探す
		size_t tagEnd = originalText.find('%', tagStart + 1);

		// 閉じの '%' が見つからないなら
		if (tagEnd == std::string::npos)
		{
			// 残りの文字列をすべて通常色で描画して終了
			std::string sub = originalText.substr(tagStart);
			DrawStringToHandle(currentX, y, sub.c_str(), normalColor, fontHandle);
			break;
		}

		// タグを取り出す
		std::string tag = originalText.substr(tagStart, tagEnd - tagStart + 1);

		// タグを実際のキー名に変換
		std::string keyName = ConvertTutorialTagToKeyName(tag);

		// 置換後のキー名をハイライト色で描画
		DrawStringToHandle(currentX, y, keyName.c_str(), highlightColor, fontHandle);
		// 描画した長さ分、X座標を進める
		currentX += GetDrawStringWidthToHandle(keyName.c_str(), static_cast<int>(keyName.length()), fontHandle);

		// 次の検索位置へ進める
		currentPos = tagEnd + 1;
	}
}

std::string TutorialScene::GetPadMoveString(void)
{
	// InputManagerから全アクションのバインド情報を取得
	const auto& binds = InputManager::GetInstance()->GetActionBinds();

	// 移動アクションの登録情報を検索
	auto it = binds.find(INPUT_INFO::ACTION::MOVE_FORWARD);
	if (it == binds.end()) return "---";

	const auto& bind = it->second.pad[0];	// 1スロット目
	if (bind.code == -1) return "---";

	// スティックが割り当てられているなら
	if (bind.type == InputManager::BindType::PAD_STICK)
	{
		auto stick = static_cast<INPUT_INFO::PAD_STICK>(bind.code);

		// 左スティックが割り当てられているなら
		if (stick >= INPUT_INFO::PAD_STICK::LEFT_UP && stick <= INPUT_INFO::PAD_STICK::LEFT_RIGHT)
		{
			return "LEFT STICK";
		}
		// 右スティックが割り当てられているなら
		if (stick >= INPUT_INFO::PAD_STICK::RIGHT_UP && stick <= INPUT_INFO::PAD_STICK::RIGHT_RIGHT)
		{
			return "RIGHT STICK";
		}
	}

	// スティック以外が割り当てられているならそのまま表示
	return GetSlot1KeyName(INPUT_INFO::ACTION::MOVE_FORWARD) + "/" +
		GetSlot1KeyName(INPUT_INFO::ACTION::MOVE_LEFT) + "/" +
		GetSlot1KeyName(INPUT_INFO::ACTION::MOVE_BACK) + "/" +
		GetSlot1KeyName(INPUT_INFO::ACTION::MOVE_RIGHT);
}
