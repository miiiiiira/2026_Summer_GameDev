#pragma once

#include "../SceneBase.h"
#include "../../Object/Component/Wisp/LightInfo.h"
#include "LightSelectInfo.h"
#include <map>

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

	// 「カラーを選んでね」の画像
	int selectLightColorTextImg_;

	// 「Best」の画像
	int BestTextImg_;

	// ライトの種類とそれに対応した画像ハンドルをもつ
	std::map<LIGHT_TYPE,int> wispImgs_;

	// 使用中のライトの種類
	LIGHT_TYPE lightType_;

	// マウス選択できる画像のハンドル
	int mouseSelectTypeImg_[MouseSelectTypeTable::MOUSE_SELECT_TYPE::MAX];
	// マウス選択できる画像のフレーム画像ハンドル
	int mouseSelectTypeFrameImg_[MouseSelectTypeTable::MOUSE_SELECT_TYPE::MAX];

	// マウスが現在選択している画像の種類
	MouseSelectTypeTable::MOUSE_SELECT_TYPE mouseSelectType_;

	// どの能力をアップグレードするか選択を行う
	void SelectUpgrade(void);

	// マウスの選択処理
	void MouseSelect(void);

	// パッドの選択処理
	void PadSelect(void);

	// 決定処理
	void ConfirmUpgrade(void);

	// 変更処理
	void ChangeMouseSelect(MouseSelectTypeTable::MOUSE_SELECT_TYPE type);
};
