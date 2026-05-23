#pragma once
#include<vector>
#include <functional>
#include "EnemyData.h"

class EnemyBase
{
public:
	
	// ウェイポイント
	struct Waypoint
	{
		int id;			// ウェイポイントID
		VECTOR pos;		// 座標
	};

	// エッジ
	struct Edge
	{
		Waypoint way;	// 行った先のウェイポイントID
		float cost;		// 距離
	};

	// コンストラクタ
	EnemyBase(void);
	// デストラクタ
	virtual ~EnemyBase(void);

	// 読み込み処理
	virtual void Load(void);
	// 初期化処理
	virtual void Init(int id = -1) = 0;
	// 更新処理
	virtual void Update(void) = 0;
	// 描画処理
	virtual void Draw(void);
	// 解放処理
	void Release(void);

	std::vector<Edge> FindPath(int startNodeId, int goalNodeId);

protected:

	EnemyData data_;

	int stageId_ = -1;

	std::vector<Waypoint> way_;
	std::vector<std::vector<Edge>> edgeList_;

	void AddEdge(int fromId, int toId);

private:
	void LoadCsvData(void);

};