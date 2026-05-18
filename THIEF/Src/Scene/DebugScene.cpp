#include<fstream>

#include "../Application.h"
#include "../Input/InputManager.h"
#include "../Audio/AudioManager.h"
#include "SceneManager.h"
#include "GameClear/GameClear.h"
#include "GameOver/GameOver.h"
#include "Pause/Pause.h"

#include "../Object/Actor/Item/Goblet/Goblet.h"

#include "../Object/ObjectManager/ObjectManager.h"
#include "../Object/Tag.h"

#include "../Object/Component/Collider/3DCollider/CapsuleCollider.h"
#include "../Object/Component/Collider/StageCollider/StageCollider.h"
#include "../Object/Component/Render/Render3D.h"
#include "../Object/Component/Camera/Camera.h"
#include "../Object/Component/PlayerController/PlayerController.h"
#include "../Object/Component/Animation/Animation.h"
#include "../Object/Component/Stage/Stage.h"
#include "../Object/Component/Lantern/Lantern.h"
#include "../Object/Component/Transform/Transform.h"
#include "../Common/Transform/MatrixUtility.h"

#include "DebugScene.h"

DebugScene::DebugScene(void)
	:
	pointNum_(0),
	time_(0),
	edit_(EditMode::ENEMY_NODE_POINT)
{
}

DebugScene::~DebugScene(void)
{
}

void DebugScene::Init(void)
{
	// オブジェクトマネージャー初期化
	objectManger_->Init();

	pointNum_ = 0;
	time_ = 0;
	edit_ = EditMode::SPAWN_POINT;
}

void DebugScene::Load(void)
{
	// オブジェクトマネージャーの生成
	objectManger_ = new ObjectManager();

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
}

void DebugScene::LoadEnd(void)
{
	Init();
}

void DebugScene::Update(void)
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

	// デバッグポイントの配置
	PlaceDebugPoint();
}

void DebugScene::Draw(void)
{
	// オブジェクトの描画前
	objectManger_->PreDraw();

	// オブジェクトの描画
	objectManger_->Draw();

	int y = 20;

	// デバッグポイント群を球体描画
	for (const auto& point : points_)
	{
		DrawSphere3D(
			point.pos,
			30.0f,
			16,
			GetColor(255, 0, 0),
			GetColor(255, 0, 0),
			false);

		DrawFormatString(20, y,
			0xffffff, "座標(%.2f, %.2f, %.2f)",
			point.pos.x, point.pos.y, point.pos.z);
		y += 20;
	}
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_RETURN))
	{
		time_ = 60 * 3;
	}

	if (--time_ > 0)
	{
		DrawString(Application::SCREEN_SIZE_X / 2 - 50, 20, "ポイントの位置をセーブしました!", 0xff0000);
	}
}

void DebugScene::Release(void)
{
	// オブジェクトマネージャー削除
	delete objectManger_;

	points_.clear();
}

void DebugScene::CameraCreate(void)
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

void DebugScene::LanternCreate(void)
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

void DebugScene::PlayerCreate(void)
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
	playerController->SetPointers(camera, nullptr, lantern);
}

void DebugScene::EnemyCreate(void)
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

void DebugScene::StageCreate(void)
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

void DebugScene::PlaceDebugPoint(void)
{
	switch (edit_)
	{
	case DebugScene::EditMode::ENEMY_NODE_POINT:
		PlaceEnemyNodePoint();
		break;
	case DebugScene::EditMode::ITEM_NODE_POINT:
		PlaceItemNodePoint();
		break;
	case DebugScene::EditMode::SPAWN_POINT:
		PlaceSpawnPoint();
		break;
	default:
		break;
	}
}

