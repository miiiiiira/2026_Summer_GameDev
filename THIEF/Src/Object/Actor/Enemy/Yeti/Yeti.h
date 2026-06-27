#pragma once
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

	// 敵のアニメーション
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
	Yeti(int modelId);

	// デストラクタ
	~Yeti(void)override;
	// 初期化処理
	void OnInitialize(void) override;
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
	static constexpr VECTOR DEFAULT_POS = { -5617.04f,10.0f,6573.71f };
	
	static constexpr float CHASE_INTERVAL = 0.5f;

	static constexpr float LOST_LIMIT_TIME = 10.0f;

	STATE state_;

	float chaseTimer_;

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