#pragma once
#include <vector>
#include "../EnemyBase.h"

class Statue : public EnemyBase
{
public:

	// 敵の状態
	enum class STATE
	{
		NONE,		// なし
		IDLE,		// 待機
		SURPRISE,	// 見つける
		CHASE,		// 追いかける
		ATTACK,		// 攻撃
	};

	// コンストラクタ
	Statue(void);

	// デストラクタ
	~Statue(void)override;

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void Draw3D(void) override;		// 3D描画

	// 特定のエリアの最大、最小値を設定
	void SetAreaPos(VECTOR minPos, VECTOR maxPos);

	// 追跡用の座標を設定
	void SetChasePos(VECTOR pos);

private:

	// エリアの最小値
	VECTOR minAreaPos_ = { -1.0f, -1.0f, -1.0f };

	// エリアの最大値
	VECTOR maxAreaPos_ = { -1.0f, -1.0f, -1.0f };

	// 追跡用の初期値座標
	VECTOR chasePos_ = { 0.0f, 0.0f, 0.0f };

	// 敵の状態
	STATE state_ = STATE::NONE;

	// SE再生からの経過時間タイマー
	float seTimer_ = 0.0f;

	// 状態遷移
	void ChangeState(STATE state);

	// 状態別変更
	void ChangeIdle(void);			// 待機
	void ChangeSurprise(void);		// 見つける
	void ChangeChase(void);			// 追いかける
	void ChangeAttack(void);		// 攻撃

	// 状態別更新
	void UpdateIdle(void);			// 待機
	void UpdateSurprise(void);		// 見つける
	void UpdateChase(void);			// 追いかける
	void UpdateAttack(void);		// 攻撃
};

