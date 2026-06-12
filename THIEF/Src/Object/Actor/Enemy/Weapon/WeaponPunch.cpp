#include "WeaponPunch.h"

WeaponPunch::WeaponPunch(void)
{
}

WeaponPunch::~WeaponPunch(void)
{
}

void WeaponPunch::Draw(void)
{
	if (!isAlive_)
	{
		return;
	}

#ifdef _DEBUG
	// デバッグ用衝突判定
	DrawSphere3D(pos_, collisionRadius_, 10, 0x0000ff, 0x0000ff, false);
#endif // _DEBUG
}

void WeaponPunch::Release(void)
{
}

void WeaponPunch::Use(VECTOR pos, VECTOR dir)
{
	// 武器の高さ調整
	pos_ = VAdd(pos, localPos_);

	moveDir_ = dir;

	isAlive_ = true;
}

void WeaponPunch::Load(void)
{

}

void WeaponPunch::SetParam(void)
{
	// モデルの大きさ
	scales_ = { 1.0f, 1.0f, 1.0f };

	// 移動スピード
	speed_ = 5.0f;

	// 衝突判定用半径
	collisionRadius_ = 60.0f;

	// 使用時の位置調整
	localPos_ = { 0.0f, 90.0f, 0.0f };

	cntAlive_ = 0;
}