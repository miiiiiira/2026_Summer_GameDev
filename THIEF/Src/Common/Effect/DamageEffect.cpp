#include <DxLib.h>
#include "../../Application.h"
#include "DamageEffect.h"

DamageEffect::DamageEffect(void)
	:alpha_(0)
{
}

DamageEffect::~DamageEffect(void)
{
}

void DamageEffect::Load(void)
{
}

void DamageEffect::Init(void)
{
	alpha_ = 0;
}

void DamageEffect::Update(void)
{
	if (alpha_ > 0)
	{
		alpha_--;
		if (alpha_ <= 0)
		{
			alpha_ = 0;
		}
	}
}

void DamageEffect::Draw(void)
{
	// アルファ値を設定
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, color_, true);

	// アルファ値を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void DamageEffect::SetEffect(int alpha, unsigned int color)
{
	// 指定されたアルファ値を設定
	alpha_ = alpha;

	// 指定されたカラー値を設定
	color_ = color;
}
