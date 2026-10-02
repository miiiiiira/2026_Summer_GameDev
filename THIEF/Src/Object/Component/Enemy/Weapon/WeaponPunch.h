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

	void Update(void) override;		// 更新
	void Draw(void) override;		// 描画
	void Release(void) override;	// 解放

	// 武器を使用する
	void UseWeapon(VECTOR pos, VECTOR dir) override;

protected:

	// 画像やモデルなどのロード
	void Load(void) override;

	// パラメータ設定
	void SetParam(void) override;

private:

	// 最大生存時間
	static constexpr float MAX_ALIVE_COUNT = 0.5f;
};