#include<DxLib.h>
#include "Amphora.h"
#include "../../../../Common/Manager/Audio/AudioManager.h"
#include "../../Transform/Transform.h"
#include "../../../../Common/Manager/EffectResManager/EffectResManager.h"

Amphora::Amphora()
{
}

Amphora::~Amphora(void)
{
}

void Amphora::SetParam(void)
{
	// ‘å‚«‚³‰Šú‰»
	info_.scale_ = SCALE;
	MV1SetScale(info_.modelId_, info_.scale_);

	// ‹àŠz(ƒAƒCƒeƒ€‚ÌHP)‰Šú‰»
	info_.price_ = PRICE;

	// Šæä‚³‰Šú‰»
	info_.hardness_ = HARDNESS;

	// d‚³‰Šú‰»
	info_.weight_ = WEIGHT;

	// ƒAƒCƒeƒ€‚Ì‘å‚«‚³Ý’è
	info_.size_ = ITEM_SIZE::BIG;

	// “–‚½‚è”»’è—p‚Ì”¼Œa‚ð‰Šú‰»
	info_.collisionRadiusX_ = COLLISION_RADIUS_X;
	info_.collisionRadiusY_ = COLLISION_RADIUS_Y;

	// ¶‘¶’†‚É‚·‚é
	info_.isAlive_ = true;
}

void Amphora::Break(void)
{
	// ‘å‚«‚¢•r”j‰óSE
	EffectResManager::GetInstance().PlayEffect(1.0f, { 0.0f,0.0f, 0.0f }, trans_->pos_, EffectResManager::TYPE::ITEM_BREAK_AMPHORA);
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_BREAK_6, &trans_->pos_);
}

void Amphora::Damage(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_DAMAGE_2, &trans_->pos_);
}
