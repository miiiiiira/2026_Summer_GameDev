#include "WeaponBase.h"

WeaponBase::WeaponBase(void)
{
}

WeaponBase::~WeaponBase(void)
{
}

void WeaponBase::Init(TYPE type)
{
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
	if (!isAlive_)
	{
		return;
	}

	// 移動処理
	Move();
}

VECTOR WeaponBase::GetPos(void)
{
	return pos_;
}

void WeaponBase::SetPos(VECTOR pos)
{
	pos_ = pos;
}

float WeaponBase::GetCollisionRadius(void)
{
	return collisionRadius_;
}

float WeaponBase::GetSpeed(void)
{
	return speed_;
}

bool WeaponBase::IsAlive(void)
{
	return isAlive_;
}

void WeaponBase::SetAlive(bool isAlive)
{
	isAlive_ = isAlive;
}

WeaponBase::TYPE WeaponBase::GetType(void)
{
	return type_;
}

void WeaponBase::Move(void)
{
	// 移動処理（一方方向）
	pos_ = VAdd(pos_, VScale(moveDir_, speed_));
}