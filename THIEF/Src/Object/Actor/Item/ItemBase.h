#pragma once

#include <DxLib.h>
#include"ItemInfo.h"

class ItemBase
{
public:

	// アイテムにかける重力
	static constexpr float GRAVITY = 0.25f;

	// 線形補間の係数
	static constexpr float COEFFICIENT = 0.1f;

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

	int GetModelID(void) { return info_.modelId_; }

	// 指定された座標と向きを反映させる
	void SetPosAngle(const VECTOR& pos,const VECTOR& angle);

	// アイテムにダメージを与える
	void SetDamage(int damage);

	// 掴まれた状態にする
	void StartGrabbed(VECTOR localPos);

	// 掴まれた状態を終了する
	void EndGrabbed(void);

	// カメラの座標と向きを参照できるようにする
	void SetCameraPosAngle(VECTOR* cameraPos, VECTOR* cameraAngle);

protected:

	// アイテムの情報
	ItemInfo info_;

	// カメラの座標
	VECTOR* cameraPos_;

	// カメラの向き
	VECTOR* cameraAngle_;

	// 重力をかける
	void Gravity(void);

	// プレイヤーの位置をみて移動処理を行う
	void TrackingPlayer(void);

	// デバッグ用の描画
	void DrawDebug(void);
};

