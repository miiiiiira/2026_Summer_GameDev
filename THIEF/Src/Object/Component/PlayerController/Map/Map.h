#pragma once

#include "../../Component.h"
#include <vector>

class Transform;
class PlayerController;
class Item;

class Map : public Component
{
public:
	Map(void);
	~Map(void)override;

	void Init(void)override;
	void Update(void)override;
	void Draw2D(void)override;

	// 発見されたアイテムを格納する
	void SetItems(std::vector<Item*> items);

	// 描画するかを設定
	void SetIsDraw(bool flg);
	bool GetIsDraw(void);

private:

	// マップの描画範囲
	static constexpr int MAP_DRAW_AREA_START_X = Application::SCREEN_SIZE_X / 5;
	static constexpr int MAP_DRAW_AREA_START_Y = Application::SCREEN_SIZE_Y / 7;
	static constexpr int MAP_DRAW_AREA_END_X = Application::SCREEN_SIZE_X - Application::SCREEN_SIZE_X / 5;
	static constexpr int MAP_DRAW_AREA_END_Y = Application::SCREEN_SIZE_Y - Application::SCREEN_SIZE_Y / 7;

	// マップの中心位置
	static constexpr int MAP_CENTER_POS_X = Application::SCREEN_SIZE_X / 2;
	static constexpr int MAP_CENTER_POS_Y = Application::SCREEN_SIZE_Y / 2;

	// マップ画像サイズ
	static constexpr int MAP_IMG_SIZE_X = 1024;
	static constexpr int MAP_IMG_SIZE_Y = 843;

	// マップ上のプレイヤーの初期位置
	static constexpr int PLAYER_SPAWN_POS_X = 982;
	static constexpr int PLAYER_SPAWN_POS_Y = 233;

	// マップ画像の初期位置
	static constexpr int MAP_IMG_DEFAULT_POS_X =
		MAP_CENTER_POS_X + (MAP_IMG_SIZE_X / 2 - PLAYER_SPAWN_POS_X);
	static constexpr int MAP_IMG_DEFAULT_POS_Y =
		MAP_CENTER_POS_Y + (MAP_IMG_SIZE_Y / 2 - PLAYER_SPAWN_POS_Y);

	// アイテムのサイズによって表示する大きさを変更
	static constexpr int BIG_RAD = 15;
	static constexpr int MEDIUM_RAD = 10;
	static constexpr int SMALL_RAD = 5;

	// マップを表示するかしないカ
	bool isDraw_;

	// ステージ上のアイテムたち
	std::vector<Item*> items_;

	// 地図画像の位置
	int mapImgPosX_;
	int mapImgPosY_;

	// 地図画像
	int mapImg_;

	// プレイヤー画像
	int playerImg_;
};

