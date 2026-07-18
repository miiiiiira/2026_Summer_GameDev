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

	enum ARROW_TYPE
	{
		RIGHT,
		LEFT,

		MAX
	};

	// ライトの種類とそれに対応した画像ハンドルをもつ
	std::map<LIGHT_TYPE,int> wispImgs_;

	// 使用中のライトの種類
	LIGHT_TYPE lightType_;

	// 選択矢印画像
	int selectArrowImg_[ARROW_TYPE::MAX];

	// ゲームスタート画像
	int gameStartImg_ = -1;

};

