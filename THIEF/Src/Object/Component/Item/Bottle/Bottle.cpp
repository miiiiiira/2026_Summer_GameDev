#include<DxLib.h>
#include "Bottle.h"
#include "../../../../Common/Manager/Audio/AudioManager.h"
#include "../../Transform/Transform.h"

Bottle::Bottle()
{
}

Bottle::~Bottle(void)
{
}

void Bottle::SetParam(void)
{
	// 大きさ初期化
	info_.scale_ = SCALE;
	MV1SetScale(info_.modelId_, info_.scale_);

	// 金額(アイテムのHP)初期化
	info_.price_ = PRICE;

	// 頑丈さ初期化
	info_.hardness_ = HARDNESS;

	// 重さ初期化
	info_.weight_ = WEIGHT;

	// アイテムの大きさ設定
	info_.size_ = ITEM_SIZE::MEDIUM;

	// 当たり判定用の半径を初期化
	info_.collisionRadiusX_ = COLLISION_RADIUS_X;
	info_.collisionRadiusY_ = COLLISION_RADIUS_Y;

	// 生存中にする
	info_.isAlive_ = true;
}

void Bottle::Break(void)
{
	// 取っ手付き瓶破壊SE
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_BREAK_4, &trans_->pos_);
}

void Bottle::Damage(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_DAMAGE_2, &trans_->pos_);
}
