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
	// ‘å‚«‚³‰Šú‰»
	info_.scale_ = SCALE;
	MV1SetScale(info_.modelId_, info_.scale_);

	// Œü‚«‰Šú‰»
	info_.angle_ = DEFAULT_ANGLE;
	MV1SetRotationXYZ(info_.modelId_, info_.angle_);

	// ‹àŠz(ƒAƒCƒeƒ€‚ÌHP)‰Šú‰»
	info_.price_ = PRICE;

	// Šæä‚³‰Šú‰»
	info_.hardness_ = HARDNESS;

	// d‚³‰Šú‰»
	info_.weight_ = WEIGHT;

	// “–‚½‚è”»’è—p‚Ì”¼Œa‚ð‰Šú‰»
	info_.collisionRadiusX_ = COLLISION_RADIUS_X;
	info_.collisionRadiusY_ = COLLISION_RADIUS_Y;

	// ¶‘¶’†‚É‚·‚é
	info_.isAlive_ = true;
}

void Bottle::Break(void)
{
	// Žæ‚ÁŽè•t‚«•r”j‰óSE
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_BREAK_4, &trans_->pos_);
}

void Bottle::Damage(void)
{
	AudioManager::GetInstance()->PlaySE(SoundID::SE_ITEM_DAMAGE_2, &trans_->pos_);
}
