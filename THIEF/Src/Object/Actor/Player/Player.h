#pragma once
#include <vector>
#include <DxLib.h>

#include "../ActorBase.h"
#include "Upgrade/Upgrade.h"

class Camera;
class Lantern;

class Player : public ActorBase
{
public:

	// 通常時移動速度
	static constexpr float DEFAULT_SPEED = 7.0f;

	// ダッシュ時の移動速度
	static constexpr float DASH_SPEED = 10.0f;

	// スライディング可能時間(秒数)
	static constexpr int SLIDING_INPUT_BUFFER_TIME = 2 * 60;

	// スタミナ
	static constexpr float DEFAULT_STAMINA = 10.0f;

	// スタミナ回復量
	static constexpr float RECOVERY_STAMINA = 0.05f;

	// スタミナ回復を行うまでの時間(秒数)
	static constexpr int RECOVERY_STAMINA_WAIT_TIME = 1 * 60;

	// プレイヤーの掴み距離
	static constexpr float DEFAULT_RENGE = 50.0f;

	// プレイヤーの状態種類
	enum class STATE
	{
		IDLE,
		MOVE,
		RUN,
		CROUCHING,
		SLIDING,
	};

	// コンストラクタ
	Player(Camera* camera);

	// デストラクタ
	~Player(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

	// プレイヤーの能力を上げる
	void Upgrade(PLAYER_UPGRADE type, float upNum);

	// 今現在の状態を渡す
	STATE GetNowState(void) { return state_; }
private:

	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

	// 移動処理
	void Move(void) override;

private:
	// カメラ
	Camera* camera_;

	// ランタンクラスのハンドル
	Lantern* lantern_;

	// プレイヤーのアップグレードの種類
	PLAYER_UPGRADE upgradeType_;

	// プレイヤーの状態の種類
	STATE state_;

	// 移動速度
	float moveSpeed_;
	float baseMoveSpeed_;

	// スライディング可能時間
	int slidingInputBufferTime;

	// スタミナ
	float stamina_;
	float staminaMax_;

	// スタミナを回復させるまでの時間カウンタ
	int staminaCounter_;

	// 掴み距離
	float range_;
	float rangeMAX_;

	// ダッシュ処理	(走るボタンを押されたら : デフォルト移動速度＋ダッシュ速度、
	//					   押されなかったら : デフォルト移動速度)
	void Run(void);

	void InputSliding(void);

	// スタミナ回復処理
	void HealStamina(void);

	// 掴み処理
	void Grasp(void);

	// デバッグ用の描画処理
	void DebugDraw();
};
