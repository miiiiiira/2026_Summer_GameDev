#pragma once

#include "../SceneBase.h"
#include <DxLib.h>


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

private:

	int handle_;
	int continueImg_;
	int optionsImg_;
	int mainMenuImg_;
	int quitImg_;
	int frameImg_;

	Menu currentMenu_;
	Menu previousMenu_;
	VECTOR currentMenuPos_;
	VECTOR mousePos_;
	int mousePosX_;
	int mousePosY_;

};

