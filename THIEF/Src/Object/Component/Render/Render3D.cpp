#include "Render3D.h"
#include <DxLib.h>

#include "../../Object.h"

void Render3D::Init(void)
{
	// オーナーから位置情報を取得
	transform_ = owner_->GetComponent<Transform>();
	isDraw_ = true;
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

void Render3D::Draw3D()
{
	// 描画フラグがfalseだったら処理しない
	if (!isDraw_)return;

	// ロードされていたら
	if (handle_ != -1)
	{
		// モデルを描画
		MV1DrawModel(handle_);
	}

	// ロードされていたら
	if (handles_.size() > 0)
	{
		for (const auto handle : handles_)
		{
			// モデルを描画
			MV1DrawModel(handle);
		}
	}
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

	// ロードされているなら
	if (handles_.size() > 0)
	{
		for (auto handle : handles_)
		{
			// モデルハンドルを削除
			MV1DeleteModel(handle);
		}

		handles_.clear();
	}
}

void Render3D::SetModel(std::string path)
{
	// 既に読み込み済みだったら削除して読み込み
	if (handle_ != -1)
	{
		// モデルハンドルを削除
		MV1DeleteModel(handle_);
		handle_ = -1;
	}

	handle_ = MV1LoadModel(path.c_str());
}

void Render3D::SetModelHandles(std::string path)
{
	// 読み込み
	int handle = MV1LoadModel(path.c_str());
	if (handle != -1)
	{
		handles_.push_back(handle);
	}
}

void Render3D::SetIsDraw(bool flg)
{
	isDraw_ = flg;
}

int Render3D::GetHandles(int index) const
{
	if (index >= 0 && index < handles_.size()) {
		return handles_[index];
	}

	return -1;
}

std::vector<int> Render3D::GetAllHandles(void) const
{
	return handles_;
}
