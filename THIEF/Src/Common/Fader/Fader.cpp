#include <DxLib.h>
#include "../../Application.h"
#include "../../Scene/SceneManager.h"
#include "Fader.h"

Fader* Fader::instance_ = nullptr;

Fader::Fader(void)
{
	// シャッター画像読み込み
	for (int i = 0; i < SHUTTER_IMG_NUM; ++i)
	{
		shutterImg_[i] = LoadGraph("Data/Image/Fade/shutter.png");
	}
}

Fader::~Fader(void)
{
	// シャッター画像解放
	for (int i = 0; i < SHUTTER_IMG_NUM; ++i)
	{
		DeleteGraph( shutterImg_[i]);
	}
}

void Fader::Init(void)
{
	state_ = STATE::NONE;
	type_ = TYPE::NORMAL;
	alpha_ = 0;
	color_ = 0x000000;
}

void Fader::Update(void)
{
	switch (state_)
	{
	case STATE::NONE:
		return;

	case STATE::FADE_OUT:

		// アルファ値を増やす
		alpha_ += SPEED_ALPHA;

		// 最大アルファ値に達したら
		if (alpha_ > ALPHA_MAX)
		{
			// フェード終了
			state_ = STATE::END;
		}

		break;

	case STATE::FADE_IN:

		// アルファ値を減らす
		alpha_ -= SPEED_ALPHA;

		// 最低アルファ値に達したら
		if (alpha_ < 0)
		{
			// フェード終了
			state_ = STATE::END;
		}

		break;

	case STATE::END:

		// 初期化
		Init();

		break;
	default:
		return;
	}

}

void Fader::Draw(void)
{
	// フェード中でないなら描画しない
	if (state_ == STATE::NONE ||
		state_ == STATE::END)
	{
		return;
	}

	// 描画範囲
	const int screenX = Application::SCREEN_SIZE_X;
	const int screenY = Application::SCREEN_SIZE_Y;

	const float rate = alpha_ / 255.0f;

	int x = 0;
	int h = 0;
	int w = 0;

	switch (type_)
	{
	case Fader::TYPE::NORMAL:

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_);

		// 画面全体に指定された色のボックスを描画
		DrawBox(
			0, 0,
			Application::SCREEN_SIZE_X,
			Application::SCREEN_SIZE_Y,
			color_, true);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		break;
	case Fader::TYPE::SHUTTER:
	{
		int sizeX = SHUTTER_SIZE_WID / 2;
		int sizeY = SHUTTER_SIZE_HIG / 2;
		h = static_cast<int>(Application::SCREEN_SIZE_Y * 0.5 * rate);

		// シャッターの画像
		DrawRotaGraph(sizeX, -sizeY + h, 1.0, 0.0, shutterImg_[0], true);
		DrawRotaGraph(sizeX, Application::SCREEN_SIZE_Y + sizeY - h, 1.0, 0.0, shutterImg_[1], true);
	}
		break;
	case Fader::TYPE::WIPE:

		w = static_cast<int>(screenX * 0.5f * rate);

		DrawBox(0, 0, w, screenY, color_, true);
		DrawBox(screenX - w, 0, screenX, screenY, color_, true);

		break;
	case Fader::TYPE::CROSS:

		h = static_cast<int>(screenY * 0.5f * rate);
		w =	static_cast<int>(screenX * 0.5f * rate);

		DrawBox(0, 0, screenX, h, color_,true);
		DrawBox(0, screenY - h, screenX, screenY, color_, true);
		DrawBox(0, 0, w, screenY, color_, true);
		DrawBox(screenX - w, 0, screenX, screenY, color_, true);

		break;
	default:
		break;
	}
}

Fader::STATE Fader::GetState(void)
{
	return state_;
}

void Fader::SetFade(STATE state, TYPE type, unsigned int color)
{
	state_ = state;
	type_ = type;

	switch (state)
	{
	case Fader::STATE::NONE:
		break;
	case Fader::STATE::FADE_IN:
		alpha_ = ALPHA_MAX;
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
