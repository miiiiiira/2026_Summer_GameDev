#pragma once
#include <vector>
#include "../EnemyBase.h"

class Statue : public EnemyBase
{
public:

	// 敵の状態
	enum class STATE
	{
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

	// 大きさ
	static constexpr VECTOR SCALE = { 2.5f,2.5f,2.5f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f, 50.0f,0.0f };

	// 座標
	static constexpr VECTOR DEFAULT_POS = { -7444.0f,10.0f,3570.0f };

	// 特定エリアの最小値
	static constexpr VECTOR MIN_AREA_POS = { -7690.0f, 1.0f, 3450.0f };

	// 特定エリアの最大値
	static constexpr VECTOR MAX_AREA_POS = { -5870.0f, 1110, 5920.0f };

	// 攻撃時移動速度
	static constexpr float ATTACK_SPEED_MOVE = 20.0f;

	// 攻撃時吹っ飛び力
	static constexpr float ATTACK_JUMP_POWER = 25.0f;

	// 攻撃時ダメージ力
	static constexpr float ATTACK_DAMAGE_POWER = 20.0f;

	// 発見時の驚き演出時間
	static constexpr float STEP_TIME_SURPRISE = 4.0f;

	// 攻撃時の待機時間
	static constexpr float STEP_TIME_ATTACK = 10.0f;

	// 追跡移動速度
	static constexpr float SPEED_CHASE = 5.0f;

	// アタックのY座標のオフセット
	static constexpr float ATTACK_POS_OFFSET_Y = 80.0f;

	// 視線位置のオフセット
	static constexpr float EYE_OFFSET_Y = 100.0f;

	// 画面内判定の高さ間隔
	static constexpr float VIEW_CHECK_STEP_Y = 100.0f;

	// 画面内判定後に高さを戻す量
	static constexpr float VIEW_CHECK_RETURN_Y = 350.0f;

	// レイの半径
	static constexpr float RAY_RADIUS = 30.0f;

	// 攻撃範囲
	static constexpr float ATTACK_RANGE = 200.0f;

	// 帰還先に到達したとみなす距離
	static constexpr float RETURN_ARRIVE_DISTANCE = 100.0f * 100.0f;

	// 移動しているかの判定閾値
	static constexpr float MOVE_EPSILON = 0.001f;

	// 足音SEの届く半径
	static constexpr float SE_RADIUS_STATUE = 3500.0f;

	// 足音SEの再生間隔
	static constexpr float SE_INTERVAL_CHASE = 1.0f;

private:

	// エリアの最小値
	VECTOR minAreaPos_ = { -1.0f, -1.0f, -1.0f };

	// エリアの最大値
	VECTOR maxAreaPos_ = { -1.0f, -1.0f, -1.0f };

	// 追跡用の初期値座標
	VECTOR chasePos_ = { 0.0f, 0.0f, 0.0f };

	// 敵の状態
	STATE state_;

	// SE再生からの経過時間タイマー
	float seTimer_;

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

