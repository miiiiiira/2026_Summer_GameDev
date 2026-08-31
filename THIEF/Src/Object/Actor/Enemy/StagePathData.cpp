#include <fstream>
#include <sstream>

#include "../../../Common/Math/Math.h"
#include "StagePathData.h"

StagePathData::StagePathData(int stageId)
{
	stageId_ = stageId;
}

StagePathData::~StagePathData(void)
{
}

void StagePathData::Load(const std::string& csvPath)
{
	way_.clear();
	edgeList_.clear();

	std::ifstream ifs(csvPath);
	if (!ifs) return;

	std::string line;
	std::string c;

	// CSVからノードを読み込む
	while (std::getline(ifs, line))
	{
		std::istringstream stream(line);
		int index = 0;
		int pointId = 0;
		float posX = 0.0f, posY = 0.0f, posZ = 0.0f;

		while (std::getline(stream, c, ','))
		{
			if (index == 0)      pointId = std::stoi(c);
			else if (index == 1) posX = std::stof(c);
			else if (index == 2) posY = std::stof(c);
			else if (index == 3) posZ = std::stof(c);
			index++;
		}

		WAYPOINT way = {};
		way.id = pointId;
		way.pos = VGet(posX, posY, posZ);

		way_.push_back(way);
	}

	// エッジリストの要素数をノード数に合わせる
	edgeList_.resize(way_.size());

	// 一定距離内のノード同士を自動接続する
	for (int i = 0; i < static_cast<int>(way_.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(way_.size()); j++)
		{
			if (i == j) continue;

			float nodeDistance = VSquareSize(VSub(way_[j].pos, way_[i].pos));
			if (nodeDistance > NODE_CONNECT_MAX_DISTANCE_SQ) continue;

			AddEdge(i, j);
		}
	}
}

void StagePathData::AddEdge(int fromId, int toId)
{
	VECTOR posA = way_[fromId].pos;
	VECTOR posB = way_[toId].pos;

	float checkRadius = 50.0f; // 敵の大きさに応じて調整

	// posA から posB への間に障害物があるか判定
	MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, posA, posB, checkRadius);

	if (res.HitNum > 0)
	{
		// 障害物がある場合はエッジを接続せずに終了
		MV1CollResultPolyDimTerminate(res);
		return;
	}

	// 検出結果のメモリ解放
	MV1CollResultPolyDimTerminate(res);

	// 障害物がない場合のみエッジを追加
	EDGE edge = {};
	edge.way.id = way_[toId].id;
	edge.way.pos = way_[toId].pos;
	edge.cost = VSize(VSub(posB, posA));

	edgeList_[fromId].push_back(edge);
}