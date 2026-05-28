#include "Render2D.h"
#include "../../Object.h"
#include <DxLib.h>

void Render2D::Init()
{
	// オーナーから位置情報を取得
	transform_ = owner_->GetComponent<Transform2D>();
}

void Render2D::Draw2D()
{
	// ロードされていないなら
	if (handle_ == -1) return;
	// 位置情報がないなら処理しない
	if (!transform_) return;

	// 2D画像を描画
	DrawRotaGraphF(
		transform_->pos_.x,
		transform_->pos_.y,
		1.0f,
		0.0f,
		handle_,
		true
	);
}

void Render2D::Release(void)
{
	// ロードされているなら
	if (handle_ != -1)
	{
		// モデルハンドルを削除
		DeleteGraph(handle_);
		handle_ = -1;
	}
}

void Render2D::SetModel(std::string path)
{
	// 既に読み込み済みだったら削除して読み込み
	Release();
	handle_ = LoadGraph(path.c_str());
}
