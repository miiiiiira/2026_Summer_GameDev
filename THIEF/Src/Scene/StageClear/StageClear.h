#pragma once

#include "../SceneBase.h"
#include "../../Application.h"
#include <vector>

class StageClear : public SceneBase
{
public:

	// 現在選択しているボタンの種類
	enum TYPE
	{
		NEXT_STAGE,
		RETURN_TITLE,
		NONE,
	};

	// 画像の情報
	struct IMG_INFO
	{
		TYPE type;			// 現在選択しているボタンの種類
		int graphHandle;	// ボタンの画像ハンドル
		int x, y;			// 座標
		int sizeX, sizeY;	// 大きさ
	};

public:

	StageClear(void);				// コンストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	void SelectUpdate(void);	// 選択処理

	void MouseSelect(void);		// マウス選択

	void PadSelect(void);		// パッド選択

private:

	// NEXT_STAGE画像サイズ
	static constexpr int NEXT_STAGE_SIZE_X = 295;
	static constexpr int NEXT_STAGE_SIZE_Y = 33;

	// RETRY
	static constexpr int RETRY_POS_X = Application::SCREEN_SIZE_X / 2 - NEXT_STAGE_SIZE_X / 2;
	static constexpr int RETRY_POS_Y = 485;

	// RETURN_TITLE画像サイズ
	static constexpr int RETURN_TITLE_SIZE_X = 152;
	static constexpr int RETURN_TITLE_SIZE_Y = 32;

	// RETURN_TITLE
	static constexpr int RETURN_TITLE_POS_X = Application::SCREEN_SIZE_X / 2 - RETURN_TITLE_SIZE_X / 2;
	static constexpr int RETURN_TITLE_POS_Y = 550;

	static constexpr int FRAME_OFFSET = 10;	// フレームのオフセット

private:

	int handle_ = -1;	// 画像ハンドル
	
	std::vector<IMG_INFO> buttons_;	// ボタンの情報を格納する配列
	
	int alpha_ = 0;	// アルファ値(ボタン表示に使用)
	
	TYPE currentType_;	// 現在選択している種類
};

