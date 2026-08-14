#include "Crosshair.h"
#include "../../../Application.h"
#include <DxLib.h>

Crosshair::Crosshair(void)
{
	// 画像ハンドルの初期化
	for (int i = 0; i < static_cast<int>(CROSSHAIR_TYPE::CROSSHAIR_MAX); i++)
	{
		img[i] = -1;
	}

	// 画像をロード
	img[CROSSHAIR_TYPE::CROSSHAIR_NOT_GRAB] = LoadGraph("Data/Image/GameScene/notGrabCrosshair.png");
	img[CROSSHAIR_TYPE::CROSSHAIR_CAN_GRAB] = LoadGraph("Data/Image/GameScene/canGrabCrosshair.png");
	img[CROSSHAIR_TYPE::CROSSHAIR_GRABBING] = LoadGraph("Data/Image/GameScene/grabbingCrosshair.png");
}

Crosshair::~Crosshair(void)
{
	// 画像の解放
	for (int i = 0; i < static_cast<int>(CROSSHAIR_TYPE::CROSSHAIR_MAX); i++)
	{
		DeleteGraph(img[i]);
	}
}

void Crosshair::Init(void)
{
	// 表示する種類の初期化
	type_ = CROSSHAIR_NOT_GRAB;
}

void Crosshair::Draw2D(void)
{
	// 画像の中身がなければ処理を行わない
	if (img[type_] == -1)return;

	// 画面の中心位置にクロスヘアを描画
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2,
		1.0, 0.0, img[type_] ,true);
}

void Crosshair::ChangeCrosshair(const CROSSHAIR_TYPE type)
{
	// 種類を変更
	type_ = type;
}
