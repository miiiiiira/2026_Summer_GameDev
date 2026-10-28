#pragma once

#include "../SceneBase.h"
#include "../../Application.h"

class GameClear : public SceneBase
{
public:

	GameClear(void);				// コンストラクタ
	~GameClear(void) override;		// デストラクタ
	
	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	enum class STATE
	{
		NONE,
		WAIT_SHADER,
		SCROLL,
		WAIT_END,
	};

	// 現在のステート
	STATE state_ = STATE::NONE;

	// 画像ハンドル
	int handle_ = -1;

	// スキップ画像
	int skipHandle_ = -1;

	// スキップゲージ
	int skipGauge_ = -1;

	// 待ち時間のカウンター
	int waitTimer_ = 0;

	// スキップ用のカウンター
	float skipTimer_ = 0.0f;

	// Y座標のスクロール値
	int scrollY_ = 0;

	// アルファ値
	int alpha_ = 0;

	void ShaderInit(void);
};
