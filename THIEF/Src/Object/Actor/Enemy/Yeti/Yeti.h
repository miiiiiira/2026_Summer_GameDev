#pragma once
#include<vector>
#include "../EnemyBase.h"

class Yeti : public EnemyBase
{
public:

	// 敵の状態
	enum class STATE
	{
		NONE,		// なし
		THINK,		// 考える
		IDLE,		// 待機
		PATROL,		// 徘徊
		SURPRISE,	// 見つける
		CHASE,		// 追いかける
		ATTACK,		// 攻撃
		HIT_REACT,	// ダメージ
		DEAD,		// 死
		END,		// 終了
	};

	enum class ANIM_TYPE
	{
		DEATH,
		DUCK,
		HIT_REACT,
		IDLE,
		JUMP,
		JUMP_IDLE,
		JUMP_LAND,
		NO,
		PUNCH,
		RUN,
		WALK,
		WAVE,
		WEAPON,
		YES,
		MAX,
	};

	// コンストラクタ
	Yeti(void);

	// デストラクタ
	~Yeti(void)override;
	// 初期化
	void Init(VECTOR* pos, int id = -1) override;
	// 読み込み処理
	void Load(void) override;
	void Update(void)override;
	void Draw(void) override;

private:

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };

	// 座標
	static constexpr VECTOR DEFAULT_POS = { 1000.0f,10.0f,500.0f };
	
	static constexpr float CHASE_INTERVAL = 0.5f;

	static constexpr float LOST_LIMIT_TIME = 10.0f;

	VECTOR nextWayPoint_;

	// 更新ステップ
	float step_;

	STATE state_;

	std::vector<int> candidates_;	// 候補のノードを格納する

	int currentNodeId_;		// 今いるノード
	int prevNodeId_;		// 前回のノード
	int prevPrevNodeId_;	// 前々回のノード
	int nextNodeId_;
	float patrolRadius_;	// 巡回用の半径
	float viewRadius_;		// 視野用の半径
	float chaseTimer_;
	float targetLostTimer_;

	void SetMoveDirPatrol(void);

	// 次のノードを選ぶ
	int SelectNextNode(void);
	// ノード到着時
	void ArriveNode(void);

	// 移動処理
	void Move(void);

	// 一番近いノードを探す
	int FindNearestNode(VECTOR pos);

	// ノードを経由して追従
	void ChaseNode(void);
	// 直接追従
	void ChaseDirect(void);

	// 追従用の線分かステージと当たっているかどうか
	bool CheckChaseLineCollision(VECTOR pPos, VECTOR ePos);

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeThink(void);
	void ChangeIdle(void);
	void ChangePatrol(void);
	void ChangeSurprise(void);
	void ChangeChase(void);
	void ChangeAttack(void);
	void ChangeHit(void);
	void ChangeDead(void);
	void ChangeEnd(void);

	// 状態別更新
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdatePatrol(void);
	void UpdateSurprise(void);
	void UpdateChase(void);
	void UpdateAttack(void);
	void UpdateHit(void);
	void UpdateDead(void);
	void UpdateEnd(void);
};