void DebugScene::PlaceEnemyNodePoint(void)
{
	auto camera = objectManger_->FindComponentWithTag<Camera>(Tag::Camera);
	VECTOR* cameraPos = &camera->GetTransform()->pos_;
	VECTOR* cameraAngle = camera->GetAngle();

	// 左クリックでカメラの座標をデバックポイントとして追加
	if (InputManager::GetInstance()->IsTrgMouseLeft())
	{
		cameraPos->y -= 100.0f;

		Point point;
		point.id = pointNum_;
		point.pos = *cameraPos;
		points_.push_back(point);

		pointNum_++;
	}

	// 右クリックで最後のデバッグポイントを削除
	if (InputManager::GetInstance()->IsTrgMouseRight())
	{
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
		VECTOR localPosRot = {};

		// 相対座標
		VECTOR LOCAL_POS = { 0.0f,0.0f,500.0f };

		localPosRot = VTransform(LOCAL_POS, matRot);

		// 座標に反映
		downPos = VAdd(*cameraPos, localPosRot);

		auto targetIt = points_.end();

		for (auto it = points_.begin(); it != points_.end(); ++it)
		{
			if (HitCheck_Line_Sphere(topPos, downPos, it->pos, 30.0f))
			{
				targetIt = it;
				break;
			}
		}

		if (targetIt != points_.end())
		{
			// 指定したデバッグポイントを削除
			points_.erase(targetIt);
			pointNum_--;

			// IDを振りなおす
			for (int i = 0; i < points_.size(); i++)
			{
				points_[i].id = i;
			}
		}
	}

	// エンターでデバックポイントを保存
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_RETURN))
	{
		// デバッグポイントの保存
		SavePoints();
	}
}

void DebugScene::PlaceItemNodePoint(void)
{
}

void DebugScene::PlaceSpawnPoint(void)
{
	auto camera = objectManger_->FindComponentWithTag<Camera>(Tag::Camera);
	VECTOR* cameraPos = &camera->GetTransform()->pos_;
	VECTOR* cameraAngle = camera->GetAngle();

	// 左クリックでカメラの座標をデバックポイントとして追加
	if (InputManager::GetInstance()->IsTrgMouseLeft())
	{
		cameraPos->y -= 100.0f;

		Point point;
		point.id = pointNum_;
		point.pos = *cameraPos;
		points_.push_back(point);

		pointNum_++;
	}

	// 右クリックで最後のデバッグポイントを削除
	if (InputManager::GetInstance()->IsTrgMouseRight())
	{
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
		VECTOR localPosRot = {};

		// 相対座標
		VECTOR LOCAL_POS = { 0.0f,0.0f,500.0f };

		localPosRot = VTransform(LOCAL_POS, matRot);

		// 座標に反映
		downPos = VAdd(*cameraPos, localPosRot);

		auto targetIt = points_.end();

		for (auto it = points_.begin(); it != points_.end(); ++it)
		{
			if (HitCheck_Line_Sphere(topPos, downPos, it->pos, 30.0f))
			{
				targetIt = it;
				break;
			}
		}

		if (targetIt != points_.end())
		{
			// 指定したデバッグポイントを削除
			points_.erase(targetIt);
			pointNum_--;

			// IDを振りなおす
			for (int i = 0; i < points_.size(); i++)
			{
				points_[i].id = i;
			}
		}
	}

	// エンターでデバックポイントを保存
	if (InputManager::GetInstance()->IsTrgDown(KEY_INPUT_RETURN))
	{
		// デバッグポイントの保存
		SavePoints();
	}
}

void DebugScene::SavePoints(void)
{
	switch (edit_)
	{
	case DebugScene::EditMode::ENEMY_NODE_POINT:
		SaveEnemyNodePoints();
		break;
	case DebugScene::EditMode::ITEM_NODE_POINT:
		SaveItemNodePoints();
		break;
	case DebugScene::EditMode::SPAWN_POINT:
		SaveSpawnPoints();
		break;
	default:
		break;
	}
}

void DebugScene::SaveEnemyNodePoints(void)
{
	std::ofstream ofs("Data/EnemyPointSave.csv");

	if (!ofs) return;

	// 形式: x y z
	for (const auto& point : points_) {
		ofs << point.id << "," << point.pos.x << "," <<
			point.pos.y << "," << point.pos.z << "\n";
	}
	ofs.close();
}

void DebugScene::SaveItemNodePoints(void)
{
	std::ofstream ofs("Data/ItemPointSave.csv");

	if (!ofs) return;

	// 形式: x y z
	for (const auto& point : points_) {
		ofs << point.id << "," << point.pos.x << "," <<
			point.pos.y << "," << point.pos.z << "\n";
	}
	ofs.close();
}

void DebugScene::SaveSpawnPoints(void)
{
	std::ofstream ofs("Data/PlayerPointSave.csv");

	if (!ofs) return;

	// 形式: x y z
	for (const auto& point : points_) {
		ofs << point.id << "," << point.pos.x << "," <<
			point.pos.y << "," << point.pos.z << "\n";
	}
	ofs.close();
}

void DebugScene::LoadPoints(void)
{
}