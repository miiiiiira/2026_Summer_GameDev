#pragma once
#include <vector>
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
		PATROL,		// 巡回
		SURPRISE,	// 発見
		CHASE,		// 追跡
		ATTACK,		// 攻撃
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
	Yeti(void);

	// デストラクタ
	~Yeti(void)override;

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void Draw3D(void) override;		// 3D描画
	void Draw2D(void) override;		// 2D描画

private:

	// 敵の状態
	STATE state_ = STATE::NONE;

	// 追跡インターバルの計測用タイマー
	float chaseTimer_ = 0.0f;

	// SE再生からの経過時間タイマー
	float seTimer_ = 0.0f;

	// 状態遷移
	void ChangeState(STATE state);

	// 状態別変更
	void ChangeThink(void);		// 考える
	void ChangeIdle(void);		// 待機
	void ChangePatrol(void);	// 巡回
	void ChangeSurprise(void);	// 発見
	void ChangeChase(void);		// 追跡
	void ChangeAttack(void);	// 攻撃

	// 状態別更新
	void UpdateThink(void);		// 考える
	void UpdateIdle(void);		// 待機
	void UpdatePatrol(void);	// 巡回
	void UpdateSurprise(void);	// 発見
	void UpdateChase(void);		// 追跡
	void UpdateAttack(void);	// 攻撃
};