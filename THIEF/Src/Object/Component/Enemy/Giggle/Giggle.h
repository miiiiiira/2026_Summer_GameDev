#pragma once
#include "../EnemyBase.h"
class Giggle : public EnemyBase
{
public:

	// 敵の状態
	enum class STATE
	{
		NONE,		// なし
		IDLE,		// 待機
		THINK,		// 考える
		GIGGLING,	// 笑う
	};

	// コンストラクタ
	Giggle(void);

	// デストラクタ
	~Giggle(void)override;

	// 初期化
	void Init(void) override;

	void Update(void)override;
private:

	STATE state_;

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeIdle(void);
	void ChangeThink(void);
	void ChangeGiggling(void);

	// 状態別更新
	void UpdateIdle(void);
	void UpdateThink(void);
	void UpdateGiggling(void);
};

