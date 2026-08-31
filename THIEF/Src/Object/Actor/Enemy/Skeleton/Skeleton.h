#pragma once
#include<vector>
#include "../EnemyBase.h"

class Skeleton : public EnemyBase
{
public:
	// 敵の状態
	enum class STATE
	{
		NONE,		// なし
		IDLE,		// 待機
		LOOK,		// 見つめる
		SCARE,		// 怖がらせる
		END,		// 終了
	};

	// 敵のアニメーション
	enum class ANIM_TYPE
	{
		ATTACK,
		DEATH,
		IDLE,
		RUNNING,
		SPAWN,
		MAX,
	};

	// コンストラクタ
	Skeleton(void);

	// デストラクタ
	~Skeleton(void)override;

	void Init(void) override;
	void Update(void) override;
	void Draw3D(void) override;

	void SetSide(ENEMY_SIDE side);

private:

	// 大きさ
	static constexpr VECTOR SCALE = { 0.8f,0.8f,0.8f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f, 0.0f,0.0f };

	// 座標
	static constexpr VECTOR LOOK_POS = { -3700.0f, 10.0f, 1393.0f };
	static constexpr VECTOR SCARE_POS = { -4000.0f, 10.0f, 1393.0f };
	// 特定のポイントからの反応距離
	static constexpr float TRIGGER_RANGE = 100.0f * 100.0f;

	STATE state_ = STATE::NONE;
	ENEMY_SIDE side_ = ENEMY_SIDE::RIGHT;

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeIdle(void);
	void ChangeLook(void);
	void ChangeScare(void);
	void ChangeEnd(void);

	// 状態別更新
	void UpdateIdle(void);
	void UpdateLook(void);
	void UpdateScare(void);
	void UpdateEnd(void);
};

