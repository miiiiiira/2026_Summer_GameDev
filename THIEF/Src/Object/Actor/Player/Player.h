#pragma once
#include <vector>
#include <DxLib.h>

#include "../ActorBase.h"
#include "Upgrade/Upgrade.h"

class Camera;

class Player : public ActorBase
{
public:

	// 通常時移動速度
	static constexpr float DEFAULT_SPEED = 7.0f;

	// ダッシュ時の移動速度
	static constexpr float DEFAULET_DASH_SPEED = 10.0f;

	// スタミナ
	static constexpr float DEFAULT_STAMINA = 10.0f;

	// スタミナ回復量
	static constexpr float RECOVERY_STAMINA = 0.1f;

	// スタミナ回復を行うまでの時間
	static constexpr int RECOVERY_STAMINA_WAIT_TIME = 1;

	// プレイヤーの状態種類
	enum class STATE
	{
		IDLE,
		RUN,
		CROUCHING,
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

	// プレイヤーのアップグレードの種類
	PLAYER_UPGRADE upgradeType_;

	// プレイヤーの状態の種類
	STATE state_;

	// ダッシュ速度
	float runSpeedMax_;

	// スタミナ
	float stamina_;
	float staminaMax_;

	// スタミナを回復させるまでの時間カウンタ
	int staminaCounter_;

	// ダッシュ処理
	float Run(void);
};
