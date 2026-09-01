#pragma once
#include <DxLib.h>
#include "WeaponBase.h"

class WeaponPunch : public WeaponBase
{
public:

	// コンストラクタ
	WeaponPunch(void);

	// デストラクタ
	~WeaponPunch(void);

	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

	// 武器を使用する
	void Use(VECTOR pos, VECTOR dir) override;
protected:
	// 画像やモデルなどのロード
	void Load(void) override;

	// パラメータ設定
	void SetParam(void) override;

private:

	static constexpr float MAX_ALIVE_COUNT = 0.5f;
};