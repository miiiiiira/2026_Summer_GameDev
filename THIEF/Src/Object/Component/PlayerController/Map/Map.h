#pragma once

#include "../../Component.h"
#include <vector>

class Transform;
class PlayerController;
class Item;

class Map : public Component
{
public:

	Map(void);					// コンストラクタ
	~Map(void)override;			// デストラクタ

	void Init(void)override;	// 初期化
	void Update(void)override;	// 更新
	void Draw2D(void)override;	// 2D描画
		
public:

	// アイテムたちのポインタを格納
	void SetItems(std::vector<Item*> items) { items_ = items; }
	
	// 描画するかを設定
	void SetIsDraw(bool flg);
	// 描画しているかを渡す
	bool GetIsDraw(void) { return isDraw_; }

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
	static constexpr int BIG_RAD = 15;		// 大
	static constexpr int MEDIUM_RAD = 10;	// 中
	static constexpr int SMALL_RAD = 5;		// 小

	// チュートリアル時
	// 地図画像のサイズ
	static constexpr int TUTORIAL_MAP_IMAGE_SIZE_X = 161;
	static constexpr int TUTORIAL_MAP_IMAGE_SIZE_Y = 1620;
	// 地図画像上でのプレイヤー座標
	static constexpr int PLAYER_SPAWN_POS_X = 80;
	static constexpr int PLAYER_SPAWN_POS_Y = 1536;

private:

	// ステージ上のアイテムたち
	std::vector<Item*> items_;	

private:

	// 画像ハンドル
	int mapImg_ = -1;		// 地図
	int playerImg_ = -1;	// プレイヤー

	// マップを表示するかしないか	true / 表示, false / 非表示
	bool isDraw_ = false;

	// 地図画像の位置
	int mapImgPosX_ = 0;
	int mapImgPosY_ = 0;
	
	// 地図画像の大きさ
	int mapImgSizeX = 0;
	int mapImgSizeY = 0;

	// 地図上でのプレイヤーのスポーン位置
	int playerSpawnPosX = 0;
	int playerSpawnPosY = 0;
};

