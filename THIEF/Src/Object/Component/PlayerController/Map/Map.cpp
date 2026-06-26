#include <DxLib.h>
#include "../../Item/Item.h"
#include "Map.h"

Map::Map(PlayerController* player)
{
	player_ = player;
}

Map::~Map(void)
{
}

void Map::Load(void)
{
	// 地図画像の読み込み
	mapImg_ = LoadGraph("Data/Image/GameScene/Map.png");
	// プレイヤー画像の読み込み
	playerImg_ = LoadGraph("Data/Image/GameScene/MapPlayer.png");
}

void Map::Init(void)
{
}

void Map::Update(void)
{
	for (Item* item : foundItems_)
	{
		if (!item->GetInfo().isAlive_)
		{
			// TODO アイテムのポインタを削除させる
		}
	}
}

void Map::Draw(void)
{
	DrawGraph(0, 0, mapImg_, true);

	//// TODO プレイヤーの3D座標から2D座標変換して
	//float plaPosX, plaPosY;
	//DrawRotaGraphF(plaPosX, plaPosY, 1.0, 0.0, playerImg_, true);

	//int size = 0;
	//// 見つけたアイテムの数分回す
	//for (Item* item : foundItems_)
	//{
	//	// TODO 3D座標から2D座標変換して
	//	float posX, posY;
	//	switch (item->GetInfo().size_)
	//	{
	//	case ITEM_SIZE::BIG:
	//		size = BIG_RAD;
	//		break;
	//	case ITEM_SIZE::MEDIUM:
	//		size = MEDIUM_RAD;
	//		break;
	//	case ITEM_SIZE::SMALL:
	//		size = SMALL_RAD;
	//		break;
	//	default:
	//		break;
	//	}

	//	// サークルを表示
	//	DrawCircle(posX,posY,size,0xffff00);
	//}
}

void Map::Release(void)
{
	// 画像ハンドル解放
	DeleteGraph(mapImg_);
	DeleteGraph(playerImg_);

	foundItems_.clear();
}

void Map::AddFoundItem(Item* item)
{
	// 発見したアイテムを格納
	foundItems_.push_back(item);
}
