#include "../../../../Common/Manager/Audio/AudioManager.h"
#include "../../Transform/Transform.h"
#include "Skull.h"
#include "../../../../Common/Manager/EffectResManager/EffectResManager.h"

Skull::Skull(void)
{
}

Skull::~Skull(void)
{
}

void Skull::SetParam(void)
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

void Skull::Break(void)
{
	EffectResManager::GetInstance().PlayEffect(1.0f, { 0.0f,0.0f, 0.0f }, trans_->pos_, EffectResManager::TYPE::ITEM_BREAK_SKULL);
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_BREAK_3, &trans_->pos_);
}

void Skull::Damage(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_DAMAGE_4, &trans_->pos_);
}
