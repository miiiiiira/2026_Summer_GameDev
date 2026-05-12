#pragma once

#include <DxLib.h>
#include"ItemInfo.h"

class ItemBase
{
public:

	// アイテムにかける重力
	static constexpr float GRAVITY = 0.25f;

	// コンストラクタ
	ItemBase(void);
	// デストラクタ
	virtual ~ItemBase(void);

	// 読み込み処理
	virtual void Load(void) = 0;
	// 初期化処理
	virtual void Init(void) = 0;
	// 更新処理
	void Update(void);
	// 描画処理
	void Draw(void);
	// 解放処理
	void Release(void);

	// アイテムの情報を渡す
	const ItemInfo& GetInfo(void);
	// 指定された座標と向きを反映させる
	void SetPosAngle(const VECTOR& pos,const VECTOR& angle);

	// アイテムにダメージを与える
	void SetDamage(int damage);

protected:

	// アイテムの情報
	ItemInfo info_;

	// 重力をかける
	void Gravity(void);

	// デバッグ用の描画
	void DrawDebug(void);
};

