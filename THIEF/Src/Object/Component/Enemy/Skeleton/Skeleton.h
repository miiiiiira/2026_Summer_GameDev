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

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void Draw3D(void) override;		// 描画

	// 敵の向き設定
	void SetSide(ENEMY_SIDE side);

private:

	// 敵の状態
	STATE state_ = STATE::NONE;

	// 敵の向き
	ENEMY_SIDE side_ = ENEMY_SIDE::RIGHT;

	// 状態遷移
	void ChangeState(STATE state);

	// 状態別変更
	void ChangeIdle(void);		// 待機
	void ChangeLook(void);		// 見つめる
	void ChangeScare(void);		// 怖がらせる
	void ChangeEnd(void);		// 終了

	// 状態別更新
	void UpdateIdle(void);		// 待機
	void UpdateLook(void);		// 見つめる
	void UpdateScare(void);		// 怖がらせる
	void UpdateEnd(void);		// 終了
};

