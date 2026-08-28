#include <DxLib.h>
#include "../../Application.h"
#include "Loading.h"

Loading::Loading()
{
	// 画像ハンドルの初期化
	for (int i = 0; i < static_cast<int>(NOW_TYPE::MAX); i++)
	{
		handles_[i] = -1;
	}
}

void Loading::Init(void)
{
	// 描画種類の初期化
	nowType_ = Loading0;

	// 最低描画時間の初期化
	loadTimer_ = 0;

	// ローディング中かのフラグを初期化
	isLoading_ = false;

	// 座標の初期化
	pos_ = { 0.0f,0.0f };
}

void Loading::Load(void)
{
	// 1枚の画像を分割して読み込み
	for (int i = 0; i < static_cast<int>(NOW_TYPE::MAX); i++)
	{
		LoadDivGraph(
			"Data/Image/Loading/Loading.png",
			static_cast<int>(NOW_TYPE::MAX),
			DIV_NUM_XY,
			DIV_NUM_XY,
			Application::SCREEN_SIZE_X,
			Application::SCREEN_SIZE_Y,
			handles_);
	}
}

void Loading::Update(void)
{
	loadTimer_++;

	// 読込中のものがなくなったら or 最低ロード時間経過
	if (GetASyncLoadNum() == 0 && loadTimer_ >= MIN_LOAD_TIME)
	{
		// ロード終了
		Init();
	}
	// 読み込み中
	else
	{
		// 40フレームごとに画像の種類を切り替える
		if (loadTimer_ % 40 == 0)
		{
			switch (nowType_)
			{
			case Loading::Loading0:

				// 現在の種類を1へ
				nowType_ = Loading1;

				break;
			case Loading::Loading1:

				// 現在の種類を2へ
				nowType_ = Loading2;

				break;
			case Loading::Loading2:

				// 現在の種類を3へ
				nowType_ = Loading3;

				break;
			case Loading::Loading3:

				// 現在の種類を0へ
				nowType_ = Loading0;

				break;
			default:
				break;
			}
		}
	}
}

void Loading::Draw(void)
{
	// ローディング画面を描画
	DrawGraphF(
		pos_.x,
		pos_.y,
		handles_[static_cast<int>(nowType_)],
		true);
}

void Loading::Release(void)
{
	// 画像ハンドルの解放
	for (int i = 0; i < static_cast<int>(NOW_TYPE::MAX); i++)
	{
		DeleteGraph(handles_[i]);
	}
}

void Loading::StartAsyncLoad(void)
{
	isLoading_ = true;
	// 非同期読み込み開始
	SetUseASyncLoadFlag(true);
}

void Loading::EndAsyncLoad(void)
{
	// 非同期読み込み終了
	SetUseASyncLoadFlag(false);
}
