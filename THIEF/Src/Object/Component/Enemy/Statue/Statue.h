#pragma once
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
	~Statue(void) override;

	// 更新
	void Update(void) override;

	// 3D描画
	void Draw3D(void) override;

	// 特定のエリアの最大、最小値を設定
	void SetAreaPos(VECTOR minPos, VECTOR maxPos);

	// 帰還先の座標を設定
	void SetHomePos(VECTOR pos);

	// タグを返す
	ENEMY_TAG GetTag(void) const override { return ENEMY_TAG::STATUE; }

protected:

	// 敵ごとのパラメータ設定
	void InitParam(void) override;

	// スケール・向きの設定
	void InitTransform(void) override;

	// 初期化の最後に行う個別処理
	void InitPost(void) override;

private:

	// エリアの最小値
	VECTOR minAreaPos_ = {};

	// エリアの最大値
	VECTOR maxAreaPos_ = {};

	// 帰還先の初期値座標
	VECTOR homePos_ = {};

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

	// プレイヤーの画面内に映っているか
	bool IsLookedByPlayer(void) const;

	// 帰還先へ戻る（到着して待機に戻ったらtrue）
	bool ReturnToHomePos(void);

	// 帰還先へまっすぐ向かう（到着して待機に戻ったらtrue）
	bool MoveStraightToHomePos(void);

	// エリア内でプレイヤーを追う
	bool UpdateChaseInArea(bool isLooked, bool isPlayerVisible);

	// エリア外で帰還先へ戻る
	bool UpdateReturn(bool isLooked, bool isPlayerVisible);

	// 足音SEの更新
	void UpdateFootstep(bool isLooked);
};