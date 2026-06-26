#include "../../../Application.h"
#include <string>
#include <fstream>
#include <sstream>
#include "EnemyBase.h"
#include "Yeti/Yeti.h"
#include "Mushnub/Mushnub.h"
#include "EnemyManager.h"

EnemyManager::EnemyManager(PlayerController* player, int id)
{
	if (player == nullptr) return;
	player_ = player;

	if (id == -1) return;
	stageId_ = id;
}

EnemyManager::~EnemyManager(void)
{
}

void EnemyManager::Init(void)
{
}

void EnemyManager::Load(void)
{
	enemyModelIds_.emplace_back(
		MV1LoadModel((Application::PATH_MODEL + "Enemy/Yeti.mv1").c_str()));
	enemyModelIds_.emplace_back(
		MV1LoadModel((Application::PATH_MODEL + "Enemy/Mushnub_Evolved.mv1").c_str()));

	LoadCsvData();

	edgeList_.clear();
	edgeList_.resize(way_.size());

	for (int i = 0; i < static_cast<int>(way_.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(way_.size()); j++)
		{
			// 同じノードならスキップ
			if (i == j) continue;

			// 遠すぎるノードも除外する
			float nodeDistance = VSquareSize(VSub(way_[j].pos, way_[i].pos));
			if (nodeDistance > NODE_CONNECT_MAX_DISTANCE_SQ) continue;

			AddEdge(i, j);
		}
	}

	// Yetiを生成
	EnemyBase* newEnemy = new Yeti(enemyModelIds_[static_cast<int>(ENEMY_TYPE::YETI)]);
	// 派生クラスで持っている各変数（way_ など）を流し込む
	// EnemyBase で共通化しているため、ここでデータを渡すだけでOK！
	newEnemy->Load();
	newEnemy->Init(player_, stageId_, way_, edgeList_);
	// リストに追加
	enemys_.push_back(newEnemy);
}

void EnemyManager::Update(void)
{
	for (EnemyBase* enemy : enemys_)
	{
		enemy->Update();
	}
}

void EnemyManager::Draw(void)
{
	for (EnemyBase* enemy : enemys_)
	{
		enemy->Draw();
	}
}

void EnemyManager::Release(void)
{
	for (EnemyBase* enemy : enemys_)
	{
		enemy->Release();
		delete enemy;
	}

	for (int id : enemyModelIds_)
	{
		MV1DeleteModel(id);
	}
}

std::vector<EnemyBase*> EnemyManager::GetEnemys(void)
{
	return enemys_;
}

void EnemyManager::AddEdge(int fromId, int toId)
{
	VECTOR posA = way_[fromId].pos;
	VECTOR posB = way_[toId].pos;

	float checkRadius = 50.0f;

	// カプセルとモデルの衝突判定
	MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, posA, posB, checkRadius);

	// 当たっていたら、省く
	if (res.HitNum > 0)
	{
		// 後始末をする
		MV1CollResultPolyDimTerminate(res);
		return;
	}

	EDGE edge = {};
	edge.way.id = way_[toId].id;	// 行った先
	edge.way.pos = way_[toId].pos;	// 行った先の座標
	// 行った先から行った元を引いて、VSizeでfloat型に変換
	edge.cost = VSize(VSub(posB, posA));

	edgeList_[fromId].push_back(edge);

	// 後始末をする
	MV1CollResultPolyDimTerminate(res);
}

void EnemyManager::LoadCsvData(void)
{
	// 初期化
	way_.clear();
	std::ifstream ifs = std::ifstream("Data/PointSave.csv");

	if (!ifs) return;
	//ファイルを１行ずつ読み込む
	std::string line;		//1行の文字情報
	std::string c;			//1文字情報

	while (getline(ifs, line))
	{
		//1行情報　string を ifstream　の仲間に変換
		std::istringstream stream(line);
		int index = 0;
		int pointId = 0;
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;

		while (getline(stream, c, ','))
		{
			// 列のインデックスに応じて代入先を切り替える
			if (index == 0)
			{
				pointId = std::stoi(c);
			}
			else if (index == 1)
			{
				posX = std::stof(c);
			}
			else if (index == 2)
			{
				posY = std::stof(c);
			}
			else if (index == 3)
			{
				posZ = std::stof(c);
			}
			index++;
		}

		VECTOR position = VGet(posX, posY, posZ);

		WAYPOINT way = {};
		way.id = pointId;
		way.pos = position;

		way_.push_back(way);
	}
}