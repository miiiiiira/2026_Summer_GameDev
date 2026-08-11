#include<fstream>

#include "../Application.h"
#include "../Common/Manager/Input/InputManager.h" 
#include "../Common/Manager/Audio/AudioManager.h"
#include "SceneManager.h"
#include "GameClear/GameClear.h"
#include "GameOver/GameOver.h"
#include "Pause/Pause.h"

#include "../Object/ObjectManager/ObjectManager.h"
#include "../Object/Tag.h"

#include "../Object/Component/Collider/3DCollider/CapsuleCollider.h"
#include "../Object/Component/Collider/StageCollider/StageCollider.h"
#include "../Object/Component/Render/Render3D.h"
#include "../Object/Component/Camera/Camera.h"
#include "../Object/Component/PlayerController/PlayerController.h"
#include "../Object/Component/Animation/Animation.h"
#include "../Object/Component/Stage/Stage.h"
#include "../Object/Component/Wisp/Wisp.h"
#include "../Object/Component/Transform/Transform.h"
#include "../Common/Transform/MatrixUtility.h"
#include "../Common/CameraUtility/CameraUtility.h"

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

	// カメラユーティリティにカメラのポインタを渡す
	CameraUtility::SetCameraPoint(objectManger_->FindComponentWithTag<Camera>(Tag::Camera));

	// 火生成
	auto wisp = objectManger_->CreateObject();

	// タグを付与
	wisp->SetTagAndPriority(Tag::Wisp);

	// 座標の設定
	auto trans = wisp->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };

	// 描画
	auto render = wisp->AddComponent<Render3D>();
	render->SetModel("Data/Model/Player/Light/Wisp.mv1");

	// 火機能
	auto cont = wisp->AddComponent<Wisp>();

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

	if (InputManager::GetInstance()->IsActionDown(INPUT_INFO::ACTION::PAUSE))
	{
		// ポーズ画面を開いたサウンド
		AudioManager::GetInstance()->PlaySE(SoundID::SYS_PAUSE_ON);
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

	// オブジェクトの3D描画
	objectManger_->Draw3D();

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

		for (const auto& point2 : points_)
		{
			auto stage = objectManger_->FindComponentWithTag<Stage>(Tag::Stage);
			MV1_COLL_RESULT_POLY hit =
				MV1CollCheck_Line(stage->GetModelId(), -1, point.pos, point2.pos);
			float nodeDistance = VSquareSize(VSub(point.pos, point2.pos));
			if (!hit.HitFlag && nodeDistance <= 1300.0f * 1300.0f)
			{
				DrawLine3D(point.pos, point2.pos, 0x00ff00);
			}
		}

		DrawFormatString(20, y,
			0xffffff, "座標(%.2f, %.2f, %.2f)",
			point.pos.x, point.pos.y, point.pos.z);
		y += 20;
	}

	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::RETURN))
	{
		time_ = 60 * 3;
	}

	if (--time_ > 0)
	{
		DrawString(Application::SCREEN_SIZE_X / 2 - 50, 20, "ポイントの位置をセーブしました!", 0xff0000);
	}

	// オブジェクトの2D描画
	objectManger_->Draw2D();
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
	cameraObj->SetTagAndPriority(Tag::Camera);

	// 座標の設定
	auto cameraTrans = cameraObj->AddComponent<Transform>();

	// カメラコンポーネントの付与
	auto camera = cameraObj->AddComponent<Camera>();

	// カメラのモードを変更
	camera->ChangeMode(Camera::MODE::FOLLOW);
}

void DebugScene::WispCreate(void)
{
	// ランタン生成
	auto wisp = objectManger_->CreateObject();

	// タグを付与
	wisp->SetTagAndPriority(Tag::Wisp);

	// 描画
	auto render = wisp->AddComponent<Render3D>();
	render->SetModel("Data/Model/Player/Light/Wisp.mv1");

	// ランタン機能
	auto cont = wisp->AddComponent<Wisp>();

	// 座標の設定
	auto trans = wisp->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };
}

void DebugScene::PlayerCreate(void)
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

	// プレイヤー取得
	auto playerController = objectManger_->FindComponentWithTag<PlayerController>(Tag::Player);

	// ランタン取得
	auto wisp = objectManger_->FindComponentWithTag<Wisp>(Tag::Wisp);

	// ライトのポインタを渡す
	playerController->SetWisp(wisp);
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
	stage->SetTagAndPriority(Tag::Stage);

	// 座標の設定
	auto trans = stage->AddComponent<Transform>();
	trans->pos_ = { 0.0f,0.0f,0.0f };

	// 描画
	auto render = stage->AddComponent<Render3D>();
	render->SetModel("Data/Model/Stage/Stage2.mv1");

	// ステージ機能
	stage->AddComponent<Stage>();
}

void DebugScene::PlaceDebugPoint(void)
{
	VECTOR cameraPos = CameraUtility::GetCameraPos();

	// 左クリックでカメラの座標をデバックポイントとして追加
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::ADD_POINT))
	{
		Point point;
		point.id = pointNum_;
		point.pos = cameraPos;
		point.pos.y -= 100.0f;
		points_.push_back(point);

		pointNum_++;
	}

	// 右クリックで最後のデバッグポイントを削除
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::REMOVE_POINT))
	{
		// 線分の上座標
		VECTOR topPos = cameraPos;
		// 線分の下座標
		VECTOR downPos = cameraPos;

		// カメラの視線方向のベクトルを計算
		// DxlibのVTransformを使用
		VECTOR forward = VGet(0.0f, 0.0f, 1.0f); // 前方向をZ軸とする
		// カメラの方向を算出
		VECTOR cameraDir = CameraUtility::AddCameraPosLocalPos(forward);

		// 相対座標
		VECTOR LOCAL_POS = { 0.0f,0.0f,500.0f };

		// 座標に反映
		downPos = CameraUtility::AddCameraPosLocalPos(LOCAL_POS);

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
	if (InputManager::GetInstance()->IsDebugActionDown(INPUT_INFO::DEBUG_ACTION::RETURN))
	{
		// デバッグポイントの保存
		SavePoints();
	}
}

void DebugScene::SavePoints(void)
{
	std::ofstream ofs("Data/PointSave2.csv");

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