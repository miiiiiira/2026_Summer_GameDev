#pragma once
#include<vector>
#include "../EnemyBase.h"

class Mushnub : public EnemyBase
{
	// 敵の状態
	enum class STATE
	{
		NONE,		// なし
		IDLE,		// 待機
		SURPRISE,	// 見つける
		CHASE,		// 追いかける
		HIT_REACT,	// ダメージ
		DEAD,		// 死
		END,		// 終了
	};

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

	// 初期化
	void Init(PlayerController* player, int id = -1) override;
	// 読み込み処理
	void Load(void) override;
	void Update(void)override;
	void Draw(void) override;

	STATE GetState(void);

private:

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };

	// 座標
	static constexpr VECTOR DEFAULT_POS = { -5617.04f,10.0f,6573.71f };
	
	// 更新ステップ
	float step_;

	STATE state_;

	float viewRadius_;		// 視野用の半径

	bool isHit_;

	void SetMoveDirPatrol(void);

	// 移動処理
	void Move(void);

	// 追従用の線分かステージと当たっているかどうか
	bool CheckChaseLineCollision(VECTOR pPos, VECTOR ePos, float radius);

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeIdle(void);
	void ChangeSurprise(void);
	void ChangeChase(void);
	void ChangeHit(void);
	void ChangeDead(void);
	void ChangeEnd(void);

	// 状態別更新
	void UpdateIdle(void);
	void UpdateSurprise(void);
	void UpdateChase(void);
	void UpdateHit(void);
	void UpdateDead(void);
	void UpdateEnd(void);
};