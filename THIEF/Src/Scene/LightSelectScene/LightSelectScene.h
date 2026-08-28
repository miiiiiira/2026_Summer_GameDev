#pragma once

#include "../SceneBase.h"
#include "../../Object/Component/Wisp/LightInfo.h"
#include "LightSelectInfo.h"
#include <map>

class LightSelectScene : public SceneBase
{
public:

	LightSelectScene(void);				// コンストラクタ

	void Init(void)		override;		// 初期化
	void Load(void)		override;		// 読み込み
	void LoadEnd(void)	override;		// 読み込み後の処理
	void Update(void)	override;		// 更新
	void Draw(void)		override;		// 描画
	void Release(void)	override;		// 解放

private:

	
	void SelectUpgrade(void);	// どの能力をアップグレードするか選択を行う

	// 選択処理
	void MouseSelect(void);	// マウス
	void PadSelect(void);	// パッド

	void ConfirmUpgrade(void);	// 決定処理
	
	void ChangeSelectType(LightSelectTypeTable::SELECT_TYPE type);	// 指定の選択種類に変更

private:

	// 画像ハンドル
	int selectLightColorTextImg_;										// 「カラーを選んでね」
	int BestTextImg_;													// 「Best」
	int selectTypeImg_[LightSelectTypeTable::SELECT_TYPE::MAX];			// 選択可能ボタン
	int selectTypeFrameImg_[LightSelectTypeTable::SELECT_TYPE::MAX];	// 選択時のフレーム

	std::map<LIGHT_TYPE,int> wispImgs_;	// ライトの種類とそれに対応した画像ハンドルをもつ

	LIGHT_TYPE lightType_;	// 使用中のライトの種類
	
	LightSelectTypeTable::SELECT_TYPE selectType_;	// マウスが現在選択している画像の種類
};
