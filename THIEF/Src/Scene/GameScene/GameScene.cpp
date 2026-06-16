#include "GameScene.h"

#include <algorithm>

#include "../../Application.h"
#include "../../Common/Manager/Input/InputManager.h"
#include "../../Common/Manager/Audio/AudioManager.h"
#include "../../Common/Manager/Score/ScoreManager.h"
#include "../SceneManager.h"
#include "../GameClear/GameClear.h"
#include "../GameOver/GameOver.h"
#include "../Shop/ShopScene.h"
#include "../Pause/Pause.h"

#include "../../Object/Actor/Enemy/EnemyBase.h"
#include "../../Object/Actor/Enemy/Yeti/Yeti.h"
#include "../../Object/Actor/Enemy/Weapon/WeaponBase.h"
#include "../../Common/Crosshair/Crosshair.h"
#include "../../Object/ObjectManager/ObjectManager.h"

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

	// クロスヘアの初期化処理
	crosshair_->Init();

	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	auto player = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);
	enemy_->Init(&(player->GetTransform()->pos_),stage->GetModelId());

	// BGM再生
	AudioManager::GetInstance()->PlayBGM(SoundID::BGM_GAME_1);
}

void GameScene::Load(void)
{
	// サウンド読みこみ
	AudioManager::GetInstance()->LoadSceneSound(LoadScene::GAME);

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
	//case STAGE_NUM::STAGE_3:
	//	
	//	// ステージ3の初期化処理
	//	Stage3Init();

	//	break;
	default:
		break;
	}

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
		// ステージクリアにする
		SceneManager::GetInstance()->TrueStageClear();
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

	CheckEnemyAttack();
	CollisionEnemyToStage();

	// スコアマネージャーの更新
	ScoreManager::GetInstance().Update();

	if (InputManager::GetInstance()->PauseButtons())
	{
		// ポーズ画面を開いたサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_ON);
		// ポーズモードへ
		SceneManager::GetInstance()->PushScene(std::make_shared<Pause>());
		return;
	}

	if (SceneManager::GetInstance()->GetIsClear())
	{
		// トータルスコアを初期化
		ScoreManager::GetInstance().ResetTotalPrice();
		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		// ゲームクリアシーンへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameClear>());
		return;
	}

	if (SceneManager::GetInstance()->GetIsOver())
	{
		// トータルスコアを初期化
		ScoreManager::GetInstance().ResetTotalPrice();
		// ステージ情報などを初期化する
		SceneManager::GetInstance()->ResetGame();
		// ゲームオーバーシーンへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<GameOver>());
		return;
	}

	if (SceneManager::GetInstance()->GetIsStageClear())
	{
		// 納品した文の金額をショップで使える金額に加算
		ScoreManager::GetInstance().AddTotalPrice(ScoreManager::GetInstance().GetDeliveryPrice());
		// ショップシーンへ
		SceneManager::GetInstance()->ChangeScene(std::make_shared<ShopScene>());
		return;
	}

}

