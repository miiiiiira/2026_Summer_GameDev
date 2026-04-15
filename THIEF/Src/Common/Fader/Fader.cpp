#include <DxLib.h>
#include "../../Application.h"
#include "Fader.h"

Fader* Fader::instance_ = nullptr;

Fader::Fader(void)
{
}

Fader::~Fader(void)
{
}

void Fader::Init(void)
{
	state_ = STATE::NONE;
	alpha_ = 0.0f;
	color_ = 0x000000;
}

void Fader::Update(void)
{

	switch (state_)
	{
	case STATE::NONE:
		return;

	case STATE::FADE_OUT:
		alpha_ += SPEED_ALPHA;
		if (alpha_ > 255)
		{
			// フェード終了
			Init();
		}
		break;

	case STATE::FADE_IN:
		alpha_ -= SPEED_ALPHA;
		if (alpha_ < 0)
		{
			// フェード終了
			Init();
		}
		break;

	default:
		return;
	}

}

void Fader::Draw(void)
{
	switch (state_)
	{
	case STATE::NONE:
		return;
	case STATE::FADE_OUT:
	case STATE::FADE_IN:
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(alpha_));
		DrawBox(
			0, 0,
			Application::SCREEN_SIZE_X,
			Application::SCREEN_SIZE_Y,
			color_, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		break;
	default:
		return;
	}
}

Fader::STATE Fader::GetState(void)
{
	return state_;
}

void Fader::SetFade(STATE state, unsigned int color)
{
	state_ = state;

	switch (state)
	{
	case Fader::STATE::NONE:
		break;
	case Fader::STATE::FADE_IN:
		alpha_ = 255;
		color_ = color;
		break;
	case Fader::STATE::FADE_OUT:
		alpha_ = 0;
		color_ = color;
		break;
	default:
		break;
	}
}
