#pragma once
#include<vector>
#include <DxLib.h>
#include "EnemyCommon.h"

class AnimationController;
class WeaponBase;
class WeaponPunch;
class PlayerController;
class EnemyManager;

using namespace EnemyCommon;

class EnemyBase
{
public:
	
	// コンストラクタ
	EnemyBase(int modelId = -1);
	// デストラクタ
	virtual ~EnemyBase(void);

	// 読み込み処理
	virtual void Load(void);
	// 初期化処理
	void Init(PlayerController* player = nullptr,int stageId = -1,
		const std::vector<WAYPOINT>& way = {},
		const std::vector<std::vector<EDGE>>& edgeList = {});
	virtual void OnInitialize(void) = 0;

	// 更新処理
	virtual void Update(void) = 0;
	// 描画処理
	virtual void Draw(void);
	// 解放処理
	void Release(void);

	// 始点から終点までの最短経路を計算し、エッジのリストとして返す
	void FindPath(int startNodeId, int goalNodeId);

	// 武器クラスを取得
	WeaponBase* GetUseWeapon(void);

	int GetModelId(void);
	VECTOR GetPos(void);
	void SetPos(VECTOR pos);
	float GetRadius(void);
	VECTOR GetPrevPos(void);
	VECTOR GetStart(void);
	VECTOR GetEnd(void);
	void SetGround(bool isGround);
	float GetVelocity(void);
	void SetVelocity(float velocityY);
	void SetTag(ENEMY_TAG tag);
	ENEMY_TAG GetTag(void);

protected:

	PlayerController* player_;
	int stageId_;
	const std::vector<WAYPOINT>* way_ = nullptr;
	const std::vector<std::vector<EDGE>>* edgeList_ = nullptr;

	// ジャンプ力
	const float JUMP_POW = 25.0f;

	// 重力加速度
	const float GRAVITY = -1.98f;

	// 最大落下速度
	const float MAX_FALL = -40.0f;

	AnimationController* animationController_;

	// 武器
	WeaponBase* useWeapon_;
	WeaponPunch* weaponPunch_;

	// 敵の情報
	int modelId_;
	VECTOR scale_;
	VECTOR angle_;
	VECTOR localAngle_;
	VECTOR pos_;
	float radius_;
	VECTOR moveDir_;
	float moveSpeed_;
	VECTOR movePow_;
	VECTOR prevPos_;
	VECTOR jumpPow_;
	float stepJump_;
	bool isJump_;
	bool isNotice_;
	bool isGround_;	
	float velocityY_ = 0.0f;	// 現在の落下速度
	ENEMY_TAG tag_;

	// 開始位置
	VECTOR startOffset_;
	// 終了位置
	VECTOR endOffset_;

	std::vector<float> minCosts_;				// ポイントへの最短経路合計
	std::vector<int> parentNodes_;				// どこから来たかを記録
	std::vector<EDGE> path_;					// 探索された最短経路を格納

	VECTOR nextWayPoint_;
	// 更新ステップ
	float step_;
	std::vector<int> candidates_;	// 候補のノードを格納する
	int currentNodeId_;		// 今いるノード
	int prevNodeId_;		// 前回のノード
	int prevPrevNodeId_;	// 前々回のノード
	int nextNodeId_;
	float viewRadius_;		// 視野用の半径
	bool isHit_;
	float patrolRadius_;	// 巡回用の半径
	float targetLostTimer_;

	// 移動方向に応じた遅延回転
	void DelayRotate(void);

	// プレイヤー追従処理
	void LookPlayer(void);

	// 移動処理
	void Move(void);

	float GetDistance(VECTOR pos1, VECTOR pos2);

	// プレイヤーを見つけたかどうか
	bool CheckPlayerDiscovery(float radius);

	// プレイヤーが特定のエリアにいるかどうか
	bool IsPlayerInArea(VECTOR minPos, VECTOR maxPos);

	// ジャンプ処理
	void Jump(void);

	// 重力処理
	void ApplyGravity();

	void SetMoveDirPatrol(void);

	// 次のノードを選ぶ
	int SelectNextNode(void);
	// ノード到着時
	void ArriveNode(void);

	// 一番近いノードを探す
	int FindNearestNode(VECTOR pos);

	// ノードを経由して追従
	void ChaseNode(void);
	// 直接追従
	void ChaseDirect(void);

	// 追従用の線分かステージと当たっているかどうか
	bool CheckChaseLineCollision(VECTOR pPos, VECTOR ePos, float radius);
};