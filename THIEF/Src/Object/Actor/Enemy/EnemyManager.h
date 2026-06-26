#pragma once
#include <vector>
#include <DxLib.h>
#include "EnemyCommon.h"

class EnemyBase;
class PlayerController;

using namespace EnemyCommon;

class EnemyManager
{
public:

	enum class ENEMY_TYPE
	{
		YETI,
		MUSHNUB,
		SKELETON,
		STATUE,
		MAX,
	};

	EnemyManager(PlayerController* player, int stageId = -1);
	~EnemyManager(void);

	void Init(void);
	void Load(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	std::vector<EnemyBase*> GetEnemys(void);

private:

	const float NODE_CONNECT_MAX_DISTANCE_SQ = 1300.0f * 1300.0f;

	PlayerController* player_;
	int stageId_;

	// エネミー用のモデルハンドルID
	std::vector<int> enemyModelIds_;

	// エネミー
	std::vector<EnemyBase*> enemys_;

	std::vector<WAYPOINT> way_;					// ウェイポイントを格納
	std::vector<std::vector<EDGE>> edgeList_;	// 行動可能な辺を格納

	void AddEdge(int fromId, int toId);

	void LoadCsvData(void);
};

