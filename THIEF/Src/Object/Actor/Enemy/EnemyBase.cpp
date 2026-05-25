#include <string>
#include <fstream>
#include <sstream>
#include "../../Common/AnimationController.h"
#include "../../../Common/Math/Math.h"
#include "../../../Common/Transform/MatrixUtility.h"
#include "EnemyBase.h"

EnemyBase::EnemyBase(void)
{
	animationController_ = nullptr;
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Load(void)
{
	LoadCsvData();
}

void EnemyBase::Draw(void)
{
	MV1DrawModel(modelId_);
}

void EnemyBase::Release(void)
{
	MV1DeleteModel(modelId_);

	if (animationController_ != nullptr)
	{
		animationController_->Release();
		delete animationController_;
		animationController_ = nullptr;
	}
}

//std::vector<EnemyBase::Edge> EnemyBase::FindPath(int startNodeId, int goalNodeId)
//{
//	int nodeCount = static_cast<int>(way_.size());
//
//
//	std::vector<Edge> list;
//
//	return list;
//}

void EnemyBase::LoadCsvData(void)
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

		Waypoint way;
		way.id = pointId;
		way.pos = position;

		way_.push_back(way);
	}
}

void EnemyBase::DelayRotate(void)
{
	// 移動方向から角度に変換する
	float goal = atan2f(moveDir_.x, moveDir_.z);

	// 常に最短経路で補間
	angle_.y = Math::LerpAngle(angle_.y, goal, 0.2f);
}

void EnemyBase::AddEdge(int fromId, int toId)
{
	VECTOR posA = way_[fromId].pos;
	VECTOR posB = way_[toId].pos;

	// 線分とモデルの衝突判定
	MV1_COLL_RESULT_POLY res = MV1CollCheck_Line(stageId_, -1, posA, posB);

	if (res.HitFlag) return;
	Edge edge;
	edge.way.id = way_[toId].id;	// 行った先
	edge.way.pos = way_[toId].pos;	// 行った先の座標
	// 行った先から行った元を引いて、VSizeでfloat型に変換
	edge.cost = VSize(VSub(posB, posA));

	edgeList_[fromId].push_back(edge);
}