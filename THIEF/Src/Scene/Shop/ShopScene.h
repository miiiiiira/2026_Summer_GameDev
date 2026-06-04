#pragma once
#include "../SceneBase.h"
#include "../../Application.h"
class ShopScene :
    public SceneBase
{
public:

	ShopScene(void);				// コンストラクタ
	~ShopScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	// 終了ボタンの位置
	static constexpr float END_BUTTON_POS_X = Application::SCREEN_SIZE_X - 210.0f;
	static constexpr float END_BUTTON_POS_Y = Application::SCREEN_SIZE_Y - 50.0f;

	// 終了ボタンのサイズ
	static constexpr float COL_SIZE_X = 200.0f;
	static constexpr float COL_SIZE_Y = 45.0f;

	// 終了ボタン
	int endButtonImg_;
};

