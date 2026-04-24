#pragma once

#include "../SceneBase.h"
#include <DxLib.h>


class Confirm;

class MainMenu : public SceneBase
{
public:

	enum class Menu
	{
		NONE,
		PLAY,
		OPTION,
		QUIT,
	};

	struct Item
	{
		Menu type;
		int graphHandle;
		int x, y;
		int sizeX, sizeY;
	};

	MainMenu(void);				// コンストラクタ
	~MainMenu(void) override;		// デストラクタ

	void Init(void)		override;	// 初期化
	void Load(void)		override;	// 読み込み
	void LoadEnd(void)	override;	// 読み込み後の処理
	void Update(void)	override;	// 更新
	void Draw(void)		override;	// 描画
	void Release(void)	override;	// 解放

private:

	static constexpr int TITLE_POS_X = 50;
	static constexpr int TITLE_POS_Y = 50;

	// 画像サイズ
	static constexpr int IMAGE_SIZE_X = 200;
	static constexpr int IMAGE_SIZE_Y = 50;

	// PLAY
	static constexpr int PLAY_POS_X = 80;
	static constexpr int PLAY_POS_Y = 250;

	// OPTION
	static constexpr int OPTION_POS_X = 80;
	static constexpr int OPTION_POS_Y = 330;

	// QUIT
	static constexpr int QUIT_POS_X = 80;
	static constexpr int QUIT_POS_Y = 550;

	void ChangeSelect(Menu menu);

	// メニュー選択時の処理
	void UpdatePlay(void);
	void UpdateOption(void);
	void UpdateQuit(void);

private:

	std::shared_ptr<Confirm> confirm_;

	int handle_;
	int frameImg_;						// フレーム画像
	std::vector<Item> menuButtons_;		// メニューボタンの情報を格納する配列

	Menu currentMenu_;					// 現在選択しているメニュー

};
