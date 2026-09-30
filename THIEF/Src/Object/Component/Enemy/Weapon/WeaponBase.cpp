#include "WeaponBase.h"

WeaponBase::WeaponBase(void)
{
}

WeaponBase::~WeaponBase(void)
{
}

void WeaponBase::Init(TYPE type)
{
	// 武器種別を設定
	type_ = type;

	// 初期は生存していない
	isAlive_ = false;

	// 画像やモデルなどのロード
	Load();

	// パラメータ設定
	SetParam();

	// モデルの大きさ設定
	MV1SetScale(modelId_, scales_);
}

void WeaponBase::Update(void)
{
	// 生存していない場合は処理を行わない
	if (!isAlive_) return;

	// 移動処理
	Move();
}

void WeaponBase::SetAlive(bool isAlive)
{
	isAlive_ = isAlive;
}

void WeaponBase::Move(void)
{
	// 移動処理（一方方向）
	pos_ = VAdd(pos_, VScale(moveDir_, speed_));
}