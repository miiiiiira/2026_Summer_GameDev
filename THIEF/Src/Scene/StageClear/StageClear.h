#pragma once

#include "../SceneBase.h"
#include "../../Application.h"
#include <vector>

class StageClear : public SceneBase
{
public:

	enum TYPE
	{
		NEXT_STAGE,
		RETURN_TITLE,
		NONE,
	};

	struct IMG_INFO
	{
		TYPE type;
		int graphHandle;
		int x, y;
		int sizeX, sizeY;
	};

	StageClear(void);				// コンストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

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

	// フレームのオフセット
	static constexpr int FRAME_OFFSET = 10;

	// 画像ハンドル
	int handle_;

	// ボタンの情報を格納する配列
	std::vector<IMG_INFO> buttons_;

	// アルファ値(ボタン表示に使用)
	int alpha_ = 0;

	// 現在選択しているメニュー
	TYPE currentType_;

	// 選択処理
	void SelectUpdate(void);

	// マウス選択
	void MouseSelect(void);

	// パッド選択
	void PadSelect(void);
};

