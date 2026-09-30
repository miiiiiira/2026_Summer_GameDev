#pragma once
#include <vector>
#include "../EnemyBase.h"

class Yeti : public EnemyBase
{
public:

	// 敵の状態
	enum class STATE
	{
		NONE,		// なし
		THINK,		// 考える
		IDLE,		// 待機
		PATROL,		// 巡回
		SURPRISE,	// 発見
		CHASE,		// 追跡
		ATTACK,		// 攻撃
	};

	// 敵のアニメーション
	enum class ANIM_TYPE
	{
		DEATH,
		DUCK,
		HIT_REACT,
		IDLE,
		JUMP,
		JUMP_IDLE,
		JUMP_LAND,
		NO,
		PUNCH,
		RUN,
		WALK,
		WAVE,
		WEAPON,
		YES,
		MAX,
	};

	// コンストラクタ
	Yeti(void);

	// デストラクタ
	~Yeti(void)override;

	void Init(void) override;		// 初期化
	void Update(void) override;		// 更新
	void Draw3D(void) override;		// 3D描画
	void Draw2D(void) override;		// 2D描画

private:

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };
	
	// 追跡処理を実行する間隔
	static constexpr float CHASE_INTERVAL = 0.5f;

	// ターゲットを見失うまでの猶予時間
	static constexpr float LOST_LIMIT_TIME = 6.0f;

	// アニメーションの再生速度
	static constexpr float ANIM_SPEED = 0.2f;

	// 巡回エリアの半径
	static constexpr float PATROL_RADIUS_DEFAULT = 1500.0f;

	// 視界（発見・追跡）の半径
	static constexpr float VIEW_RADIUS_DEFAULT = 1000.0f;

	// ウェイポイント到達判定距離
	static constexpr float WAYPOINT_ARRIVE_DIST = 50.0f;

	// 巡回移動速度
	static constexpr float SPEED_PATROL = 5.0f;

	// 追跡移動速度
	static constexpr float SPEED_CHASE = 10.0f;

	// 攻撃時移動速度
	static constexpr float ATTACK_SPEED_MOVE = 30.0f;

	// 攻撃時吹っ飛び力
	static constexpr float ATTACK_JUMP_POWER = 25.0f;

	// 攻撃時ダメージ力
	static constexpr float ATTACK_DAMAGE_POWER = 20.0f;

	// 待機時間
	static constexpr float STEP_TIME_IDLE = 5.0f;

	// 発見時の驚き演出時間
	static constexpr float STEP_TIME_SURPRISE = 2.0f;

	// 巡回時の足音再生間隔（秒）
	static constexpr float SE_INTERVAL_PATROL = 0.5f;

	// 追跡時の足音再生間隔（秒）
	static constexpr float SE_INTERVAL_CHASE = 0.3f;

	// 視線チェックを行う上限の高低差
	static constexpr float HEIGHT_DIFF_LIMIT = 700.0f;

	// 視線レイキャストのカプセル半径
	static constexpr float RAY_RADIUS_CHASE = 40.0f;

	// 攻撃判定に入る距離
	static constexpr float ATTACK_RANGE = 300.0f;

	// プレイヤー最新ノードへの到達判定距離
	static constexpr float PLAYER_NODE_ARRIVE_DIST = 100.0f;

	// 咆哮SEの聴こえる範囲
	static constexpr float SE_RADIUS_YETI = 2000.0f;

	// 足音SEの聴こえる範囲
	static constexpr float SE_RADIUS_MOVE = 3500.0f;

	// 確率計算の全体値
	static constexpr int MAX_PERCENTAGE = 100;

	// 待機状態に遷移する確率
	static constexpr int IDLE_PROBABILITY = 10;

	// 移動判定の閾値
	static constexpr float MOVE_EPSILON = 0.001f;

private:

	// 敵の状態
	STATE state_;

	// 追跡インターバルの計測用タイマー
	float chaseTimer_;

	// SE再生からの経過時間タイマー
	float seTimer_;

	// 状態遷移
	void ChangeState(STATE state);

	// 状態別変更
	void ChangeThink(void);		// 考える
	void ChangeIdle(void);		// 待機
	void ChangePatrol(void);	// 巡回
	void ChangeSurprise(void);	// 発見
	void ChangeChase(void);		// 追跡
	void ChangeAttack(void);	// 攻撃

	// 状態別更新
	void UpdateThink(void);		// 考える
	void UpdateIdle(void);		// 待機
	void UpdatePatrol(void);	// 巡回
	void UpdateSurprise(void);	// 発見
	void UpdateChase(void);		// 追跡
	void UpdateAttack(void);	// 攻撃
};