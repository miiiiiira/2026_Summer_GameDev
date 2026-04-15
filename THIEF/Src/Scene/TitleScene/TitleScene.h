#pragma once
#include "../SceneBase.h"

class TitleScene : public SceneBase
{

public:
	
	TitleScene(void);				// コンストラクタ
	~TitleScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:
	static constexpr int BUTTON_POS_X = 512;
	static constexpr int BUTTON_POS_Y = 520;

private:

	int handle_;
	int buttonHandle_;				// PushAnyButton画像のハンドル
};
