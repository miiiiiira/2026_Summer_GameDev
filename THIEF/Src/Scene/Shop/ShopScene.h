#pragma once
#include "../SceneBase.h"

class ShopScene : public SceneBase
{
public:

	ShopScene(void);				// コンストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	int backImg_ = -1;	// 背景画像ハンドル
};