void GameScene::Draw(void)
{
	// オブジェクトの描画前
	objectManger_->PreDraw();

	enemy_->Draw();

	// オブジェクトの3D描画
	objectManger_->Draw3D();

	// クロスヘアの描画
	crosshair_->Draw();

	// オブジェクトの2D描画
	objectManger_->Draw2D();

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
	render->SetModel("Data/Model/Stage/Stage1.mv1");

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
	col->startOffset_ = PlayerController::STANDING_CAP_START_OFFSET;
	col->endOffset_ = PlayerController::STANDING_CAP_END_OFFSET;
	col->radius_ = 40.0f;

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

void GameScene::ItemCreateStage1(void)
{
	ItemCreate(Tag::Item_Goblet, { -100,50.0f,0.0f });
	ItemCreate(Tag::Item_Potion_Red, { 100.0f,50.0f,30.0f });
	ItemCreate(Tag::Item_Potion_Green, { 100.0f,50.0f,30.0f });
	ItemCreate(Tag::Item_Potion_Blue, { 100.0f,50.0f,30.0f });
	ItemCreate(Tag::Item_Amphora, { 100.0f,50.0f,30.0f });
	ItemCreate(Tag::Item_Bottle, { 100.0f,50.0f,30.0f });
	ItemCreate(Tag::Item_Jar, { 100.0f,50.0f,30.0f });
	ItemCreate(Tag::Item_Mug, { 100.0f,50.0f,30.0f });
	ItemCreate(Tag::Item_Skull, { 100.0f,50.0f,30.0f });
}

void GameScene::ItemCreateStage2(void)
{
	ItemCreateStage1();
}

void GameScene::ItemCreateStage3(void)
{
}

void GameScene::Stage1Init(void)
{
	// ステージの作成
	StageCreate();

	// ランタンの作成
	LanternCreate();

	// プレイヤーの作成
	PlayerCreate();

	// 敵の作成
	EnemyCreate();

	// アイテムの作成
	ItemCreateStage1();
}

void GameScene::Stage2Init(void)
{
	// ステージの作成
	StageCreate();

	// ランタンの作成
	LanternCreate();

	// プレイヤーの作成
	PlayerCreate();

	// 敵の作成
	EnemyCreate();

	// アイテムの作成
	ItemCreateStage2();
}

void GameScene::Stage3Init(void)
{
	// ステージの作成
	StageCreate();

	// ランタンの作成
	LanternCreate();

	// プレイヤーの作成
	PlayerCreate();

	// 敵の作成
	EnemyCreate();

	// アイテムの作成
	ItemCreateStage3();
}

void GameScene::CheckEnemyAttack(void)
{
	// 武器の情報
	WeaponBase* useWeapon = enemy_->GetUseWeapon();

	// 攻撃中（描画されている）なら
	if (useWeapon->IsAlive())
	{
		auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
		// 敵の攻撃とステージのの当たり判定
		MV1_COLL_RESULT_POLY_DIM hits = MV1CollCheck_Sphere
		(
			stage->GetModelId(),
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

		// プレイヤーと敵の攻撃の当たり判定
		auto player = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);
		VECTOR startPos = player->GetOwner()->GetComponent<CapsuleCollider>()->GetStart();
		VECTOR endPos = player->GetOwner()->GetComponent<CapsuleCollider>()->GetEnd();
		float radius = player->GetOwner()->GetComponent<CapsuleCollider>()->radius_;

		if (Collision::HitSphereCapsule(useWeapon->GetPos(), useWeapon->GetCollisionRadius(),
			startPos, endPos, radius))
		{
			player->SetDamage(10);

			VECTOR moveDir = VNorm(VSub(startPos, useWeapon->GetPos()));
			player->SetHitReact(moveDir, 30.0f);
			useWeapon->SetAlive(false);
		}
	}
}

void GameScene::CollisionEnemyToStage(void)
{
	// 必要なコンポーネントが存在しないなら処理しない
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
	if (!stage) return;

	// 現在座標
	VECTOR currentPos = enemy_->GetPos();

	// 前フレーム座標
	VECTOR prevPos = enemy_->GetPrevPos();

	// 今フレーム移動量
	VECTOR move = VSub(currentPos, prevPos);

	// 衝突判定開始座標
	VECTOR pos = prevPos;

	// 毎フレーム初期化
	enemy_->SetGround(false);

	// 1フレーム中に複数回衝突する場合に対応するための最大反復回数
	const int MAX_BOUNCE = 5;

	// めり込み防止用の押し出し量
	const float SKIN = 1.0f;

	// 衝突とスライドを繰り返す
	for (int bounce = 0; bounce < MAX_BOUNCE; bounce++)
	{
		// 残り移動量の長さ
		float length = VSize(move);

		// ほぼ移動していないなら終了
		if (length < 0.01f)
			break;

		float radius = enemy_->GetRadius();

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

			// カプセル始点・終点を算出
			VECTOR capStart = VAdd(nextPos, enemy_->GetStart());
			VECTOR capEnd = VAdd(nextPos, enemy_->GetEnd());

			// ステージとカプセルの衝突判定
			auto result =
				MV1CollCheck_Capsule(
					stage->GetModelId(),
					-1,
					capStart,
					capEnd,
					radius);

			float bestPush = 0.0f;
			VECTOR bestNormal = VGet(0, 0, 0);

			bool collision = false;

			// ヒットしたポリゴンを調べる
			for (int i = 0; i < result.HitNum; i++)
			{
				auto& poly = result.Dim[i];

				// ポリゴン法線
				VECTOR normal = VNorm(poly.Normal);

				// 現在の移動方向と法線の向きから
				// 正面衝突している度合いを求める
				float push = -VDot(VNorm(move), normal);

				// 背面や平行な面は無視
				if (push <= 0.0f)
					continue;

				// 最も正面から当たっている面を採用
				if (push > bestPush)
				{
					bestPush = push;
					bestNormal = normal;
				}

				collision = true;
			}

			// 衝突結果を解放
			MV1CollResultPolyDimTerminate(result);

			// 衝突したら探索終了
			if (collision)
			{
				hit = true;
				hitNormal = bestNormal;
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

		// 衝突していない最後の座標へ戻す
		pos = safePos;

		// 少しだけ法線の方向へ押し出して
		// めり込みを防止する
		pos = VAdd(pos, VScale(hitNormal, SKIN));

		float velocityY = enemy_->GetVelocity();

		// 床判定
		if (hitNormal.y > 0.6f)
		{
			// 接地フラグを立てる
			enemy_->SetGround(true);


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

		enemy_->SetVelocity(velocityY);

		// 衝突後に残っている移動割合
		float remainRatio = (float)(stepCount - hitStep) / stepCount;

		VECTOR remainMove = VScale(move, remainRatio);

		// 壁スライド処理
		// 法線方向成分を除去して壁に沿って移動させる
		float dot = VDot(remainMove, hitNormal);

		if (dot < 0.0f)
		{
			remainMove = VSub(remainMove, VScale(hitNormal, dot));
		}

		// 次の反復で残り移動量を処理
		move = remainMove;
	}

	// 最終位置を反映
	enemy_->SetPos(pos);
}

void GameScene::ItemCreate(Tag tag, VECTOR pos)
{
	auto ItemData = ItemTable_Stage1::Table.find(tag);

	// アイテムの作成
	auto item = objectManger_->CreateObject();

	// タグを付与
	item->SetTagAndPriority(tag);

	// 座標の設定
	auto trans = item->AddComponent<Transform>();
	trans->pos_ = pos;

	// 描画
	auto render = item->AddComponent<Render3D>();
	render->SetModel(ItemData->second);

	Item* itemBase = nullptr;
	// アイテム機能
	switch (ItemData->first)
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

	// アイテムの当たり判定
	auto itemCol = item->AddComponent<ItemCollider>();
	// プレイヤーを渡す
	itemCol->SetPlayer(playerController);
	// ステージを渡す
	itemCol->SetStage(stage);
	// クロスヘアを渡す
	itemCol->SetCrosshair(crosshair_);

	// ステージにアイテムを渡す
	if (itemBase != nullptr)
	{
		stage->SetItem(itemBase);
	}
}
