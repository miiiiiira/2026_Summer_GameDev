#pragma once

#include "../SceneBase.h"
#include <DxLib.h>

class Confirm;


class Pause : public SceneBase
{
public:

	enum class Menu
	{
		NONE,
		CONTINUE,
		OPTION,
		MAINMENU,
		QUIT,
	};

	Pause(void);				// コンストラクタ
	~Pause(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放


private:
	 
	static constexpr int PAUSE_POS_X = 50;
	static constexpr int PAUSE_POS_Y = 50;

	// 画像サイズ
	static constexpr int IMAGE_SIZE_X = 200;
	static constexpr int IMAGE_SIZE_Y = 50;

	// CONTINUE
	static constexpr int CONTINUE_POS_X = 80;
	static constexpr int CONTINUE_POS_Y = 200;

	// OPTION
	static constexpr int OPTION_POS_X = 80;
	static constexpr int OPTION_POS_Y = 280;

	// MAIN MENU
	static constexpr int MAINMENU_POS_X = 80;
	static constexpr int MAINMENU_POS_Y = 360;

	// QUIT
	static constexpr int QUIT_POS_X = 80;
	static constexpr int QUIT_POS_Y = 550;

	void ChangeSelect(Menu menu);

	// メニュー選択時の処理
	void UpdateContinue(void);
	void UpdateOption(void);
	void UpdateMainMenu(void);
	void UpdateQuit(void);

private:

	std::shared_ptr<Confirm> confirm_;

	int handle_;			// PAUSEの文字画像
	int continueImg_;		// CONTINUEの文字画像
	int optionsImg_;		// OPTIONの文字画像
	int mainMenuImg_;		// MAIN MENUの文字画像
	int quitImg_;			// QUITの文字画像
	int frameImg_;			// フレームの画像

	Menu currentMenu_;		// 現在選択しているメニュー
	VECTOR currentMenuPos_;	// 現在選択しているメニューの位置

};