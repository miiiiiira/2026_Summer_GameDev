#include <fstream>
#include <sstream>

#include "../../../Common/Math/Math.h"
#include "StagePathData.h"

StagePathData::StagePathData(void)
{
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

	// 一定距離内のノード同士を自動接続（AddEdge）する
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
	EDGE edge = {};
	edge.way = way_[toId];
	edge.cost = VSize(VSub(way_[toId].pos, way_[fromId].pos));

	edgeList_[fromId].push_back(edge);
}