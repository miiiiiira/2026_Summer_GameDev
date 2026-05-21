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

	// 杯クラス(アイテム)
	item_ = new Goblet();
	item_->Load();

	// ステージの作成
	StageCreate();

	// カメラの作成
	CameraCreate();

	// ランタンの作成
	LanternCreate();

	// プレイヤーの作成
	PlayerCreate();

	// 敵の作成
	EnemyCreate();

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

	// アイテムとプレイヤーの掴める範囲の当たり判定
	CheckItemPlayerCollision();
	// アイテムとステージの当たり判定
	CheckItemStageCollision();
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
	cont->SetCameraPosAngle(cameraPos, cameraAngle);

	// 座標の設定
	auto trans = lantern->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };
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

	// プレイヤークラスにカメラ、アイテム、ランタンのポインタを渡す
	playerController->SetPointers(camera,item_,lantern);
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
	// プレイヤーの取得
	auto player = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);

	// プレイヤーが何かを掴んでいる状態だったら処理を行わない
	if (player->grapState_ != GraspingState::NOT_GRAPING)return;

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

	// ローカル座標
	VECTOR localPosRot;

	// 相対座標
	VECTOR LOCAL_POS = { 0.0f,0.0f,player->GetRangeMax()};

	localPosRot = VTransform(LOCAL_POS, matRot);

	// 座標に反映
	downPos = VAdd(*cameraPos, localPosRot);

	// 線分とモデルの衝突判定
	MV1_COLL_RESULT_POLY hitResult =
		MV1CollCheck_Line(itemModelId, -1, topPos, downPos);

	// 当たっているかつ、マウスが押されていたら
	if (hitResult.HitFlag&& InputManager::GetInstance()->IsClickMouseLeft())
	{
		// カメラと当たった場所の距離を求める
		float distance = VSize(VSub(hitResult.HitPosition, *cameraPos));

		// 距離が最低距離値よりも小さかったら最低距離値にする
		if (distance < PlayerController::MIN_RENGE)distance = PlayerController::MIN_RENGE;

		// アイテムの追従モードをオンにする
		item_->StartGrabbing({ 0,0,distance });
		// 掴み状態にする
		player->StartGrabbing(distance);
	}
}

void GameScene::CheckItemStageCollision(void)
{
	// ステージの取得
	auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);

	// ステージが取得できないなら処理しない
	if (!stage) return;

	// ステージのモデルID取得
	int stageModelId = stage->GetModelId();

	// 接触したかどうか
	bool isHitStage = false;

	// 現在の座標
	VECTOR currentPos = item_->GetInfo().pos_;

	// 前回の座標取得
	VECTOR prevPos = item_->GetInfo().prevPos_;

	// 移動量(前回の座標と現在の座標の差分)
	VECTOR move = VSub(currentPos, prevPos);

	// 移動距離(距離を取る)
	float distance = VSize(move);

	// アイテムの半径取得
	float itemRad = item_->GetInfo().collisionRadius_;

	// 半径ベースで分割
	int stepCount = (int)(distance / itemRad) + 1;

	// 最大ステップ数
	const int MAX_STEP = 16;

	// 最低制限(最低でも1回は動くようにする)
	if (stepCount < 1)
		stepCount = 1;

	// 最大制限(試行回数が多すぎて重くなるのを防ぐ)
	if (stepCount > MAX_STEP)
		stepCount = MAX_STEP;

	// 1ステップ移動量を計算
	VECTOR stepMove = VScale(move, 1.0f / stepCount);

	// 判定用座標
	VECTOR testPos = prevPos;

	// 分割移動
	for (int step = 0; step < stepCount; step++)
	{
		// 判定用座標に1ステップの移動量を足し、次座標を計算
		VECTOR nextPos = VAdd(testPos, stepMove);

		// カプセル開始座標
		VECTOR capStart = nextPos;
		capStart.y -= item_->GetInfo().collisionOffset_;

		// カプセル終了座標
		VECTOR capEnd = nextPos;
		capEnd.y += item_->GetInfo().collisionOffset_;

		// 衝突判定　ステージモデルとカプセル(アイテム)
		MV1_COLL_RESULT_POLY_DIM hitResult =
			MV1CollCheck_Capsule(
				stageModelId,
				-1,
				capStart,
				capEnd,
				itemRad
			);

		// 当たっていなければ移動確定
		if (hitResult.HitNum <= 0)
		{
			testPos = nextPos;

			// 衝突情報の解放
			MV1CollResultPolyDimTerminate(hitResult);

			continue;
		}

		// 当たっているためフラグをたてる
		isHitStage = true;

		// 衝突押し戻し
		for (int i = 0; i < hitResult.HitNum; i++)
		{
			auto& hit = hitResult.Dim[i];

			// 法線
			VECTOR normal = hit.Normal;

			// 法線の長さを確認
			float normalLength = VSize(normal);

			// 小さすぎたら処理をしない
			if (normalLength < 0.0001f)
				continue;

			// 正規化
			normal = VNorm(normal);

			// めり込み解消
			
			// 三角形最近点取得
			bool isHit =
				HitCheck_Capsule_Triangle(
					capStart,
					capEnd,
					itemRad,
					hit.Position[0],
					hit.Position[1],
					hit.Position[2]
				);

			// めり込んでなければスキップ
			if (!isHit)
				continue;

			// 押し戻し量計算

			// 少しずつ押し戻す
			const float PUSH_BACK = 0.5f;

			// 押し出し量を計算
			VECTOR push = VScale(normal, PUSH_BACK);

			// 押し戻し
			nextPos = VAdd(nextPos, push);

			// 壁沿い移動
			float dot = VDot(stepMove, normal);

			// 壁に向かっている場合だけ
			if (dot < 0.0f)
			{
				// 法線成分除去
				VECTOR slide =
					VSub(
						stepMove,
						VScale(normal, dot)
					);

				// スライド移動へ変更
				stepMove = slide;
			}

			// カプセル位置更新
			capStart = VAdd(capStart, push);
			capEnd = VAdd(capEnd, push);
		}

		// 衝突情報の解放
		MV1CollResultPolyDimTerminate(hitResult);
	}

	// 最終位置更新
	item_->SetPos(testPos);

	// 空中にいるならダメージ処理しない
	if (!isHitStage)return;

	float hitSpeed = 0;

	// アイテムが掴まれていたら
	if (item_->GetInfo().isGrabbed)
	{
		hitSpeed = VSize(VSub(currentPos,prevPos));
		// 重力分を引いておく(重力でお金が削れるのを防ぐため)
		hitSpeed -= VSize(item_->GetInfo().velocity_);
	}
	// 掴まれていないかつ、空中状態から1度も設置していなかったら
	else if (!item_->GetInfo().isGrabbed && !item_->GetInfo().hasTouchedStage_)
	{
		hitSpeed = VSize(VSub(item_->GetInfo().grabbedPos_,testPos));
		// 重力分を引いておく(重力でお金が削れるのを防ぐため)
		hitSpeed -= VSize(item_->GetInfo().velocity_);

		// 設置したためフラグを接触フラグを立てる
		item_->TrueHasToucheStage();
	}

	// スピードをそのままダメージに変換
	int damage = static_cast<int>(hitSpeed);

	// マイナス値になるのを防ぐ
	item_->SetDamage(abs(damage));

}
