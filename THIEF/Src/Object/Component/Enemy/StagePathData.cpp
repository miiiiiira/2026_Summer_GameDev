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
	// メモリ解放
	for (auto& edges : edgeList_)
	{
		// vectorをクリア
		edges.clear();

		// 容量を縮小してメモリを解放
		edges.shrink_to_fit();
	}

	// vectorをクリア
	edgeList_.clear();

	// 容量を縮小してメモリを解放
	edgeList_.shrink_to_fit();

	// vectorをクリア
	waypoints_.clear();

	// 容量を縮小してメモリを解放
	waypoints_.shrink_to_fit();
}

void StagePathData::Load(const std::string& csvPath)
{
	// 既存のメモリバッファを完全に破棄してリセットする
	waypoints_.clear();
	waypoints_.shrink_to_fit();

	// エッジリストの各要素をクリアしてメモリを解放
	for (auto& edges : edgeList_)
	{
		edges.clear();
		edges.shrink_to_fit();
	}

	// エッジリスト自体をクリアしてメモリを解放
	edgeList_.clear();
	edgeList_.shrink_to_fit();

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
		way.id = pointId;
		way.pos = VGet(posX, posY, posZ);

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
			float nodeDistance = VSquareSize(VSub(waypoints_[j].pos, waypoints_[i].pos));

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
	VECTOR posA = waypoints_[fromId].pos;
	VECTOR posB = waypoints_[toId].pos;

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
	Edge edge = {};
	edge.way.id = waypoints_[toId].id;
	edge.way.pos = waypoints_[toId].pos;
	edge.cost = VSize(VSub(posB, posA));

	edgeList_[fromId].push_back(edge);
}