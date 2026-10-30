#pragma once
#include <DxLib.h>

class WeaponBase
{
public:

	// コンストラクタ
	WeaponBase(void);

	// デストラクタ
	virtual ~WeaponBase(void);

	// 初期化
	void Init(void);

	// 更新
	virtual void Update(void);

	// 描画
	virtual void Draw(void) = 0;

	// 解放
	virtual void Release(void) = 0;

	// 座標の取得
	const VECTOR& GetPos(void) const { return pos_; }

	// 衝突判定用半径
	float GetCollisionRadius(void) const { return collisionRadius_; }

	// 武器を使用する
	virtual void UseWeapon(const VECTOR& pos, const VECTOR& dir) = 0;

	// 生存判定
	bool IsAlive(void) const { return isAlive_; }

	// 生存判定の設定
	void SetAlive(bool isAlive);

protected:

	// 座標
	VECTOR pos_ = {};

	// 衝突判定用半径
	float collisionRadius_ = 0.0f;

	// 移動スピード
	float speed_ = 0.0f;

	// 移動方向
	VECTOR moveDir_ = {};

	// 生存判定
	bool isAlive_ = false;

	// 生存カウント
	float cntAlive_ = 0.0f;

	// 使用時の位置調整オフセット
	VECTOR localPos_ = {};

	// パラメータ設定(純粋仮想関数)
	virtual void SetParam(void) = 0;

	// 移動処理
	virtual void Move(void);
};