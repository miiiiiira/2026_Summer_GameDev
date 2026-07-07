#include <DxLib.h>
#include "../../Item/Item.h"
#include "../../../Object.h"
#include "../PlayerController.h"
#include "../../Transform/Transform.h"
#include "../../../../Application.h"
#include "../../../../Common/CameraUtility/CameraUtility.h"
#include "../../../../Scene/SceneManager.h"
#include "MapInfo.h"
#include "Map.h"

Map::Map(void)
{
	// ステージ情報を取ってきて初期化処理を行う
	auto stageNum = SceneManager::GetInstance()->GetCurrentStage();
	auto mapData = MapTable::Table.find(stageNum);

	// 地図画像の読み込み
	mapImg_ = LoadGraph(mapData->second.path.c_str());

	mapImgSizeX = mapData->second.mapImgSizeX;
	mapImgSizeY = mapData->second.mapImgSizeY;

	playerSpawnPosX = mapData->second.playerSpawnPosX;
	playerSpawnPosY = mapData->second.playerSpawnPosY;

	// マップ画像の位置初期化
	mapImgPosX_ = MAP_CENTER_POS_X + (mapImgSizeX / 2 - playerSpawnPosX);
	mapImgPosY_ = MAP_CENTER_POS_Y + (mapImgSizeY / 2 - playerSpawnPosY);

	// プレイヤー画像の読み込み
	playerImg_ = LoadGraph("Data/Image/GameScene/MapPlayer.png");
}

Map::~Map(void)
{
	// 画像ハンドル解放
	DeleteGraph(mapImg_);
	DeleteGraph(playerImg_);

	items_.clear();
}

void Map::Init(void)
{
	isDraw_ = false;
}

void Map::Update(void)
{
	// 描画フラグが立っていなかったら処理を行わない
	if (!isDraw_)return;

	// プレイヤーの3D座標を2D座標に変換して移動分マップの位置を動かす
	auto* player = owner_->GetComponent<PlayerController>();
	VECTOR plaPos = player->GetTransform()->pos_;
	mapImgPosX_ = playerSpawnPosX + (plaPos.x * 0.1f);
	mapImgPosY_ = playerSpawnPosY - (plaPos.z * 0.1f);

}

void Map::Draw2D(void)
{
	// 描画フラグが立っていなかったら処理を行わない
	if (!isDraw_)return;

	// アルファ値をかける
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);

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

	// カメラのY軸回転取得
	float angle = -CameraUtility::GetCameraAngle().y;

	// マップ自体の描画
	DrawRotaGraph2(
		MAP_CENTER_POS_X,
		MAP_CENTER_POS_Y,
		mapImgPosX_,
		mapImgPosY_,
		1.0,
		// カメラの回転と逆方向にマップを回すため
		angle,
		mapImg_,
		true);

	// プレイヤーの位置は必ずマップの中心
	DrawCircle(MAP_CENTER_POS_X, MAP_CENTER_POS_Y,3,0xffffff);
	//DrawRotaGraphF(MAP_CENTER_POS_X, MAP_CENTER_POS_Y, 1.0, 0.0, playerImg_, true);

	int size = 0;
	// 見つけたアイテムの数分回す
	for (Item* item : items_)
	{
		// アイテムが壊れているなら次の処理へ
		if (!item->GetInfo().isAlive_)continue;

		// アイテムがまだ見つけられていなかったら次の処理へ
		if (!item->GetInfo().isFound_)continue;

		// プレイヤーの3D座標を2D座標に変換して移動分マップの位置を動かす
		auto* player = owner_->GetComponent<PlayerController>();
		VECTOR plaPos = player->GetTransform()->pos_;
		// 実際のアイテムの座標とプレイヤーの座標でスケーリング(中心からの相対座標)
		float localX = (item->GetTransform()->pos_.x - plaPos.x) * 0.1f;
		float localZ = (-item->GetTransform()->pos_.z + plaPos.z) * 0.1f;

		// 2次元の回転行列を適用
		float rotatedX = localX * cosf(angle) - localZ * sinf(angle);
		float rotatedY = localX * sinf(angle) + localZ * cosf(angle);

		float posX = MAP_CENTER_POS_X + rotatedX;
		float posY = MAP_CENTER_POS_Y + rotatedY;
		// プレイヤーのスポーン位置から実際のアイテムの座標分をスケーリングして足してあげる
		switch (item->GetInfo().size_)
		{
		case ITEM_SIZE::BIG:
			size = BIG_RAD;
			break;
		case ITEM_SIZE::MEDIUM:
			size = MEDIUM_RAD;
			break;
		case ITEM_SIZE::SMALL:
			size = SMALL_RAD;
			break;
		default:
			break;
		}

		// サークルを表示
		DrawCircle(posX,posY,size,0xffff00);
	}
	
	// 描画の範囲制限解除
	SetDrawArea(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	// 通常描画に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

}

void Map::SetItems(std::vector<Item*> items)
{
	// アイテムたちのポインタを格納
	items_ = items;
}

void Map::SetIsDraw(bool flg)
{
	// 描画するかを設定
	isDraw_ = flg;
}

bool Map::GetIsDraw(void)
{
	return isDraw_;
}