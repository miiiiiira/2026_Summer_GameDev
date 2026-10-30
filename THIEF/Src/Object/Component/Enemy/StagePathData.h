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
		int id_ = -1;		// ノードID
		VECTOR pos_ = {};	// ノードの座標
	};

	// エッジ構造体
	struct Edge
	{
		Waypoint way_ = {};	// 接続先のノード
		float cost_ = 0.0f;	// 接続先までの移動コスト
	};

	// コンストラクタ
	StagePathData(int stageId);

	// デストラクタ
	~StagePathData(void);

	// CSVの読み込みとエッジ構築を一括で行う
	void Load(const std::string& csvPath);

	// ノードのリストを取得
	const std::vector<Waypoint>* GetWayList(void) const { return &waypoints_; }

	// エッジのリストを取得
	const std::vector<std::vector<Edge>>* GetEdgeList(void) const { return &edgeList_; }

private:

	// ノードを自動接続する最大距離（2乗）
	static constexpr float NODE_CONNECT_MAX_DISTANCE_SQ = 1300.0f * 1300.0f;

	// ノードのリスト
	std::vector<Waypoint> waypoints_;

	// ノードIDごとの接続先エッジのリスト
	std::vector<std::vector<Edge>> edgeList_;

	// ステージID
	int stageId_ = -1;

	// エッジの追加
	void AddEdge(int fromId, int toId);
};