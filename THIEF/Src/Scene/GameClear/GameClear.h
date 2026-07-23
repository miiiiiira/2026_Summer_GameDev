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
	
	static constexpr int LIMIT_Y = Application::SCREEN_SIZE_Y - IMAGE_SIZE_Y;

	int handle_;	// 画像ハンドル

	int waitTimer_;	// 待ち時間のカウンター

	int skipTimer_;	// スキップ用のカウンター

	int scrollY_;	// Y座標のスクロール値

	void ShaderInit(void);
};
