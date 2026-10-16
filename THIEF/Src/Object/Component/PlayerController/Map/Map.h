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
	int mapImgSizeX_ = 0;
	int mapImgSizeY_ = 0;

	// 地図上でのプレイヤーのスポーン位置
	int playerSpawnPosX_ = 0;
	int playerSpawnPosY_ = 0;
};

