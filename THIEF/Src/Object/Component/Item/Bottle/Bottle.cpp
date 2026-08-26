#include "../../../../Manager/Audio/AudioManager.h"
#include "../../../../Manager/EffectResManager/EffectResManager.h"
#include "../../Transform/Transform.h"

#include "Bottle.h"

Bottle::Bottle(void)
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
	// 破壊エフェクトを再生
	EffectResManager::GetInstance().PlayEffect(1.0f, { 0.0f,0.0f, 0.0f }, trans_->pos_, EffectResManager::TYPE::ITEM_BREAK_BOTTLE);
	// 取っ手付き瓶破壊SE再生
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_BREAK_4, &trans_->pos_);
}

void Bottle::Damage(void)
{
	// 取っ手付き瓶ダメージSE再生
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_DAMAGE_2, &trans_->pos_);
}
