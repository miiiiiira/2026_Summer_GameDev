#include <DxLib.h>
#include "../../Item/Item.h"
#include "../../../Object.h"
#include "../PlayerController.h"
#include "../../Transform/Transform.h"
#include "../../../../Application.h"
#include "Map.h"
#include "../../../../Common/CameraUtility/CameraUtility.h"

Map::Map(void)
{
	// 地図画像の読み込み
	mapImg_ = LoadGraph("Data/Image/GameScene/Map.png");
	// プレイヤー画像の読み込み
	playerImg_ = LoadGraph("Data/Image/GameScene/MapPlayer.png");
}

Map::~Map(void)
{
	// 画像ハンドル解放
	DeleteGraph(mapImg_);
	DeleteGraph(playerImg_);

	foundItems_.clear();
}

void Map::Init(void)
{
	// マップ画像の位置初期化
	mapImgPosX_ = MAP_IMG_DEFAULT_POS_X;
	mapImgPosY_ = MAP_IMG_DEFAULT_POS_Y;
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

	// プレイヤーの3D座標を2D座標に変換して移動分マップの位置を動かす
	auto* player = owner_->GetComponent<PlayerController>();
	VECTOR plaPos = player->GetTransform()->pos_;
	mapImgPosX_ = PLAYER_SPAWN_POS_X - (plaPos.z * 0.1f);
	mapImgPosY_ = PLAYER_SPAWN_POS_Y - (plaPos.x * 0.1f);

}

void Map::Draw2D(void)
{
	// アルファ値をかける
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);

	// 描画の範囲制限をかける
	SetDrawArea(
		MAP_DRAW_AREA_START_X,
		MAP_DRAW_AREA_START_Y,
		MAP_DRAW_AREA_END_X,
		MAP_DRAW_AREA_END_Y);

	// マップの背景
	DrawBox(
		MAP_DRAW_AREA_START_X,
		MAP_DRAW_AREA_START_Y,
		MAP_DRAW_AREA_END_X,
		MAP_DRAW_AREA_END_Y,
		0x000000, true);

	// マップ自体の描画
	DrawRotaGraph2(
		MAP_CENTER_POS_X,
		MAP_CENTER_POS_Y,
		mapImgPosX_,
		mapImgPosY_,
		1.0,
		// カメラの回転と逆方向にマップを回すため
		// マップ画像がステージと比べて-90°向きが回転しているため
		-CameraUtility::GetCameraAngle().y+(90 * DX_PI_F / 180.0f),
		mapImg_,
		true);

	// プレイヤーの位置は必ずマップの中心
	DrawCircle(MAP_CENTER_POS_X, MAP_CENTER_POS_Y,3,0xffffff);
	//DrawRotaGraphF(MAP_CENTER_POS_X, MAP_CENTER_POS_Y, 1.0, 0.0, playerImg_, true);

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
	
	// 描画の範囲制限解除
	SetDrawArea(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	// 通常描画に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void Map::AddFoundItem(Item* item)
{
	// 発見したアイテムを格納
	foundItems_.push_back(item);
}
