#include "../../../../Scene/SceneManager.h"

#include "WeaponPunch.h"

WeaponPunch::WeaponPunch(void)
{
}

WeaponPunch::~WeaponPunch(void)
{
}

void WeaponPunch::Update(void)
{
	if (isAlive_)
	{
		// 生存時間のカウント
		cntAlive_ += SceneManager::GetInstance()->GetDeltaTime();
	}
	else
	{
		// 生存していない場合はカウントをリセット
		cntAlive_ = 0.0f;
	}

	// 生存時間が最大値を超えた場合は非生存状態にする
	if (cntAlive_ >= MAX_ALIVE_COUNT)
	{
		isAlive_ = false;	// 非生存状態にする
		cntAlive_ = 0.0f;	// カウントをリセット
	}

	// 移動処理
	WeaponBase::Update();
}

void WeaponPunch::Draw(void)
{
	// 生存していない場合は描画しない
	if (!isAlive_) return;

#ifdef _DEBUG
	// デバッグ用衝突判定
	DrawSphere3D(pos_, collisionRadius_, 10, 0x0000ff, 0x0000ff, false);
#endif // _DEBUG
}

void WeaponPunch::Release(void)
{
}

void WeaponPunch::UseWeapon(VECTOR pos, VECTOR dir)
{
	// 武器の高さ調整
	pos_ = VAdd(pos, localPos_);

	// 移動方向を設定
	moveDir_ = dir;

	// 生存状態にする
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
	speed_ = 8.0f;

	// 衝突判定用半径
	collisionRadius_ = 100.0f;

	// 使用時の位置調整
	localPos_ = { 0.0f, 90.0f, 0.0f };

	// 生存時間のカウントを初期化
	cntAlive_ = 0;
}