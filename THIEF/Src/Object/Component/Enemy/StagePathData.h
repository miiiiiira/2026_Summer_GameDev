#pragma once
#include <vector>
#include <string>
#include <DxLib.h>

class StagePathData
{
public:

	// ノード構造体
	struct Waypoint
	{
		int id;	
		VECTOR pos;
	};

	// エッジ構造体
	struct Edge
	{
		Waypoint way;
		float cost;
	};

	// コンストラクタ
	StagePathData(int stageId);

	// デストラクタ
	~StagePathData(void);

	// CSVの読み込みとエッジ構築を一括で行う
	void Load(const std::string& csvPath);

	// 敵（EnemyBase）から参照するためのゲッター
	const std::vector<Waypoint>* GetWayList(void) const { return &waypoints_; }
	const std::vector<std::vector<Edge>>* GetEdgeList(void) const { return &edgeList_; }

private:

	// ノードを自動接続する最大距離（2乗）
	static constexpr float NODE_CONNECT_MAX_DISTANCE_SQ = 1300.0f * 1300.0f;

	// ノードのリスト
	std::vector<Waypoint> waypoints_;

	// エッジのリスト
	std::vector<std::vector<Edge>> edgeList_;

	// ステージID
	int stageId_;

	// エッジの追加
	void AddEdge(int fromId, int toId);
};

