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

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };

	// 座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f,10.0f,500.0f };

	// コンストラクタ
	Yeti(void);

	// デストラクタ
	~Yeti(void)override;
	// 初期化
	void Init(int id = -1) override;
	// 読み込み処理
	void Load(void) override;
	void Update(void)override;
	void Draw(void) override;

private:

	VECTOR nextWayPoint_;

	// 更新ステップ
	float step_;

	STATE state_;

	std::vector<int> candidates_;

	int currentNodeId_;		// 今いるノード
	int prevNodeId_;		// 前回のノード
	int prevPrevNodeId_;	// 前々回のノード
	int nextNodeId_;
	float patrolRadius_;	// 巡回用の半径

	void SetMoveDirPatrol(void);

	// 次のノードを選ぶ
	int SelectNextNode(void);
	// ノード到着時
	void ArriveNode(void);

	// 状態遷移
	void ChangeState(STATE state);

	// 状態遷移
	void ChangeThink(void);
	void ChangeIdle(void);
	void ChangePatrol(void);
	void ChangeChase(void);
	void ChangeAttack(void);
	void ChangeHit(void);
	void ChangeDead(void);
	void ChangeEnd(void);

	// 状態別更新
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdatePatrol(void);
	void UpdateChase(void);
	void UpdateAttack(void);
	void UpdateHit(void);
	void UpdateDead(void);
	void UpdateEnd(void);

};