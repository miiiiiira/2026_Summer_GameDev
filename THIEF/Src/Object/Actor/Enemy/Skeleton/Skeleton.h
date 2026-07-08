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

	enum class SIDE
	{
		RIGHT,
		LEFT,
	};

	// コンストラクタ
	Skeleton(int modelId);

	// デストラクタ
	~Skeleton(void)override;

	// 初期化
	void OnInitialize(void) override;
	// 読み込み処理
	void Load(void) override;
	void Update(void)override;
	void Draw(void)override;

	void SetSide(SIDE side);

private:

	// 大きさ
	static constexpr VECTOR SCALE = { 0.8f,0.8f,0.8f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f, 0.0f,0.0f };

	// 座標
	static constexpr VECTOR LOOK_POS = { -3962.0f, 10.0f, 1393.0f };
	static constexpr VECTOR SCARE_POS = { -4687.0f, 10.0f, 1393.0f };
	// 特定のポイントからの反応距離
	static constexpr float TRIGGER_RANGE = 100.0f * 100.0f;


	int baseModelId_;

	STATE state_;
	SIDE side_;

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

