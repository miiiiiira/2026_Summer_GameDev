//#include <string>
//#include <fstream>
//#include <sstream>
//#include "../../../Application.h"
//#include "../../../Common/Math/Math.h"
//#include "EnemyBase.h"
//#include "Yeti/Yeti.h"
//#include "Mushnub/Mushnub.h"
//#include "Giggle/Giggle.h"
//#include "Statue/Statue.h"
//#include "Skeleton/Skeleton.h"
//#include "EnemyManager.h"
//
//EnemyManager::EnemyManager(void)
//{
//}
//
//EnemyManager::~EnemyManager(void)
//{
//}
//
//void EnemyManager::Init(PlayerController* player, int id)
//{
//	if (player == nullptr) return;
//	player_ = player;
//
//	if (id == -1) return;
//	stageId_ = id;
//
//	edgeList_.clear();
//	edgeList_.resize(way_.size());
//
//	for (int i = 0; i < static_cast<int>(way_.size()); i++)
//	{
//		for (int j = 0; j < static_cast<int>(way_.size()); j++)
//		{
//			// 同じノードならスキップ
//			if (i == j) continue;
//
//			// 遠すぎるノードも除外する
//			float nodeDistance = VSquareSize(VSub(way_[j].pos, way_[i].pos));
//			if (nodeDistance > NODE_CONNECT_MAX_DISTANCE_SQ) continue;
//
//			AddEdge(i, j);
//		}
//	}
//}
//
//void EnemyManager::CreateEnemyStage1(void)
//{
//	// Giggleを生成
//	EnemyBase* newEnemy = new Giggle();
//	newEnemy->Load();
//	newEnemy->SetTag(ENEMY_TAG::GIGGLE);
//	newEnemy->Init();
//	// リストに追加
//	enemys_.push_back(newEnemy);
//
//	// Yetiを生成
//	newEnemy = new Yeti(enemyModelIds_[ENEMY_TAG::YETI]);
//	newEnemy->Load();
//	newEnemy->SetTag(ENEMY_TAG::YETI);
//	VECTOR pos = { -5617.04f,10.0f,6573.71f };
//	newEnemy->SetPos(pos);
//	newEnemy->Init(player_, stageId_, way_, edgeList_);
//	// リストに追加
//	enemys_.push_back(newEnemy);
//
//	// Mushnubを生成
//	newEnemy = new Mushnub(enemyModelIds_[ENEMY_TAG::MUSHNUB]);
//	newEnemy->Load();
//	newEnemy->SetTag(ENEMY_TAG::MUSHNUB);
//	newEnemy->Init(player_, stageId_, way_, edgeList_);
//	// リストに追加
//	enemys_.push_back(newEnemy);
//}
//
//void EnemyManager::CreateEnemyStage2(void)
//{
//	// Giggleを生成
//	EnemyBase* newEnemy = new Giggle();
//	newEnemy->Load();
//	newEnemy->SetTag(ENEMY_TAG::GIGGLE);
//	newEnemy->Init();
//	// リストに追加
//	enemys_.push_back(newEnemy);
//
//	// Yetiを生成
//	newEnemy = new Yeti(enemyModelIds_[ENEMY_TAG::YETI]);
//	newEnemy->Load();
//	newEnemy->SetTag(ENEMY_TAG::YETI);
//	VECTOR pos = { -7887.0f,10.0f,1399.0f };
//	newEnemy->SetPos(pos);
//	newEnemy->Init(player_, stageId_, way_, edgeList_);
//	// リストに追加
//	enemys_.push_back(newEnemy);
//
//	// Statueを生成
//	Statue* statue = new Statue(enemyModelIds_[ENEMY_TAG::STATUE]);
//	statue->Load();
//	statue->SetTag(ENEMY_TAG::STATUE);
//	statue->SetChasePos({ -7444.0f,40.0f,3570.0f });
//	statue->Init(player_, stageId_, way_, edgeList_);
//	// リストに追加
//	enemys_.push_back(statue);
//
//	// Skeletonを生成
//	float startX = -4000.0f;	// 檻の並びの開始X座標
//	float spacingX = 200.0f;	// 敵と敵の間隔（X軸方向）
//	float leftZ = 2197.0f;		// 左側の檻のZ座標
//	float rightZ = 680.0f;		// 右側の檻のZ座標
//	float posY = 12.0f;			// 地面のY座標
//
//	// 右側のスケルトン
//	for (int i = 0; i < 8; ++i)
//	{
//		Skeleton* skeleton = new Skeleton(enemyModelIds_[ENEMY_TAG::SKELETON]);
//		skeleton->Load();
//		skeleton->SetTag(ENEMY_TAG::SKELETON);
//
//		VECTOR pos = { startX - (i * spacingX), posY, rightZ };
//		skeleton->SetPos(pos);
//		float random = static_cast<float>(GetRand(360));
//		VECTOR angle = { 0.0f, Math::Deg2Rad(random), 0.0f };
//		skeleton->SetAngle(angle);
//
//		skeleton->SetSide(Skeleton::SIDE::RIGHT);
//		skeleton->Init(player_, stageId_);
//		enemys_.push_back(skeleton);
//	}
//
//	// 左側のスケルトン
//	for (int i = 0; i < 8; ++i)
//	{
//		Skeleton* skeleton = new Skeleton(enemyModelIds_[ENEMY_TAG::SKELETON]);
//		skeleton->Load();
//		skeleton->SetTag(ENEMY_TAG::SKELETON);
//
//		VECTOR pos = { startX - (i * spacingX), posY, leftZ };
//		skeleton->SetPos(pos);
//		float random = static_cast<float>(GetRand(360));
//		VECTOR angle = { 0.0f, Math::Deg2Rad(random), 0.0f };
//		skeleton->SetAngle(angle);
//
//		skeleton->SetSide(Skeleton::SIDE::LEFT);
//		skeleton->Init(player_, stageId_);
//		enemys_.push_back(skeleton);
//	}
//}
//
//void EnemyManager::CreateEnemyStage3(void)
//{
//	// Giggleを生成
//	EnemyBase* newEnemy = new Giggle();
//	newEnemy->Load();
//	newEnemy->SetTag(ENEMY_TAG::GIGGLE);
//	newEnemy->Init();
//	// リストに追加
//	enemys_.push_back(newEnemy);
//
//	// Yetiを生成
//	newEnemy = new Yeti(enemyModelIds_[ENEMY_TAG::YETI]);
//	newEnemy->Load();
//	newEnemy->SetTag(ENEMY_TAG::YETI);
//	VECTOR pos = { 5704.0f,10.0f,7919.0f };
//	newEnemy->SetPos(pos);
//	newEnemy->Init(player_, stageId_, way_, edgeList_);
//	// リストに追加
//	enemys_.push_back(newEnemy);
//
//	// Mushnubを生成
//	Mushnub* mushnub = new Mushnub(enemyModelIds_[ENEMY_TAG::MUSHNUB]);
//	mushnub->Load();
//	mushnub->SetTag(ENEMY_TAG::MUSHNUB);
//
//	// 1体目の座標とエリア指定
//	pos = { 6996.0f, 10.0f, 623.0f };
//	mushnub->SetPos(pos);
//	VECTOR minAreaPos = { 6547.0f, 1.0f, -609.0f };
//	VECTOR maxAreaPos = { 7688.0f, 700.0f, 882.0f };
//	mushnub->SetAreaPos(minAreaPos, maxAreaPos);
//	mushnub->SetChasePos(pos);
//	mushnub->Init(player_, stageId_, way_, edgeList_);
//	// リストに追加
//	enemys_.push_back(mushnub);
//
//
//	// Mushnubを生成
//	mushnub = new Mushnub(enemyModelIds_[ENEMY_TAG::MUSHNUB]);
//	mushnub->Load();
//	mushnub->SetTag(ENEMY_TAG::MUSHNUB);
//	// 2体目の座標とエリア指定
//	pos = { 3606.0f, 10.0f, 2610.0f };
//	mushnub->SetPos(pos);
//	minAreaPos = { 3458.0f, 1.0f, 2207.0f };
//	maxAreaPos = { 5081.0f, 700.0f, 3695.0f };
//	mushnub->SetAreaPos(minAreaPos, maxAreaPos);
//	mushnub->SetChasePos(pos);
//	mushnub->Init(player_, stageId_, way_, edgeList_);
//	// リストに追加
//	enemys_.push_back(mushnub);
//
//	// Statueを生成
//	Statue* statue = new Statue(enemyModelIds_[ENEMY_TAG::STATUE]);
//	statue->Load();
//	statue->SetTag(ENEMY_TAG::STATUE);
//	// 座標とエリア指定
//	pos = { 45.0f, 10.0f, 4680.0f };
//	statue->SetPos(pos);
//	minAreaPos = { -1200.0f, 1.0f, 4450.0f };
//	maxAreaPos = { 250.0f, 700.0f, 5810.0f };
//	statue->SetAreaPos(minAreaPos, maxAreaPos);
//	statue->SetChasePos({ 45.0f, 40.0f, 4680.0f });
//	statue->Init(player_, stageId_, way_, edgeList_);
//	// リストに追加
//	enemys_.push_back(statue);
//}
//
//void EnemyManager::LoadStage1(void)
//{
//	enemyModelIds_[ENEMY_TAG::YETI] = MV1LoadModel((Application::PATH_MODEL + "Enemy/Yeti.mv1").c_str());
//	enemyModelIds_[ENEMY_TAG::MUSHNUB] = MV1LoadModel((Application::PATH_MODEL + "Enemy/Mushnub_Evolved.mv1").c_str());
//
//	LoadCsvData("Data/pointSave.csv");
//}
//
//void EnemyManager::LoadStage2(void)
//{
//	enemyModelIds_[ENEMY_TAG::YETI] = MV1LoadModel((Application::PATH_MODEL + "Enemy/Yeti.mv1").c_str());
//	enemyModelIds_[ENEMY_TAG::STATUE] = MV1LoadModel((Application::PATH_MODEL + "Enemy/Statue.mv1").c_str());
//	enemyModelIds_[ENEMY_TAG::SKELETON] = MV1LoadModel((Application::PATH_MODEL + "Enemy/Skeleton.mv1").c_str());
//
//	LoadCsvData("Data/pointSave2.csv");
//}
//
//void EnemyManager::LoadStage3(void)
//{
//	enemyModelIds_[ENEMY_TAG::YETI] = MV1LoadModel((Application::PATH_MODEL + "Enemy/Yeti.mv1").c_str());
//	enemyModelIds_[ENEMY_TAG::MUSHNUB] = MV1LoadModel((Application::PATH_MODEL + "Enemy/Mushnub_Evolved.mv1").c_str());
//	enemyModelIds_[ENEMY_TAG::STATUE] = MV1LoadModel((Application::PATH_MODEL + "Enemy/Statue.mv1").c_str());
//
//	LoadCsvData("Data/pointSave3.csv");
//}
//
//void EnemyManager::Update(void)
//{
//	for (EnemyBase* enemy : enemys_)
//	{
//		enemy->Update();
//	}
//}
//
//void EnemyManager::Draw(void)
//{
//	for (EnemyBase* enemy : enemys_)
//	{
//		enemy->Draw();
//	}
//}
//
//void EnemyManager::Release(void)
//{
//	for (EnemyBase* enemy : enemys_)
//	{
//		enemy->Release();
//		delete enemy;
//	}
//
//	for (const auto& [tag, modelId] : enemyModelIds_)
//	{
//		MV1DeleteModel(modelId);
//	}
//	enemyModelIds_.clear();
//}
//
//std::vector<EnemyBase*> EnemyManager::GetEnemys(void)
//{
//	return enemys_;
//}
//
//void EnemyManager::AddEdge(int fromId, int toId)
//{
//	VECTOR posA = way_[fromId].pos;
//	VECTOR posB = way_[toId].pos;
//
//	float checkRadius = 50.0f;
//
//	// カプセルとモデルの衝突判定
//	MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, posA, posB, checkRadius);
//
//	// 当たっていたら、省く
//	if (res.HitNum > 0)
//	{
//		// 後始末をする
//		MV1CollResultPolyDimTerminate(res);
//		return;
//	}
//
//	EDGE edge = {};
//	edge.way.id = way_[toId].id;	// 行った先
//	edge.way.pos = way_[toId].pos;	// 行った先の座標
//	// 行った先から行った元を引いて、VSizeでfloat型に変換
//	edge.cost = VSize(VSub(posB, posA));
//
//	edgeList_[fromId].push_back(edge);
//
//	// 後始末をする
//	MV1CollResultPolyDimTerminate(res);
//}
//
//void EnemyManager::LoadCsvData(std::string path)
//{
//	// 初期化
//	way_.clear();
//	std::ifstream ifs = std::ifstream(path);
//
//	if (!ifs) return;
//	//ファイルを１行ずつ読み込む
//	std::string line;		//1行の文字情報
//	std::string c;			//1文字情報
//
//	while (getline(ifs, line))
//	{
//		//1行情報　string を ifstream　の仲間に変換
//		std::istringstream stream(line);
//		int index = 0;
//		int pointId = 0;
//		float posX = 0.0f;
//		float posY = 0.0f;
//		float posZ = 0.0f;
//
//		while (getline(stream, c, ','))
//		{
//			// 列のインデックスに応じて代入先を切り替える
//			if (index == 0)
//			{
//				pointId = std::stoi(c);
//			}
//			else if (index == 1)
//			{
//				posX = std::stof(c);
//			}
//			else if (index == 2)
//			{
//				posY = std::stof(c);
//			}
//			else if (index == 3)
//			{
//				posZ = std::stof(c);
//			}
//			index++;
//		}
//
//		VECTOR position = VGet(posX, posY, posZ);
//
//		WAYPOINT way = {};
//		way.id = pointId;
//		way.pos = position;
//
//		way_.push_back(way);
//	}
//}