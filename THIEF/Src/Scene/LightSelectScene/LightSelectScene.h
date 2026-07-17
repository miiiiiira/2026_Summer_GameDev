#pragma once

#include "../SceneBase.h"
#include <map>
#include "../../Object/Component/Wisp/LightInfo.h"

class LightSelectScene : public SceneBase
{
public:

	LightSelectScene(void);				// コンストラクタ
	~LightSelectScene(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	// ライトの種類とそれに対応した画像ハンドルをもつ
	std::map<LIGHT_TYPE,int> wispImgs_;

	// 選択時のフレーム画像
	int wispSelectFrameImg_;

	// ゲームスタート画像
	int gameStartImg_;

	// 選択矢印画像
	int selectArrowImg_;
};

