#include <queue>
#include "EnemyBase.h"

EnemyBase::EnemyBase(void)
{
}

EnemyBase::~EnemyBase(void)
{
}

void EnemyBase::Update(void)
{

}

void EnemyBase::Draw(void)
{
	MV1DrawModel(modelId_);
}

void EnemyBase::Release(void)
{
	MV1DeleteModel(modelId_);
}

void EnemyBase::FindPath(int startNodeId, int goalNodeId)
{
	std::vector<Edge> list;

	int nodeId = goalNodeId;
	int minCost = edgeList_[nodeId][0].cost;

	for (int i = 0; i < edgeList_.size(); i++)
	{
		if (minCost > edgeList_[nodeId][i].cost)
		{
			minCost = edgeList_[nodeId][i].cost;
		}
	}
}

void EnemyBase::AddEdge(int fromId, int toId)
{
	VECTOR posA = way_[fromId].pos;
	VECTOR posB = way_[toId].pos;

	// 線分とモデルの衝突判定
	// MV1_COLL_RESULT_POLY res = MV1CollCheck_Line(, -1, posA, posB);


	//if (res.HitFlag) return;


	Edge edge;
	edge.way.id = way_[toId].id;	// 行った先
	// 行った先から行った元を引いて、VSizeでfloat型に変換
	edge.cost = VSize(VSub(posB, posA));

	edgeList_[fromId].push_back(edge);
}
