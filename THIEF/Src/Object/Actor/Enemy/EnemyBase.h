#pragma once
#include<vector>
#include <DxLib.h>

class EnemyBase
{
public:

	struct Waypoint
	{
		int id;			// ウェイポイントID
		VECTOR pos;		// 座標
	};

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
	virtual void Load(void) = 0;
	// 初期化処理
	virtual void Init(void) = 0;
	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);
	// 解放処理
	void Release(void);

	void FindPath(int startNodeId, int goalNodeId);


protected:

	// モデルのハンドル
	int modelId_;

	// 大きさ
	VECTOR scale_;

	// 向き
	VECTOR angle_;

	// 座標
	VECTOR pos_;

private:
	void AddEdge(int fromId, int toId);

	std::vector<Waypoint> way_;
	std::vector<std::vector<Edge>> edgeList_;
};