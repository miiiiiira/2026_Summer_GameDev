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

	// アイテムたちのポインタを格納
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
	
	// 地図画像の大きさ
	int mapImgSizeX;
	int mapImgSizeY;

	// 地図上でのプレイヤーのスポーン位置
	int playerSpawnPosX;
	int playerSpawnPosY;

	// 地図画像
	int mapImg_;

	// プレイヤー画像
	int playerImg_;
};

