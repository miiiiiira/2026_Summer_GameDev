#pragma once

#include "../SceneBase.h"
#include <DxLib.h>
#include "../../Application.h"


class Confirm : public SceneBase
{
public:

	enum class RESULT
	{
		NONE,
		QUIT,
		MAIN_MENU,
	};

	enum class SELECT
	{
		NONE,
		YES,
		NO,
	};

	Confirm(void);					// コンストラクタ
	~Confirm(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

	void ChangeResult(RESULT result);

private:

	// 確認ウィンドウの位置とサイズ
	static constexpr int CONFIRM_SIZE_X = 800;
	static constexpr int CONFIRM_SIZE_Y = 500;
	static constexpr int CONFIRM_POS_X = (Application::SCREEN_SIZE_X - CONFIRM_SIZE_X) / 2;
	static constexpr int CONFIRM_POS_Y = (Application::SCREEN_SIZE_Y - CONFIRM_SIZE_Y) / 2;

	// 画像サイズ（YES, NO）
	static constexpr int IMAGE_SIZE_X = 150;
	static constexpr int IMAGE_SIZE_Y = 70;

	// YES
	static constexpr int YES_POS_X = (Application::SCREEN_SIZE_X - IMAGE_SIZE_X) / 2 - 150;
	static constexpr int YES_POS_Y = (Application::SCREEN_SIZE_Y - IMAGE_SIZE_Y) / 2 + 150;

	// NO
	static constexpr int NO_POS_X = (Application::SCREEN_SIZE_X - IMAGE_SIZE_X) / 2 + 150;
	static constexpr int NO_POS_Y = YES_POS_Y;

	void ChangeSelect(SELECT select);

	// メニュー選択時の処理
	void UpdateYes(void);
	void UpdateNo(void);


private:
	SELECT currentSelect_;		// 現在の選択肢
	VECTOR currentSelectPos_;	// 現在の選択肢の位置

	RESULT result_;				// 結果

	int confirmImg_;			// 画像ハンドル
	int quitImg_;				// QUIT画像ハンドル
	int mainMenuImg_;			// MAIN MENU画像ハンドル
	int yesImg_;				// YES画像ハンドル
	int noImg_;					// NO画像ハンドル
	int frameImg_;				// フレーム画像ハンドル
};

