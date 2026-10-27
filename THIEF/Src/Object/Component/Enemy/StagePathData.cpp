#include <fstream>
#include <sstream>

#include "../../../Common/Math/Math.h"
#include "StagePathData.h"

namespace
{
	// エッジの判定に使う半径
	constexpr float EDGE_CHECK_RADIUS = 50.0f;
}

StagePathData::StagePathData(int stageId)
	:
	stageId_(stageId)
{
}

StagePathData::~StagePathData(void)
{
}

void StagePathData::Load(const std::string& csvPath)
{
	// 前回読み込んだノードとエッジを空にする
	waypoints_.clear();
	edgeList_.clear();

	// CSVファイルを開く
	std::ifstream ifs(csvPath);

	// ファイルが開けなかった場合は処理を終了
	if (!ifs) return;

	// CSVの各行を読み込むための変数
	std::string line;
	std::string c;

	// CSVからノードを読み込む
	while (std::getline(ifs, line))
	{
		// 1行をカンマ区切りで分割してノード情報を取得
		std::istringstream stream(line);
		int index = 0;
		int pointId = 0;
		float posX = 0.0f, posY = 0.0f, posZ = 0.0f;

		// カンマ区切りで各要素を取得
		while (std::getline(stream, c, ','))
		{
			if (index == 0)      pointId = std::stoi(c);	// ノードID
			else if (index == 1) posX = std::stof(c);		// X座標
			else if (index == 2) posY = std::stof(c);		// Y座標
			else if (index == 3) posZ = std::stof(c);		// Z座標

			index++;
		}

		// ノード情報をwaypoints_に追加
		Waypoint way = {};
		way.id_ = pointId;
		way.pos_ = VGet(posX, posY, posZ);

		waypoints_.push_back(way);
	}

	// エッジリストの要素数をノード数に合わせる
	edgeList_.resize(waypoints_.size());

	// 一定距離内のノード同士を自動接続する
	for (int i = 0; i < static_cast<int>(waypoints_.size()); i++)
	{
		for (int j = 0; j < static_cast<int>(waypoints_.size()); j++)
		{
			// 自分自身のノードはスキップ
			if (i == j) continue;

			// ノード間の距離を計算（2乗距離で比較）
			float nodeDistance = VSquareSize(VSub(waypoints_[j].pos_, waypoints_[i].pos_));

			// 一定距離以上のノードは接続しない
			if (nodeDistance > NODE_CONNECT_MAX_DISTANCE_SQ) continue;

			// エッジを追加
			AddEdge(i, j);
		}
	}
}

void StagePathData::AddEdge(int fromId, int toId)
{
	// ノードの座標を取得
	VECTOR posA = waypoints_[fromId].pos_;
	VECTOR posB = waypoints_[toId].pos_;

	// posA から posB への間に障害物があるか判定
	MV1_COLL_RESULT_POLY_DIM res = MV1CollCheck_Capsule(stageId_, -1, posA, posB, EDGE_CHECK_RADIUS);

	if (res.HitNum > 0)
	{
		// 障害物がある場合はエッジを接続せずに終了
		MV1CollResultPolyDimTerminate(res);
		return;
	}

	// 検出結果のメモリ解放
	MV1CollResultPolyDimTerminate(res);

	// 障害物がない場合のみエッジを追加
	Edge edge = {};
	edge.way_.id_ = waypoints_[toId].id_;
	edge.way_.pos_ = waypoints_[toId].pos_;
	edge.cost_ = VSize(VSub(posB, posA));

	edgeList_[fromId].push_back(edge);
}