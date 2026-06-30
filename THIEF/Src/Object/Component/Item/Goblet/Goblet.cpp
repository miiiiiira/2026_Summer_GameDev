#include<DxLib.h>
#include "Goblet.h"

#include "../../../../Common/Manager/Audio/AudioManager.h"
#include "../../Transform/Transform.h"

Goblet::Goblet()
	:Item()
{
}

Goblet::~Goblet(void)
{
}

void Goblet::SetParam(void)
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

	// アイテムの大きさ設定
	info_.size_ = ITEM_SIZE::SMALL;

	// 当たり判定用の半径を初期化
	info_.collisionRadiusX_ = COLLISION_RADIUS_X;
	info_.collisionRadiusY_ = COLLISION_RADIUS_Y;

	// 生存中にする
	info_.isAlive_ = true;
}

void Goblet::Break(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_BREAK_1, &trans_->pos_);
}

void Goblet::Damage(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_DAMAGE_1, &trans_->pos_);
}
