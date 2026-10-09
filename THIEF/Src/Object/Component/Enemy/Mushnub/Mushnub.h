#pragma once
#include<vector>
#include "../EnemyBase.h"

class Mushnub : public EnemyBase
{
public:

	// 敵の状態
	enum class STATE
	{
		NONE,		// 無し
		IDLE,		// 待機
		SURPRISE,	// 見つける
		CHASE,		// 追いかける
	};

	// 敵のアニメーション
	enum class ANIM_TYPE
	{
		FRONT,
		DANCE,
		DEATH,
		HIT_REACT,
		IDLE,
		JUMP,
		NO,
		WALK,
		YES,
		MAX,
	};

	// コンストラクタ
	Mushnub(void);

	// デストラクタ
	~Mushnub(void)override;

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void Draw3D(void) override;		// 3D描画
	void Draw2D(void) override;		// 2D描画

	// 特定のエリアの最大、最小値を設定
	void SetAreaPos(VECTOR minPos, VECTOR maxPos);

	// 追跡用の座標を設定
	void SetChasePos(VECTOR pos);

private:

	// 状態
	STATE state_ = STATE::NONE;

	// エリアの最小値
	VECTOR minAreaPos_ = {};

	// エリアの最大値
	VECTOR maxAreaPos_ = {};

	// 追跡用の初期値座標
	VECTOR chasePos_ = {};

	// 状態遷移
	void ChangeState(STATE state);

	// 状態別変更
	void ChangeIdle(void);			// 待機
	void ChangeSurprise(void);		// 見つける
	void ChangeChase(void);			// 追いかける

	// 状態別更新
	void UpdateIdle(void);			// 待機
	void UpdateSurprise(void);		// 見つける
	void UpdateChase(void);			// 追いかける
};