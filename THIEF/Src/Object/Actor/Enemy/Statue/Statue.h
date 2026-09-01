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
		END,		// 終了
	};

	// コンストラクタ
	Statue(void);

	// デストラクタ
	~Statue(void)override;

	void Init(void) override;
	void Update(void) override;
	void Draw3D(void) override;

	// 特定のエリアの最大、最小値を設定
	void SetAreaPos(VECTOR minPos, VECTOR maxPos) { minAreaPos_ = minPos; maxAreaPos_ = maxPos; }

	void SetChasePos(VECTOR pos) { chasePos_ = pos; }
private:

	// 大きさ
	static constexpr VECTOR SCALE = { 2.5f,2.5f,2.5f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f, 50.0f,0.0f };

	// 座標
	static constexpr VECTOR DEFAULT_POS = { -7444.0f,10.0f,3570.0f };

	// 特定エリアの最大、最小値
	static constexpr VECTOR MIN_AREA_POS = { -7690.0f, 1.0f, 3450.0f };
	static constexpr VECTOR MAX_AREA_POS = { -5870.0f, 1110, 5920.0f };

	// エリアの最大値、最小値
	VECTOR minAreaPos_ = { -1.0f, -1.0f, -1.0f };
	VECTOR maxAreaPos_ = { -1.0f, -1.0f, -1.0f };
	VECTOR chasePos_ = { 0.0f, 0.0f, 0.0f };

	STATE state_;

	float seTimer_;		// SEが鳴り終わっているか

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeIdle(void);
	void ChangeSurprise(void);
	void ChangeChase(void);
	void ChangeAttack(void);
	void ChangeEnd(void);

	// 状態別更新
	void UpdateIdle(void);
	void UpdateSurprise(void);
	void UpdateChase(void);
	void UpdateAttack(void);
	void UpdateEnd(void);
};

