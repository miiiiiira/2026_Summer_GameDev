#pragma once
#include <vector>
#include <string>
#include <DxLib.h>

class StagePathData
{
public:

	// ノード構造体
	struct WAYPOINT
	{
		int id;	
		VECTOR pos;
	};

	// エッジ構造体
	struct EDGE
	{
		WAYPOINT way;
		float cost;
	};

	// コンストラクタ・デストラクタ
	StagePathData(int stageId);
	~StagePathData(void);

	// CSVの読み込みとエッジ構築を一括で行う
	void Load(const std::string& csvPath);

	// 敵（EnemyBase）から参照するためのゲッター
	const std::vector<WAYPOINT>* GetWayList(void) const { return &waypoints_; }
	const std::vector<std::vector<EDGE>>* GetEdgeList(void) const { return &edgeList_; }

private:

	// エッジの追加
	void AddEdge(int fromId, int toId);

private:

	// ノードのリスト
	std::vector<WAYPOINT> waypoints_;

	// エッジのリスト
	std::vector<std::vector<EDGE>> edgeList_;

	// ステージID
	int stageId_;

	// ノードを自動接続する最大距離（2乗）
	static constexpr float NODE_CONNECT_MAX_DISTANCE_SQ = 1300.0f * 1300.0f;
};

