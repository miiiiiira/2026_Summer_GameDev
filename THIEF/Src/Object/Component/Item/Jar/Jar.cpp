#include<DxLib.h>
#include "Jar.h"
#include "../../../../Common/Manager/Audio/AudioManager.h"

Jar::Jar(void)
{
}

Jar::~Jar(void)
{
}

void Jar::SetParam(void)
{
	// 大きさ初期化
	info_.scale_ = SCALE;
	MV1SetScale(info_.modelId_, info_.scale_);

	// 向き初期化
	info_.angle_ = DEFAULT_ANGLE;
	MV1SetRotationXYZ(info_.modelId_, info_.angle_);

	// 金額(アイテムのHP)初期化
	info_.price_ = PRICE;

	// 頑丈さ初期化
	info_.hardness_ = HARDNESS;

	// 重さ初期化
	info_.weight_ = WEIGHT;

	// 当たり判定用の半径を初期化
	info_.collisionRadiusX_ = COLLISION_RADIUS_X;
	info_.collisionRadiusY_ = COLLISION_RADIUS_Y;

	// 生存中にする
	info_.isAlive_ = true;
}

void Jar::Break(void)
{
	// TODO 食べ物入ってるジャー破壊SE
}
