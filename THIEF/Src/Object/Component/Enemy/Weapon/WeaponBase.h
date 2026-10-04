#pragma once
#include <DxLib.h>

class WeaponBase
{
public:

	// 武器種別
	enum class TYPE
	{
		PUNCH,
		MAX,
	};

	// コンストラクタ
	WeaponBase(void);

	// デストラクタ
	virtual ~WeaponBase(void);

	void Init(TYPE type);			// 初期化
	virtual void Update(void);		// 更新
	virtual void Draw(void) = 0;	// 描画
	virtual void Release(void) = 0;	// 解放

	// 座標の取得
	const VECTOR& GetPos(void) const { return pos_; }

	// 座標の設定
	void SetPos(const VECTOR& pos);

	// 衝突判定用半径
	float GetCollisionRadius(void) const { return collisionRadius_; }

	// 移動スピードの取得
	float GetSpeed(void) const { return speed_; }

	// 武器を使用する
	virtual void UseWeapon(const VECTOR& pos, const VECTOR& dir) = 0;

	// 生存判定
	bool IsAlive(void) const { return isAlive_; }

	// 生存判定の設定
	void SetAlive(bool isAlive);

	// 武器種別の取得
	TYPE GetType(void) const { return type_; }

protected:

	// 武器種別
	TYPE type_ = TYPE::MAX;

	// モデルID
	int modelId_ = -1;

	// 座標
	VECTOR pos_ = {};

	// 角度
	VECTOR angles_ = {};

	// スケール
	VECTOR scales_ = { 1.0f, 1.0f, 1.0f };

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

	// 画像やモデルなどのロード(純粋仮想関数)
	virtual void Load(void) = 0;

	// パラメータ設定(純粋仮想関数)
	virtual void SetParam(void) = 0;

	// 移動処理
	virtual void Move(void);
};