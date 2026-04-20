#pragma once
#include <vector>
#include <DxLib.h>

#include "../ActorBase.h"
#include "Upgrade/Upgrade.h"

class Camera;

class Player : public ActorBase
{

public:
	// プレイヤーの状態種類
	enum class STATE
	{
		IDLE,
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
};
