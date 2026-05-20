#pragma once
#include "../EnemyBase.h"

class Yeti : public EnemyBase
{
public:

	// 大きさ
	static constexpr VECTOR SCALE = { 1.0f,1.0f,1.0f };

	// 向き
	static constexpr VECTOR DEFAULT_ANGLE = { 0.0f,0.0f,0.0f };

	// 座標
	static constexpr VECTOR DEFAULT_POS = { 0.0f,10.0f,500.0f };

	// コンストラクタ
	Yeti(void);

	// デストラクタ
	~Yeti(void)override;

	// 読み込み処理
	void Load(void) override;

	// 初期化
	void Init(void) override;
};