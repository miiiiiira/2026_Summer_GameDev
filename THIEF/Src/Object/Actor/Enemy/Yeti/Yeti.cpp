#include "Yeti.h"
#include "../../../../Application.h"

Yeti::Yeti(void)
{
}

Yeti::~Yeti(void)
{
}

void Yeti::Load(void)
{
	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Enemy/Yeti.mv1").c_str());
}

void Yeti::Init(void)
{
	scale_ = SCALE;
	MV1SetScale(modelId_, scale_);

	angle_ = DEFAULT_ANGLE;
	angle_ = DEFAULT_ANGLE;
	MV1SetRotationXYZ(modelId_, angle_);

	pos_ = DEFAULT_POS;
	MV1SetPosition(modelId_, pos_);
}
