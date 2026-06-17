#include <DxLib.h>
#include "../../Application.h"
#include "Loading.h"

// コンストラクタ
Loading::Loading()
	: posX_(0)
	, posY_(0)
	, isLoading_(false)
	, loadTimer_(0)
{
	for (int i = 0; i < static_cast<int>(NOW_TYPE::MAX); i++)
	{
		handles_[i] = -1;
	}
}

// デストラクタ
Loading::~Loading()
{}

// 初期化
void Loading::Init(void)
{
	nowType_ = Loading0;
	loadTimer_ = 0;
	isLoading_ = false;
	posX_ = 0.0f;
	posY_ = 0.0f;
}

// 読み込み
void Loading::Load(void)
{
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

// 更新
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
				nowType_ = Loading1;
				break;
			case Loading::Loading1:
				nowType_ = Loading2;
				break;
			case Loading::Loading2:
				nowType_ = Loading3;
				break;
			case Loading::Loading3:
				nowType_ = Loading0;
				break;
			default:
				break;
			}
		}
	}
}

// 描画
void Loading::Draw(void)
{
	DrawGraphF(
		posX_, posY_,	// 座標
		handles_[static_cast<int>(nowType_)],		// ハンドル
		true			// 透過フラグ
	);
}

// 解放
void Loading::Release(void)
{
	for (int i = 0; i < static_cast<int>(NOW_TYPE::MAX); i++)
	{
		DeleteGraph(handles_[i]);
	}
}

// 非同期読み込みに切り替える
void Loading::StartAsyncLoad(void)
{
	isLoading_ = true;
	// 非同期読み込み開始
	SetUseASyncLoadFlag(true);
}

// 同期読み込みに切り替える
void Loading::EndAsyncLoad(void)
{
	// 非同期読み込み終了
	SetUseASyncLoadFlag(false);
}
