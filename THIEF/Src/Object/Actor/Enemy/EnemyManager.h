#pragma once
#include <vector>
#include <unordered_map>
#include <DxLib.h>
#include "EnemyCommon.h"

class EnemyBase;
class PlayerController;

using namespace EnemyCommon;

class EnemyManager
{
public:

	EnemyManager(void);
	~EnemyManager(void);

	void Init(PlayerController* player, int id = -1);
	void CreateEnemyStage1(void);
	void CreateEnemyStage2(void);
	void CreateEnemyStage3(void);
	void LoadStage1(void);
	void LoadStage2(void);
	void LoadStage3(void);
	void Update(void);
	void Draw(void);
	void Release(void);

	std::vector<EnemyBase*> GetEnemys(void);

private:

	const float NODE_CONNECT_MAX_DISTANCE_SQ = 1300.0f * 1300.0f;

	PlayerController* player_;
	int stageId_;

	// エネミー用のモデルハンドルID
	std::unordered_map<ENEMY_TAG, int> enemyModelIds_;

	// エネミー
	std::vector<EnemyBase*> enemys_;

	std::vector<WAYPOINT> way_;					// ウェイポイントを格納
	std::vector<std::vector<EDGE>> edgeList_;	// 行動可能な辺を格納

	void AddEdge(int fromId, int toId);

	void LoadCsvData(void);
};

