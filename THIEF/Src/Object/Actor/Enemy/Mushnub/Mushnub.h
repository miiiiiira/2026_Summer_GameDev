#pragma once
#include<vector>
#include "../EnemyBase.h"

class Mushnub : public EnemyBase
{
public:
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
	Mushnub(int modelId);

	// デストラクタ
	~Mushnub(void)override;

	// 初期化
	void OnInitialize(void) override;
	// 読み込み処理
	void Load(void) override;
	void Update(void)override;
	void Draw(void) override;

private:

	// 大きさ
	static constexpr VECTOR SCALE = { 0.3f,0.3f,0.3f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f, 0.0f,0.0f };

	// 座標
	static constexpr VECTOR CHASE_POS = { -1506.83f,10.0f,6513.62f };
	static constexpr VECTOR DEFAULT_POS = { -1526.83f,10.0f,5500.0f };

	// 特定エリアの最大、最小値
	static constexpr VECTOR MIN_AREA_POS = { -2150.0f, 10.0f, 5400.0f };
	static constexpr VECTOR MAX_AREA_POS = { -560.0f, 700.0f, 7800.0f };

	STATE state_;

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