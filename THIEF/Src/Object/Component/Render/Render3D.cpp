#include "Render3D.h"
#include <DxLib.h>

#include "../../Object.h"

void Render3D::Init(void)
{
	// オーナーから位置情報を取得
	transform_ = owner_->GetComponent<Transform>();
}

void Render3D::Update(void)
{
	// ロードされていないなら処理しない
	if (handle_ == -1) return;
	// 位置情報がないなら処理しない
	if (!transform_) return;

	// モデルの座標を更新
	MV1SetPosition(handle_, transform_->pos_);
}

void Render3D::Draw()
{
	// ロードされていないなら処理しない
	if (handle_ == -1) return;

	// モデルを描画
	MV1DrawModel(handle_);
}

void Render3D::Release(void)
{
	// ロードされているなら
	if (handle_ != -1)
	{
		// モデルハンドルを削除
		MV1DeleteModel(handle_);
		handle_ = -1;
	}
}

void Render3D::SetModel(std::string path)
{
	// 既に読み込み済みだったら削除して読み込み
	Release();
	handle_ = MV1LoadModel(path.c_str());
}
