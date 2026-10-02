#pragma once
#include "../EnemyBase.h"

class Giggle : public EnemyBase
{
public:

	// 敵の状態
	enum class STATE
	{
		NONE,		// なし
		IDLE,		// 待機
		THINK,		// 考える
		GIGGLING,	// 笑う
	};

	// コンストラクタ
	Giggle(void);

	// デストラクタ
	~Giggle(void)override;

	void Init(void) override;	// 初期化
	void Update(void)override;	// 更新

private:

	// 状態別ステップタイマー
	enum STEP_TIMER
	{
		STEP_IDLE = 10,		// 待機状態のステップタイマー
		STEP_GIGGLING = 20,	// 笑う状態のステップタイマー
	};

	// 確率計算の全体値
	static constexpr int MAX_PERCENTAGE = 100;

	// 待機状態に遷移する確率
	static constexpr int IDLE_PROBABILITY = 90;

	// 状態
	STATE state_;

	// 状態遷移
	void ChangeState(STATE state);

	// 状態遷移処理
	void ChangeIdle(void);		// 待機状態に遷移
	void ChangeThink(void);		// 考える状態に遷移
	void ChangeGiggling(void);	// 笑う状態に遷移

	// 状態別更新
	void UpdateIdle(void);		// 待機状態の更新
	void UpdateThink(void);		// 考える状態の更新
	void UpdateGiggling(void);	// 笑う状態の更新
};

