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

	StagePathData(void);
	~StagePathData(void);

	// CSVの読み込みとエッジ構築を一括で行う
	void Load(const std::string& csvPath);

	// 敵（EnemyBase）から参照するためのゲッター
	const std::vector<WAYPOINT>* GetWayList(void) const { return &way_; }
	const std::vector<std::vector<EDGE>>* GetEdgeList(void) const { return &edgeList_; }

private:
	void AddEdge(int fromId, int toId);

private:
	std::vector<WAYPOINT> way_;
	std::vector<std::vector<EDGE>> edgeList_;

	// ノードを自動接続する最大距離（2乗）
	static constexpr float NODE_CONNECT_MAX_DISTANCE_SQ = 1300.0f * 1300.0f;
};

