#include "Crosshair.h"
#include "../../Application.h"
#include <DxLib.h>

Crosshair::Crosshair(void)
{
	// 画像ハンドルの初期化
	for (int i = 0; i < static_cast<int>(CROSSHAIR_TYPE::MAX); i++)
	{
		img[i] = -1;
	}
}

Crosshair::~Crosshair(void)
{
}

void Crosshair::Load(void)
{
	// 画像をロード
	img[CROSSHAIR_TYPE::NOT_GRAB] = LoadGraph("Data/Image/GameScene/notGrabCrosshair.png");
	img[CROSSHAIR_TYPE::CAN_GRAB] = LoadGraph("Data/Image/GameScene/canGrabCrosshair.png");
	img[CROSSHAIR_TYPE::GRABBING] = LoadGraph("Data/Image/GameScene/grabbingCrosshair.png");
}

void Crosshair::Init(void)
{
	// 表示する種類の初期化
	type_ = NOT_GRAB;
}

void Crosshair::Update(void)
{
	
}

void Crosshair::Draw(void)
{
	// 画像の中身がなければ処理を行わない
	if (img[type_] == -1)return;
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2,
		1.0, 0.0, img[type_] ,true);
}

void Crosshair::Release(void)
{
	for (int i = 0; i < static_cast<int>(CROSSHAIR_TYPE::MAX); i++)
	{
		DeleteGraph(img[i]);
	}
}

void Crosshair::ChangeCrosshair(const CROSSHAIR_TYPE type)
{
	// 種類を変更
	type_ = type;

	switch (type_)
	{
	case NOT_GRAB:
		break;
	case CAN_GRAB:
		break;
	case GRABBING:
		break;
	case MAX:
		break;
	default:
		break;
	}
}
