#pragma once
#include<vector>
#include "../EnemyBase.h"

class Mushnub : public EnemyBase
{
public:

	// 敵の状態
	enum class STATE
	{
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

	// 大きさ
	static constexpr VECTOR SCALE = { 0.3f,0.3f,0.3f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f, 0.0f,0.0f };

	// 座標
	static constexpr VECTOR DEFAULT_POS = { -1526.83f,10.0f,5500.0f };

	// 追跡用の座標
	static constexpr VECTOR CHASE_POS = { -1506.83f,10.0f,6513.62f };

	// 特定エリアの最小値
	static constexpr VECTOR MIN_AREA_POS = { -2150.0f, 1.0f, 5400.0f };

	// 特定エリアの最大値
	static constexpr VECTOR MAX_AREA_POS = { -560.0f, 700.0f, 7800.0f };

	// 視界（発見・追跡）の半径
	static constexpr float VIEW_RADIUS_DEFAULT = 1000.0f;

	// 攻撃時ダメージ力
	static constexpr float ATTACK_DAMAGE_POWER = 10.0f;

	// 待機時間
	static constexpr float STEP_TIME_IDLE = 5.0f;

	// 発見時の驚き演出時間
	static constexpr float STEP_TIME_SURPRISE = 2.0f;

	// 追跡移動速度
	static constexpr float SPEED_CHASE = 3.0f;

	// 元の場所へ戻り切ったと判定する閾値
	static constexpr float RETURN_THRESHOLD_DISTANCE_SQ = 100.0f * 100.0f;

	// プレイヤーへの距離を保つための閾値
	static constexpr float PLAYER_KEEP_DISTANCE_SQ = 200.0f * 200.0f;

	// アニメーションの再生速度
	static constexpr float ANIM_SPEED = 0.2f;

private:

	// 状態
	STATE state_;

	// エリアの最小値
	VECTOR minAreaPos_;

	// エリアの最大値
	VECTOR maxAreaPos_;

	// 追跡用の初期値座標
	VECTOR chasePos_;

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