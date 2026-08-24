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
		WAIT_SHADER,
		SCROLL,
		WAIT_END,
		MAX,
	};

	STATE state_;	// 現在のステート

	// 画像のYサイズ
	static constexpr int IMAGE_SIZE_Y = 3840;
	
	// エンドロールのY軸リミット値
	static constexpr int LIMIT_Y = Application::SCREEN_SIZE_Y - IMAGE_SIZE_Y;

	// スキップ画像のサイズ
	static constexpr int SKIP_IMAGE_SIZE = 120;

	// スキップ時間の最大値
	static constexpr float MAX_SKIP_TIME = 120.0f;

	// 最大アルファ値
	static constexpr int ALPHA_MAX = 255;

	// アルファ値下げる量
	static constexpr int SUB_ALPHA = 10;


	int handle_;	// 画像ハンドル

	int skipHandle_;	// スキップ画像

	int skipGauge_;		// スキップゲージ

	int waitTimer_;	// 待ち時間のカウンター

	float skipTimer_;	// スキップ用のカウンター

	int scrollY_;	// Y座標のスクロール値

	int alpha_;	// アルファ値

	void ShaderInit(void);
};
