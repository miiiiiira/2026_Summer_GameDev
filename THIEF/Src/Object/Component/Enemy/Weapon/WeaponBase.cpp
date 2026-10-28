#include "WeaponBase.h"

WeaponBase::WeaponBase(void)
{
}

WeaponBase::~WeaponBase(void)
{
}

void WeaponBase::Init(void)
{
	// 初期は生存していない
	isAlive_ = false;

	// パラメータ設定
	SetParam();
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
	// 移動処理
	pos_ = VAdd(pos_, VScale(moveDir_, speed_));
}