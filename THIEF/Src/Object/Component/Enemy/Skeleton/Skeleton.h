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

	// 大きさ
	static constexpr VECTOR SCALE = { 0.8f,0.8f,0.8f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f, 0.0f,0.0f };

	// プレイヤー通過時に発動するイベント用座標
	// プレイヤーがこの座標に近づくと、敵が反応する
	static constexpr VECTOR LOOK_POS = { -3700.0f, 10.0f, 1393.0f };

	// プレイヤーがこの座標に近づくと、敵がプレイヤーを驚かす
	static constexpr VECTOR SCARE_POS = { -4000.0f, 10.0f, 1393.0f };

	// 特定のポイントからの反応距離
	static constexpr float TRIGGER_RANGE = 100.0f * 100.0f;

	// アニメーションの再生速度
	static constexpr float ANIM_SPEED = 0.3f;

	// 移動スピード
	static constexpr float MOVE_SPEED = 20.0f;

	// 予測判定距離
	static constexpr float FORWARD_COLLISION_CHECK_DISTANCE = 30.0f;

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

