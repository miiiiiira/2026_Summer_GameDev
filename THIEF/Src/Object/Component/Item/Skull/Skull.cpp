#include "../../../../Common/Manager/Audio/AudioManager.h"
#include "../../Transform/Transform.h"
#include "Skull.h"

Skull::Skull(void)
{
}

Skull::~Skull(void)
{
}

void Skull::SetParam(void)
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
	info_.size_ = ITEM_SIZE::MEDIUM;

	// “–‚½‚è”»’è—p‚Ì”¼Œa‚ð‰Šú‰»
	info_.collisionRadiusX_ = COLLISION_RADIUS_X;
	info_.collisionRadiusY_ = COLLISION_RADIUS_Y;

	// ¶‘¶’†‚É‚·‚é
	info_.isAlive_ = true;
}

void Skull::Break(void)
{
	// TODO “ªŠWœ”j‰óSE
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_BREAK_3, &trans_->pos_);
}

void Skull::Damage(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_DAMAGE_4, &trans_->pos_);
}
