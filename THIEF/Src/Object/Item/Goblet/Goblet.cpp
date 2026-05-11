#include<DxLib.h>
#include "Goblet.h"

Goblet::Goblet(void)
{
}

Goblet::~Goblet(void)
{
}

void Goblet::Load(void)
{
	info_.modelId_ = MV1LoadModel("Data/item/Goblet");
}

void Goblet::Init(void)
{
	info_.scale_ = SCALE;
	MV1SetScale(info_.modelId_, info_.scale_);

	info_.angle_ = DEFAULT_ANGLE;
	MV1SetRotationXYZ(info_.modelId_, info_.angle_);

	info_.pos_ = DEFAULT_POS;
	MV1SetPosition(info_.modelId_, info_.pos_);

	info_.itemTag_ = ITEM_TAG::GOBLET;

	info_.money_ = MONEY;

	info_.isGravity_ = true;

	info_.isAlive_ = true;
}
