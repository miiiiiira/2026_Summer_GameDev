#include "ItemBase.h"

ItemBase::ItemBase(void)
{
	info_.modelId_ = -1;
}

ItemBase::~ItemBase(void)
{
}

void ItemBase::Update(void)
{
	// 重力をかけない状態だったら処理を行わない
	if (!info_.isGravity_)return;

	// 重力をかける
	Gravity();
}

void ItemBase::Draw(void)
{
	// モデルを描画
	MV1DrawModel(info_.modelId_);
}

void ItemBase::Release(void)
{
	// モデルハンドルの解放
	MV1DeleteModel(info_.modelId_);
}

const ItemInfo& ItemBase::GetInfo(void)
{
	return info_;
}

void ItemBase::SetPosAngle(const VECTOR& pos, const VECTOR& angle)
{
	// 指定された座標と向きを反映
	info_.pos_ = pos;
	info_.angle_ = angle;

	MV1SetPosition(info_.modelId_, info_.pos_);
	MV1SetRotationXYZ(info_.modelId_, info_.angle_);
}

void ItemBase::SetDamage(int damage)
{
	// 指定のダメージ分お金を削る
	info_.money_ -= damage;

	// お金が0以下になったら
	if (info_.money_ <= 0)
	{
		// 0初期化
		info_.money_ = 0;

		// 生存フラグを折る
		info_.isAlive_ = false;
	}
}

void ItemBase::Gravity(void)
{

	// 座標に重力を反映
	info_.pos_.y += info_.gravity_;
}
