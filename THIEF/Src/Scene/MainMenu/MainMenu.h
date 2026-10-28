#pragma once
#include <memory>
#include <vector>
#include "../SceneBase.h"

class Confirm;

class MainMenu : public SceneBase
{
public:

	enum class MENU
	{
		NONE,
		PLAY,
		OPTION,
		QUIT,
	};

	struct Item
	{
		MENU type = MENU::NONE;
		int graphHandle = -1;
		int x = 0;
		int y = 0;
		int sizeX = 0;
		int sizeY = 0;
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

	// 確認画面
	std::shared_ptr<Confirm> confirm_ = nullptr;

	// メインメニューのタイトルロゴ
	int handle_ = -1;

	// メニューボタンの情報を格納する配列
	std::vector<Item> menuButtons_ = {};

	// 現在選択しているメニュー
	MENU currentMenu_ = MENU::NONE;

	// 選択処理
	void UpdateSelect(void);

	// マウス選択
	void MouseSelect(void);

	// パッド選択
	void PadSelect(void);

	// メニューの選択を変更する
	void ChangeSelect(MENU menu);

	// メニュー選択時の処理
	void UpdatePlay(void);
	void UpdateOption(void);
	void UpdateQuit(void);
};
