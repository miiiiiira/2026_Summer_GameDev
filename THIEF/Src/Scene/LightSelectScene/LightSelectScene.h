#pragma once

#include <map>

#include "../SceneBase.h"
#include "../../Object/Component/Wisp/LightInfo.h"
#include "LightSelectInfo.h"

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

	// 画像ハンドル
	int selectLightColorTextImg_ = -1;									// 「カラーを選んでね」
	int BestTextImg_ = -1;												// 「Best」
	int selectTypeImg_[LightSelectTypeTable::SELECT_TYPE::MAX];			// 選択可能ボタン
	int selectTypeFrameImg_[LightSelectTypeTable::SELECT_TYPE::MAX];	// 選択時のフレーム

	// ライトの種類とそれに対応した画像ハンドルをもつ
	std::map<LIGHT_TYPE, int> wispImgs_;

	// 使用中のライトの種類
	LIGHT_TYPE lightType_ = LIGHT_TYPE::COLOR_0;

	// マウスが現在選択している画像の種類
	LightSelectTypeTable::SELECT_TYPE selectType_
		= LightSelectTypeTable::SELECT_TYPE::NON;

private:

	// どの能力をアップグレードするか選択を行う
	void SelectUpgrade(void);

	// 選択処理
	void MouseSelect(void);	// マウス
	void PadSelect(void);	// パッド

	// 決定処理
	void ConfirmUpgrade(void);

	// 指定の選択種類に変更
	void ChangeSelectType(LightSelectTypeTable::SELECT_TYPE type) { selectType_ = type; }
};
